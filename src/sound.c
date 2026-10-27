// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Sound.

#include "objects.h"
#include "calendar.h"
#include "state_types.h"
#include "sound_types.h"
#include "sound.h"
#include "initial_data.h"
#include "render_types.h"
#include "frame.h"
#include "runtime_state.h"
#include "files.h"
#include "graphics.h"
#include <stdio.h>
#include <string.h>
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wstatic-in-inline"
#endif
#include "../vendor/nuked-opl3/opl3.c"
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#include "../vendor/pt_opl_adapter.h"

// Fixed audio resource owners are independent of any saved arena or registry.

PTSoundResources pt_sound_resources;
void pt_native_sound_defaults_apply(void) {
  pt_sound_resources = (PTSoundResources){.fm_status = &pt_native_globals.initial_music_status};
}

// Each runtime resource has one live allocation; borrowed cursors never own it.

static u8 *sound_buffers[PT_SOUND_BUFFER_COUNT];
static const u32 sound_buffer_sizes[PT_SOUND_BUFFER_COUNT] = {[PT_SOUND_BUFFER_STATUS] = 16,
  [PT_SOUND_BUFFER_MUSIC] = 64000,
  [PT_SOUND_BUFFER_BANK] = PT_SOUND_BANK_CAPACITY,
  [PT_SOUND_BUFFER_INTRO] = 15000};

static PTPCMResident *create_pcm_resident(const u8 *);
static const PTSoundVector initial_sound_vector = {.endpoint_kind = PT_SOUND_ENDPOINT_NONE, .valid_asset = true};
static PTSoundVector *native_sound_vector;
void pt_sound_vector_storage_reset(void) {
  if (pt_sound_resources.bridge == native_sound_vector)
    pt_sound_resources.bridge = NULL;
  free(native_sound_vector);
  native_sound_vector = NULL;
}
static PTSoundVector *create_sound_vector(void) {
  pt_sound_vector_storage_reset();
  native_sound_vector = malloc(sizeof(*native_sound_vector));
  if (!native_sound_vector)
    abort();
  *native_sound_vector = initial_sound_vector;
  return native_sound_vector;
}

u32 pt_sound_buffer_size(PTSoundBufferKind kind) {
  if ((unsigned)kind >= PT_SOUND_BUFFER_COUNT)
    abort();
  return sound_buffer_sizes[kind];
}
u8 *pt_sound_buffer_allocate(PTSoundBufferKind kind) {
  u32 size = pt_sound_buffer_size(kind);
  if (sound_buffers[kind]) {
    fputs("Pizza Tycoon: audio resource is already allocated\n", stderr);
    abort();
  }
  u8 *buffer = calloc(1, size);
  if (buffer)
    sound_buffers[kind] = buffer;
  return buffer;
}
static void retire_sound_buffer(u8 *buffer, size_t size) {
  u8 **borrowed[] = {&pt_sound_resources.bank,
    &pt_sound_resources.fm_status,
    &pt_sound_resources.music,
    &pt_sound_resources.cursor,
    &pt_sound_resources.driver,
    &pt_sound_resources.vector,
    &pt_sound_resources.repeat_start,
    &pt_sound_resources.output,
    &pt_sound_resources.repeat_end,
    &pt_sound_resources.samples};
  for (unsigned i = 0; i < sizeof(borrowed) / sizeof(*borrowed); ++i)
    if (*borrowed[i] && (uintptr_t)*borrowed[i] - (uintptr_t)buffer < size)
      *borrowed[i] = NULL;
  for (unsigned i = 0; i < sizeof(pt_sound_resources.dos_blocks) / sizeof(*pt_sound_resources.dos_blocks); ++i)
    if (pt_sound_resources.dos_blocks[i] && (uintptr_t)pt_sound_resources.dos_blocks[i] - (uintptr_t)buffer < size)
      pt_sound_resources.dos_blocks[i] = NULL;
}
void pt_sound_buffer_release(u8 *buffer) {
  if (!buffer)
    return;
  for (unsigned kind = 0; kind < PT_SOUND_BUFFER_COUNT; ++kind) {
    if (sound_buffers[kind] != buffer)
      continue;
    retire_sound_buffer(buffer, pt_sound_buffer_size((PTSoundBufferKind)kind));
    sound_buffers[kind] = NULL;
    free(buffer);
    return;
  }
  fputs("Pizza Tycoon: release of an unowned audio resource\n", stderr);
  abort();
}
void pt_sound_buffer_storage_reset(void) {
  for (unsigned kind = 0; kind < PT_SOUND_BUFFER_COUNT; ++kind)
    pt_sound_buffer_release(sound_buffers[kind]);
}
void pt_native_sound_storage_reset(void) {
  pt_pcm_resident_storage_reset();
  pt_sound_vector_storage_reset();
  pt_sound_dma_transfer_reset();
  pt_sound_buffer_storage_reset();
}
static void require_dos_block(unsigned index) {
  if (index >= sizeof(pt_sound_resources.dos_blocks) / sizeof(*pt_sound_resources.dos_blocks)) {
    fprintf(stderr, "Pizza Tycoon: invalid audio block index %u (count 256)\n", index);
    abort();
  }
}

void pt_sound_pcm_driver_image_set(const u8 *buffer) {
  if (buffer != pt_sound_resources.driver)
    pt_sound_resources.resident = NULL;
  pt_sound_resources.driver = (u8 *)buffer;
  if (!buffer) {
    pt_pcm_resident_storage_reset();
    pt_sound_resources.resident = NULL;
  } else if (!pt_sound_resources.resident) {
    pt_sound_resources.resident = create_pcm_resident(buffer);
  }
}
void pt_sound_vector_image_set(const u8 *buffer) {
  if (buffer != pt_sound_resources.vector)
    pt_sound_resources.bridge = NULL;
  pt_sound_resources.vector = (u8 *)buffer;
  if (!buffer) {
    pt_sound_vector_storage_reset();
    pt_sound_resources.bridge = NULL;
  } else if (!pt_sound_resources.bridge) {
    pt_sound_resources.bridge = create_sound_vector();
  }
}

// Cursors are owned directly by the live driver.

PTFMBuffer pt_fm_buffer_rebase(PTFMBuffer buffer, u32 bytes) {
  if (bytes > buffer.capacity || (!buffer.base && bytes)) {
    fputs("Pizza Tycoon: FM rebase exceeds its native buffer\n", stderr);
    abort();
  }
  if (bytes)
    buffer.base += bytes;
  buffer.capacity -= bytes;
  return buffer;
}

static PTFMDeviceBindings fm_devices;

void pt_fm_devices_reset(void) {
  fm_devices = (PTFMDeviceBindings){0};
}

void pt_fm_device_bind(u8 interrupt, PTFMResident *driver) {
  fm_devices.drivers[interrupt] = driver;
}
void pt_fm_timer_bind(PTFMResident *driver) {
  fm_devices.timer = driver;
}
PTFMResident *pt_fm_device(u8 interrupt) {
  PTFMResident *driver = fm_devices.drivers[interrupt];
  if (!driver)
    pt_runtime_interrupt_unavailable(interrupt);
  return driver;
}
bool pt_fm_device_probe(u8 interrupt) {
  bool present = fm_devices.drivers[interrupt] != NULL;
  return present;
}
PTFMBuffer pt_fm_resource(const u8 *bytes, u32 capacity) {
  if (!bytes && capacity)
    abort();
  // A music cursor uses file-relative windows, independent of allocation
  // alignment. Standalone resources begin at their supplied byte pointer.

  const u8 *music = pt_sound_resources.music;
  uintptr_t offset = (uintptr_t)bytes - (uintptr_t)music;
  if (music && offset < pt_sound_buffer_size(PT_SOUND_BUFFER_MUSIC)) {
    if (capacity > pt_sound_buffer_size(PT_SOUND_BUFFER_MUSIC) - offset)
      abort();
    return (PTFMBuffer){music + (offset & ~(uintptr_t)15), (u16)(offset & 15u), capacity + (u32)(offset & 15u)};
  }
  return (PTFMBuffer){bytes, 0, capacity};
}
static u32 fm_timer_tick(PTFMResident *driver);
static PTPCMPlaybackState pcm_driver;
static PTPCMResident *native_pcm_resident;
static const u8 *native_pcm_image;
void pt_pcm_resident_storage_reset(void) {
  if (pt_sound_resources.resident == native_pcm_resident)
    pt_sound_resources.resident = NULL;
  free(native_pcm_resident);
  native_pcm_resident = NULL;
  native_pcm_image = NULL;
}
static PTSoundEndpoint sound_endpoint = {.bound = true};
void pt_sound_endpoint_bind(PTSoundEndpointKind kind, PTSoundEndpointKind previous_pcm) {
  if ((unsigned)kind > PT_SOUND_ENDPOINT_MIDI || (unsigned)previous_pcm > PT_SOUND_ENDPOINT_MIDI)
    abort();
  sound_endpoint = (PTSoundEndpoint){true, kind, previous_pcm};
}

PTPCMSampleBuffer pt_pcm_resource(const void *bytes) {
  return (PTPCMSampleBuffer){bytes};
}

static PTPCMResident *create_pcm_resident(const u8 *image) {
  if (!image) {
    fputs("Pizza Tycoon: missing native PCM device\n", stderr);
    abort();
  }
  if (image == pt_sound_resources.driver && pt_sound_resources.resident)
    return pt_sound_resources.resident;
  if (image != native_pcm_image || !native_pcm_resident) {
    pt_pcm_resident_storage_reset();
    native_pcm_resident = malloc(sizeof(*native_pcm_resident));
    if (!native_pcm_resident)
      abort();
    *native_pcm_resident = pt_pcm_initial_state;
    native_pcm_image = image;
  }
  return native_pcm_resident;
}

void pt_pcm_driver_restore(const PTPCMPlaybackState *state) {
  pcm_driver = *state;
}
static void require_pcm_transfer(const PTSoundDMATransfer *record) {
  if (!record) {
    fprintf(stderr, "Missing native PCM transfer\n");
    abort();
  }
}
PTPCMSampleBuffer pt_pcm_transfer_samples(const PTSoundDMATransfer *record) {
  require_pcm_transfer(record);
  return record->samples;
}
PTSoundDMATransfer *pt_pcm_transfer_allocate(void) {
  PTSoundDMATransfer *record = calloc(1, sizeof(*record));
  return record;
}

static PTSoundDMATransfer *native_dma_transfer;
void pt_sound_dma_transfer_reset(void) {
  if (pt_sound_resources.dma_record == native_dma_transfer)
    pt_sound_resources.dma_record = NULL;
  free(native_dma_transfer);
  native_dma_transfer = NULL;
}
PTSoundDMATransfer *pt_sound_dma_transfer_create(void) {
  pt_sound_dma_transfer_reset();
  native_dma_transfer = pt_pcm_transfer_allocate();
  return native_dma_transfer;
}

static void pcm_queue_transfer(const PTSoundDMATransfer *record) {
  require_pcm_transfer(record);
  pcm_driver.queued_samples = pt_pcm_transfer_samples(record);
  pcm_driver.queued_length = record->length ? record->length : 65536u;
  pcm_driver.queued_frequency = record->frequency;
}
u8 *DpmiAllocateDosBlock(u32 bytes, u16 *selector_address) {
  u16 selector;
  // Preserve the original wrapping ADD, logical shift, and WORD DPMI input.

  u8 *result = pt_dos_memory_allocate((u16)((bytes + 15u) >> 4), &selector);
  if (result) {
    *selector_address = selector;
  }

  return result;
}
u32 DpmiFreeDosBlock(u32 selector) {
  return pt_dos_memory_free((u16)selector);
}
u8 *AllocateSoundDOSBlock(u32 bytes) {
  u8 *result = NULL;

  u32 index = sound_dos_block_count;
  if (index < 256) {
    result = DpmiAllocateDosBlock(bytes, &sound_dos_selectors[index]);
    if (result) {
      pt_sound_resources.dos_blocks[index] = result;
      sound_dos_block_count = index + 1u;
    }
  }

  return result;
}
u32 FreeSoundDOSBlock(const void *address) {
  u32 result = 0;

  while (result < sound_dos_block_count) {
    u32 index = result;
    require_dos_block(index);
    if (pt_sound_resources.dos_blocks[index] == address) {
      (void)DpmiFreeDosBlock((u32)(i32)(i16)sound_dos_selectors[index]);
      u32 last = --sound_dos_block_count;
      require_dos_block(last);
      pt_sound_resources.dos_blocks[index] = pt_sound_resources.dos_blocks[last];
      result = sound_dos_selectors[last];
      sound_dos_selectors[index] = (u16)result;
      break;
    }
    ++result;
  }

  return result;
}
u8 *LoadSoundDOSFile(const u8 *filename, PTSoundFileSize *length) {
  u8 *result = NULL;
  GuestFileStream *stream;
  stream = GuestFopen(filename, sound_binary_read_mode);

  if (stream) {
    (void)GuestFseek(stream, 0, 2);
    u32 size;
    size = GuestFtell(stream);

    (void)GuestFseek(stream, 0, 0);
    result = AllocateSoundDOSBlock(size);
    if (result) {
      (void)GuestFread(result, size, 1, stream);
    }
    (void)GuestFclose(stream);
    length->bytes = size;
  }

  return result;
}
// VECTOR.COM is a platform bridge, not a game-state substitute. The native
// adapter reproduces its initializer's DOS vector query and four WORD stores.
// Diagnostic execution runs the actual 16-bit initializer independently.

static bool fm_delta_initialized;
static bool fm_start_automatically;

u32 InstallSoundVector(PTSoundVector *image) {
  u32 result = 0;

  if (!image->valid_asset)
    goto done;

  image->endpoint_kind = PT_SOUND_ENDPOINT_OTHER;
  if (sound_endpoint.bound)
    sound_endpoint.kind = image->endpoint_kind;

  result = 1;
done:;

  return result;
}
u32 ProbePCMDriver(void) {
  if (sound_endpoint.kind == PT_SOUND_ENDPOINT_MIDI) {
    if (pt_frame_worker())
      pt_frame_unsupported(PT_FRAME_MIDI_UNAVAILABLE);
    abort();
  }
  return sound_endpoint.kind == PT_SOUND_ENDPOINT_PCM;
}
u32 LoadPCMDriver(const u8 *filename) {
  PTSoundFileSize length;
  u32 result = 0;
  if (sound_pcm_driver_loaded)
    goto done;
  result = ProbePCMDriver();
  if ((u16)result) {
    sound_pcm_driver_external = 1;
    sound_pcm_driver_loaded = 1;
    result = 1;
  } else {
    pt_sound_pcm_driver_image_set(LoadSoundDOSFile(filename, &length));
    if (pt_sound_resources.driver)
      sound_pcm_driver_loaded = 1;
    result = sound_pcm_driver_loaded;
  }
done:;

  return result;
}
u32 EnsureSoundVector(void) {
  PTSoundFileSize length;
  u32 result = 0;
  if (!pt_sound_resources.vector) {
    pt_sound_vector_image_set(LoadSoundDOSFile(sound_vector_filename, &length));
    if (pt_sound_resources.vector) {
      u32 installed;
      installed = InstallSoundVector(pt_sound_resources.bridge);
      if (!installed) {
        (void)FreeSoundDOSBlock(pt_sound_resources.vector);
        pt_sound_vector_image_set(NULL);
      }
    }
  }
  if (pt_sound_resources.vector)
    result = 1;

  return result;
}

// Initialize the native device and logical playback state.

u32 InstallPCMDriver(PTPCMResident *image) {
  u32 result = 0;

  if (memcmp(image->signature, "DIGPAK", sizeof(image->signature)))
    goto done;

  PTPCMResident *driver = image;

  if (sound_endpoint.bound) {
    sound_endpoint.previous_pcm = sound_endpoint.kind;
    sound_endpoint.kind = PT_SOUND_ENDPOINT_PCM;
  }

  driver->playback.extended_dsp = 1;
  // Installing a device ends its previous playback.

  pcm_driver.installed = 1;
  pcm_driver.active = driver->playback.playing;
  pcm_driver.queued = driver->playback.queued;
  pcm_queue_transfer(&driver->playback.queued_record);
  pcm_driver.current_samples = (PTPCMSampleBuffer){0};
  pcm_driver.current_length = pcm_driver.current_frequency = pcm_driver.current_position = 0;
  pcm_driver.block_size = pcm_driver.block_position = 0;
  result = 1;
done:;

  return result;
}
static PTPCMResident *pcm_live_image(void) {
  PTPCMResident *image = pt_sound_resources.driver ? pt_sound_resources.resident : NULL;

  return image;
}

u32 QueryPCMDriverVersion(void) {
  PTPCMResident *image = pcm_live_image();

  return image->version;
}
u32 QueryPCMDriverCapabilities(void) {
  PTPCMResident *image = pcm_live_image();

  u32 result = 0xdc1u | (image->playback.extended_dsp == 1 ? 0x200u : 0u);

  return result;
}
void SetPCMDriverFlatPointers(u32 enabled) {
  PTPCMResident *image = pcm_live_image();

  image->playback.flat_pointers = (u16)enabled;

  return;
}
u32 UninstallPCMDriver(PTPCMResident *image) {
  PTPCMResident *driver = image;
  PTPCMPlayback *p = &driver->playback;

  p->queued = 0;
  p->stop_count = 0;
  p->callback_enabled = 0;
  if (sound_endpoint.bound)
    sound_endpoint.kind = sound_endpoint.previous_pcm;

  pcm_driver.installed = 0;
  pcm_driver.active = p->playing;
  pcm_driver.queued = p->queued;
  // The COM image came from a DPMI block, without a DOS PSP/MCB. Its two
  // DOS AH=49 frees fail with error 9; the wrapper consequently returns 0.
  // UnloadPCMDriver performs the actual DPMI free afterwards.

  return 0;
}
u32 TerminatePCMDriver(void) {
  u32 result = 0;
  if (sound_pcm_driver_ready) {
    (void)SetPCMDriverFlatPointers(0);
    if (sound_pcm_driver_external)
      sound_pcm_driver_external = 0;
    else
      (void)UninstallPCMDriver(pt_sound_resources.resident);
    sound_pcm_driver_ready = 0;
    result = 1;
  }

  return result;
}
u32 UnloadPCMDriver(void) {
  if (!sound_pcm_driver_loaded)
    return 0;
  if (sound_pcm_driver_ready)
    (void)TerminatePCMDriver();
  if (pt_sound_resources.driver)
    (void)FreeSoundDOSBlock(pt_sound_resources.driver);
  pt_sound_pcm_driver_image_set(NULL);
  sound_pcm_driver_loaded = 0;
  return 1;
}
u32 InitializePCMDriver(void) {
  u32 result;
  if (sound_pcm_driver_loaded) {
    if (sound_pcm_driver_external)
      sound_pcm_driver_ready = 1;
    else {
      result = EnsureSoundVector();
      if (!result) {
        sound_pcm_driver_ready = 0;
        goto done;
      }
      result = InstallPCMDriver(pt_sound_resources.resident);
      sound_pcm_driver_ready = (u32)(i32)(i16)((u16)result);
    }
  }
  if (sound_pcm_driver_ready) {
    u32 version, capabilities;
    version = QueryPCMDriverVersion();
    capabilities = QueryPCMDriverCapabilities();
    if ((capabilities & 8u) || (i16)((u16)version) < 340)
      (void)TerminatePCMDriver();
    else
      (void)SetPCMDriverFlatPointers(1);
  }
done:;
  result = sound_pcm_driver_ready;

  return result;
}
// These platform services are implemented from the bundled SOUNDRV.COM.
// PT_PCM_STATUS reports idle/active/queued; PT_PCM_COMMIT is a no-op in that driver.
// Queue submission implements the resident SB16 start/queued/busy protocol.
// DMA block completion follows its IRQ and queued-buffer callback.

static u8 pcm_output[PT_PCM_OUTPUT_CAPACITY];
static size_t pcm_output_size;
static PTSoundTimer sound_timer;
static PTFMOutputState fm_output;
const PTPCMPlaybackState *pt_pcm_driver_state(void) {
  return &pcm_driver;
}
const PTFMOutputState *pt_fm_output_state(void) {
  return &fm_output;
}
bool pt_fm_output_restore(const PTFMOutputState *state) {
  if (!state)
    return false;
  fm_output = *state;
  return true;
}
const PTSoundTimer *pt_sound_timer_state(void) {
  return &sound_timer;
}
static PTOplState fm_synth;
static i16 mixed_output[PT_PCM_OUTPUT_CAPACITY * 4];
static size_t mixed_output_size;

bool pt_fm_synth_load(const void *data, size_t size) {
  return !pt_frame_active() && pt_opl_load(&fm_synth, data, size);
}
bool pt_audio_output(void *data, size_t capacity, size_t *size) {
  if (!size || capacity < mixed_output_size || (!data && mixed_output_size))
    return false;
  if (mixed_output_size)
    memcpy(data, mixed_output, mixed_output_size);
  *size = mixed_output_size;
  return true;
}
bool pt_audio_load_output(const void *data, size_t size) {
  if (pt_frame_active() || size > sizeof(mixed_output) || (size & 1u) || (!data && size))
    return false;
  if (size)
    memcpy(mixed_output, data, size);
  mixed_output_size = size;
  return true;
}
static void audio_mix(u32 count) {
  mixed_output_size = (size_t)count * 8u;
  for (unsigned i = 0; i < count; ++i)
    for (unsigned phase = 0; phase < 4; ++phase) {
      i32 sample = ((i32)pcm_output[i] - 128) * 256;
      if (fm_output.enabled)
        sample += pt_opl_sample(&fm_synth);
      if (sample < -32768)
        sample = -32768;
      else if (sample > 32767)
        sample = 32767;
      mixed_output[4u * i + phase] = (i16)sample;
    }
}

void pt_pcm_driver_reset_platform(void) {
  // Reset the hardware adapter without destroying selected resource owners.

  if (!pt_sound_resources.bridge)
    pt_sound_vector_storage_reset();
  if (!pt_sound_resources.resident)
    pt_pcm_resident_storage_reset();
  pt_sound_endpoint_bind(PT_SOUND_ENDPOINT_NONE, PT_SOUND_ENDPOINT_NONE);
  pt_fm_devices_reset();
  fm_delta_initialized = false;
  fm_start_automatically = false;
  pcm_driver = (PTPCMPlaybackState){0};
  pcm_output_size = 0;
  fm_output = (PTFMOutputState){0};
  pt_opl_reset(&fm_synth);
  mixed_output_size = 0;
  sound_timer =
      (PTSoundTimer){.handler = pt_sound_timer_bios_handler(), .previous_handler = pt_sound_timer_bios_handler()};
}
bool pt_sound_timer_restore(const PTSoundTimer *state) {
  if (!state || state->reload > UINT16_MAX || state->counter > UINT16_MAX)
    return false;
  sound_timer = *state;
  return true;
}

static void pcm_timer_callback(void *context) {
  (void)context;
  SoundPCMCallback();
}
PTRuntimeCallback pt_sound_pcm_timer_callback(void) {
  return (PTRuntimeCallback){pcm_timer_callback, NULL};
}
PTRuntimeCallback InstallSoundTimer(PTRuntimeCallback callback, u32 period) {
  sound_timer_period = (u16)period;
  pt_runtime.sound_timer = callback;
  sound_timer_active = 0;
  // Scanning all WORD counter values always yields the same maximum.

  sound_timer_previous_period = UINT16_MAX;
  sound_timer.previous_handler = sound_timer.handler;

  sound_timer.reload = (u16)period;
  sound_timer.counter = (u16)period;
  sound_timer.handler = pt_sound_timer_callback_handler();

  return callback;
}
u8 SoundBCDToBinary(u8 value) {
  return (u8)(((value >> 4) * 10u) + (value & 15u));
}
void RestoreSoundTimer(void) {
  sound_timer.reload = sound_timer.counter = sound_timer_previous_period;

  sound_timer.handler = sound_timer.previous_handler;
  u32 cx, dx;
  pt_frame_poll(PT_FRAME_POLL_SOUND_ENTER);
  pt_dos_read_rtc(&cx, &dx);
  u8 first = (u8)(dx >> 8);
  do {
    pt_frame_poll(PT_FRAME_POLL_SOUND_LEAVE);
    pt_dos_read_rtc(&cx, &dx);
  } while ((u8)(dx >> 8) == first);
  u8 hours, minutes, seconds;
  hours = SoundBCDToBinary((u8)(cx >> 8));
  minutes = SoundBCDToBinary((u8)cx);
  seconds = SoundBCDToBinary((u8)(dx >> 8));
  pt_dos_set_time(((u32)hours << 8) | minutes, (u32)seconds << 8);
}

static u32 pcm_service(bool commit) {
  const bool playing =
      pt_sound_resources.driver ? pt_sound_resources.resident->playback.playing != 0 : pcm_driver.active != 0;
  const bool queued =
      pt_sound_resources.driver ? pt_sound_resources.resident->playback.queued != 0 : pcm_driver.queued != 0;
  u32 result = commit || !playing ? 0u : queued ? 2u : 1u;
  return result;
}

u32 pt_pcm_buffer_length(u32 word) {
  return (u16)word ? (u16)word : 65536u;
}
PTPCMPlayback *pt_pcm_playback(PTPCMResident *image) {
  return &image->playback;
}

static void pcm_resident_begin(PTPCMResident *image) {
  PTPCMPlayback *p = pt_pcm_playback(image);

  p->active_half = 1;
  p->playing = 1;
}
static void pcm_start(PTPCMSampleBuffer buffer, u32 length, u32 frequency) {
  pcm_driver.current_samples = buffer;
  pcm_driver.current_length = pt_pcm_buffer_length(length);
  pcm_driver.current_frequency = frequency;
  pcm_driver.current_position = 0;
  pcm_driver.block_size = pcm_driver.current_length;
  if (pt_sound_resources.driver && pt_pcm_playback(pt_sound_resources.resident)->auto_dma == 1)
    pcm_driver.block_size = pcm_driver.current_length / 2u;
  pcm_driver.block_position = 0;
  pcm_driver.active = 1;
}

bool pt_pcm_driver_load_output(const void *data, size_t size) {
  if (pt_frame_active() || size > sizeof(pcm_output) || (!data && size))
    return false;
  if (size)
    memcpy(pcm_output, data, size);
  pcm_output_size = size;
  return true;
}
// Commands advance the sample clock over native byte buffers. The SDL
// audio thread consumes output without mutating playback state.

PTCommandStatus pt_pcm_samples_ready(u32 count) {
  if (count > PT_PCM_OUTPUT_CAPACITY)
    return PT_COMMAND_INVALID_ARGUMENT;
  if (!pcm_driver.installed)
    return fm_output.enabled ? PT_COMMAND_OK : PT_COMMAND_UNSUPPORTED;
  if (!count || !pcm_driver.active)
    return PT_COMMAND_OK;
  if (!pcm_driver.current_length || pcm_driver.current_length > 65536u || !pcm_driver.block_size ||
      pcm_driver.current_position >= pcm_driver.current_length || pcm_driver.block_position >= pcm_driver.block_size ||
      pcm_driver.current_frequency != 11025 ||
      pcm_driver.block_size - pcm_driver.block_position > pcm_driver.current_length - pcm_driver.current_position)
    return PT_COMMAND_UNSUPPORTED;
  if (!pcm_driver.current_samples.bytes)
    return PT_COMMAND_INVALID_ARGUMENT;
  const PTPCMResident *image = pt_sound_resources.driver ? pt_sound_resources.resident : NULL;
  if (image && image->playback.auto_dma == 1 &&
      (pcm_driver.queued || pcm_driver.current_length < 2 || (pcm_driver.current_length & 1u) ||
          pcm_driver.block_size != pcm_driver.current_length / 2u))
    return PT_COMMAND_UNSUPPORTED;
  if (pcm_driver.queued) {
    if (!pcm_driver.queued_length || pcm_driver.queued_length > 65536u || pcm_driver.queued_frequency != 11025)
      return PT_COMMAND_UNSUPPORTED;
    if (!pcm_driver.queued_samples.bytes)
      return PT_COMMAND_INVALID_ARGUMENT;
  }
  return PT_COMMAND_OK;
}
u32 pt_pcm_driver_samples(u32 count) {
  pcm_output_size = count;
  if (!pcm_driver.installed) {
    memset(pcm_output, 128, count);
    audio_mix(count);
    return count;
  }
  for (u32 i = 0; i < count; ++i) {
    u8 sample = 128;
    if (pcm_driver.active) {
      sample = pcm_driver.current_samples.bytes[pcm_driver.current_position];
      ++pcm_driver.current_position;
      ++pcm_driver.block_position;
      if (pcm_driver.block_position == pcm_driver.block_size) {
        PTPCMPlayback *resident = pt_sound_resources.driver ? pt_pcm_playback(pt_sound_resources.resident) : NULL;

        if (resident && resident->auto_dma == 1) {
          resident->active_half ^= 1;
          pcm_driver.block_position = 0;
          if (pcm_driver.current_position == pcm_driver.current_length) {
            pcm_driver.current_position = 0;
          }
        } else if (pcm_driver.current_position < pcm_driver.current_length) {
          u32 left = pcm_driver.current_length - pcm_driver.current_position;
          pcm_driver.block_size = left;
          pcm_driver.block_position = 0;
        } else {

          pcm_driver.active = 0;
          if (resident)
            resident->playing = 0;
          if (pcm_driver.queued) {
            pcm_driver.queued = 0;
            if (resident) {
              resident->queued = 0;
              resident->callback_enabled = 0;
              pcm_resident_begin(pt_sound_resources.resident);
            }
            pcm_start(pcm_driver.queued_samples, pcm_driver.queued_length, pcm_driver.queued_frequency);
          } else if (resident && resident->callback_enabled)
            resident->callback_enabled = 0;
        }
      }
    }
    pcm_output[i] = sample;
  }
  audio_mix(count);
  return count;
}
u32 QueryPCMDriverStatus(void) {
  u32 result = pcm_service(false);

  return result;
}
u32 QueryPCMDriverPosition(void) {
  PTPCMResident *image = pcm_live_image();

  const PTPCMPlayback *p = pt_pcm_playback(image);
  u32 result = p->active_half ? (u16)(pcm_driver.current_length + 2u) >> 1 : 1;

  return result;
}
u32 SetPCMDriverAutoDMA(u32 enabled) {
  PTPCMResident *image = pcm_live_image();

  PTPCMPlayback *p = pt_pcm_playback(image);

  p->playing = 0;
  pcm_driver.active = 0;
  p->auto_dma = (u16)enabled;

  return 1;
}
u32 SetPCMDriverSampleFormat(u32 format) {
  PTPCMResident *image = pcm_live_image();

  u32 result = (u16)format <= 3;
  if (result)
    pt_pcm_playback(image)->sample_format = (u16)format;

  return result;
}
u32 CheckPCMDMABuffer(const void *buffer, u32 length) {

  u32 result = buffer != NULL || length == 0;

  return result;
}
u32 PCMDriverPlayBuffer(PTSoundDMATransfer *record) {
  PTPCMResident *image = pcm_live_image();

  require_pcm_transfer(record);
  u32 length = record->length;

  pcm_resident_begin(image);
  pcm_start(pt_pcm_transfer_samples(record), length, record->frequency);

  return 0;
}
void PCMDriverCommit(PTSoundDMATransfer *record) {
  (void)record;

  (void)pcm_service(true);

  return;
}
u32 PCMDriverQueueBuffer(PTSoundDMATransfer *record) {
  PTPCMResident *image = pt_sound_resources.driver ? pt_sound_resources.resident : NULL;
  PTPCMPlayback *resident = NULL;
  if (image) {
    (void)pcm_live_image();

    resident = pt_pcm_playback(image);
  }
  u32 result;
  if (pcm_driver.active && pcm_driver.queued)
    result = 2;
  else {
    require_pcm_transfer(record);
    if (!pcm_driver.active) {
      if (resident)
        pcm_resident_begin(image);
      pcm_start(pt_pcm_transfer_samples(record), record->length, record->frequency);
      result = 0;
    } else {
      pcm_queue_transfer(record);
      pcm_driver.queued = 1;
      result = 1;
      if (resident) {
        resident->queued = 1;
        resident->queued_record = *record;
        resident->callback_enabled = 1;
      }
    }
  }

  return result;
}
u32 SubmitPCMBuffer(PTSoundDMATransfer *record) {
  u32 result = 0;
  if (!sound_pcm_auto_dma)
    result = PCMDriverQueueBuffer(record);

  return result;
}
u32 GetPCMOutputStatus(void) {
  u32 status;
  u32 result;
  if (sound_pcm_auto_dma) {
    status = 2;
    u32 position;
    position = QueryPCMDriverPosition();
    position &= 65535u;

    if (sound_pcm_dma_half && (i32)position < pt_signed_dword(sound_pcm_dma_half)) {
      status = 1;
      sound_pcm_dma_half = 0;
    } else if (!sound_pcm_dma_half && position >= 1024) {
      status = 1;
      sound_pcm_dma_half = 1024;
    }
  } else {
    result = QueryPCMDriverStatus();
    status = (u32)(i32)(i16)((u16)result);
  }
  result = status;

  return result;
}
u32 SoundFillBytes(void *target, u32 size, u32 value) {
  if (size) {
    memset(target, (u8)value, size);
  }

  return value;
}
void SoundCopyBytes(void *target, const void *source, u32 size) {
  if (size) {
    u8 *target_bytes = target;
    const u8 *source_bytes = source;
    for (u32 i = 0; i < size; ++i)
      target_bytes[i] = source_bytes[i];
  }
}

// The original callback refills one 1024-byte half, submits its DMA record,
// and toggles the half pointer. BUGFIX: append repeated data after the tail,
// matching the labelled original repair in tools/repair_pcm_repeat.py.

PTActionResult SoundPCMCallback(void) {
  u32 tail_bytes;
  PTActionResult result = pt_action_value(0);
  if (sound_digital_configuration == 3)
    goto done;
  result = pt_action_value(GetPCMOutputStatus());
  if (result.value == 2)
    goto done;
  if (!pt_sound_resources.output || !pt_sound_resources.samples)
    pt_resource_bounds_failure("PCM output storage", 0, 0);
  if (pt_sound_resources.cursor) {
    if (sound_pcm_remaining >= 1024) {
      SoundCopyBytes(pt_sound_resources.output, pt_sound_resources.cursor, 1024);
      sound_pcm_remaining -= 1024;
      pt_sound_resources.cursor = (pt_sound_resources.cursor + 1024);
    } else {
      SoundCopyBytes(pt_sound_resources.output, pt_sound_resources.cursor, sound_pcm_remaining);
      if (sound_pcm_repeats && pt_sound_resources.repeat_start) {
        if (!pt_sound_resources.repeat_end ||
            (uintptr_t)pt_sound_resources.repeat_end < (uintptr_t)pt_sound_resources.repeat_start)
          pt_resource_bounds_failure("PCM repeat span", 0, 0);
        tail_bytes = sound_pcm_remaining;
        sound_pcm_remaining =
            (u32)((uintptr_t)pt_sound_resources.repeat_end - (uintptr_t)pt_sound_resources.repeat_start);
        pt_sound_resources.cursor = pt_sound_resources.repeat_start;
        SoundCopyBytes(pt_sound_resources.output + tail_bytes, pt_sound_resources.cursor, 1024u - tail_bytes);
        sound_pcm_remaining -= 1024u - tail_bytes;
        pt_sound_resources.cursor = (pt_sound_resources.cursor + 1024u - tail_bytes);
        --sound_pcm_repeats;
      } else {
        (void)SoundFillBytes(
            pt_sound_resources.output + sound_pcm_remaining, (u32)(i32)(i16)((u16)(1024u - sound_pcm_remaining)), 128);
        sound_pcm_remaining = 0;
        pt_sound_resources.cursor = NULL;
      }
    }
  } else
    (void)SoundFillBytes(pt_sound_resources.output, 1024, 128);
  pt_current_sound_dma_transfer()->samples = pt_pcm_resource(pt_sound_resources.output);

  (void)PCMDriverCommit(pt_current_sound_dma_transfer());
  (void)SubmitPCMBuffer(pt_current_sound_dma_transfer());
  pt_sound_resources.output = pt_sound_resources.output == pt_sound_resources.samples
      ? (pt_sound_resources.samples + 1024u)
      : pt_sound_resources.samples;
  result = pt_action_object(pt_sound_resources.output);
done:;

  return result;
}
static void sound_timer_chain(void) {
  PTFMResident *fm = pt_fm_timer_device();
  if (fm)
    (void)fm_timer_tick(fm);
  else
    ++pt_platform_clock()->ticks;
}
static void sound_timer_bios_interrupt(void *context) {
  (void)context;
  sound_timer_chain();
}
static void sound_timer_callback_interrupt(void *context) {
  (void)context;
  ++sound_timer_active;
  pt_runtime_invoke(pt_runtime.sound_timer);
  sound_timer_chain();
  sound_timer_active = 0;
}
PTRuntimeCallback pt_sound_timer_bios_handler(void) {
  return (PTRuntimeCallback){sound_timer_bios_interrupt, NULL};
}
PTRuntimeCallback pt_sound_timer_callback_handler(void) {
  return (PTRuntimeCallback){sound_timer_callback_interrupt, NULL};
}
bool pt_sound_timer_installed(void) {
  return sound_timer.handler.invoke == sound_timer_callback_interrupt && !sound_timer.handler.context;
}
bool pt_sound_timer_uses_bios(void) {
  return sound_timer.handler.invoke == sound_timer_bios_interrupt && !sound_timer.handler.context;
}
PTCommandStatus pt_sound_timer_ready(void) {
  if (!sound_timer.handler.invoke)
    return PT_COMMAND_UNSUPPORTED;
  if (!pt_sound_timer_uses_bios() && !pt_sound_timer_installed())
    return PT_COMMAND_OK;
  PTFMResident *fm = pt_fm_timer_device();
  if (fm) {
    PTFMResident *driver = fm;
    if (!driver->restore_period)
      return PT_COMMAND_UNSUPPORTED;
    if (driver->playing == 1 &&
        (!driver->voice_count.word || driver->voice_count.word > 11 || driver->channel.word >= 16 ||
            driver->event_kind.word >= 8))
      return PT_COMMAND_UNSUPPORTED;
    if (driver->playing == 1 && (!driver->status.base || !driver->stream.base))
      return PT_COMMAND_INVALID_ARGUMENT;
  }
  PTRuntimeCallback callback = pt_runtime.sound_timer;
  if (pt_sound_timer_installed() && callback.invoke == pcm_timer_callback && !callback.context &&
      sound_digital_configuration != 3) {
    const PTPCMResident *image = pt_sound_resources.driver ? pt_sound_resources.resident : NULL;
    if (!image || !pcm_driver.installed)
      return PT_COMMAND_UNSUPPORTED;
    if (!pt_sound_resources.samples || !pt_sound_resources.output || !pt_sound_resources.dma_record)
      return PT_COMMAND_INVALID_ARGUMENT;
    if (pt_sound_resources.output != pt_sound_resources.samples &&
        pt_sound_resources.output != pt_sound_resources.samples + 1024u)
      return PT_COMMAND_UNSUPPORTED;
    if (pt_sound_resources.cursor && sound_pcm_remaining < 1024 && sound_pcm_repeats &&
        pt_sound_resources.repeat_start) {
      uintptr_t begin = (uintptr_t)pt_sound_resources.repeat_start, end = (uintptr_t)pt_sound_resources.repeat_end;
      if (end < begin || end - begin < 1024u - sound_pcm_remaining)
        return PT_COMMAND_UNSUPPORTED;
    }
  }
  return PT_COMMAND_OK;
}
u32 pt_sound_timer_tick(void) {
  pt_runtime_invoke(sound_timer.handler);

  sound_timer.counter = sound_timer.reload;
  return pt_platform_clock()->ticks;
}
u32 InitializeDigitalSound(void) {
  u32 result = 0;
  GuestFileStream *stream;
  stream = GuestFopen(sound_config_filename, sound_config_read_mode);

  if (stream) {
    GuestFread(&pt_native_globals.sound_configuration, 1, sizeof(pt_native_globals.sound_configuration), stream);

    (void)GuestFclose(stream);
  } else {
    sound_digital_configuration = 3;
    sound_config_fallback_word = 4;
  }
  pt_sound_resources.dma_record = pt_sound_dma_transfer_create();
  pt_sound_resources.samples = AllocateSoundDOSBlock(3072);
  result = LoadPCMDriver(sound_pcm_driver_filename);
  if (!result)
    goto failed;
  result = InitializePCMDriver();
  if (!result)
    goto failed;
  if (!pt_sound_resources.samples)
    pt_resource_bounds_failure("PCM sample storage", 0, 0);
  for (;;) {
    result = CheckPCMDMABuffer(pt_sound_resources.samples, 2048);
    if ((u16)result)
      break;
    pt_sound_resources.samples = (pt_sound_resources.samples + 10);
  }
  sound_pcm_auto_dma = 0;
  (void)SetPCMDriverAutoDMA(0);
  (void)SetPCMDriverSampleFormat(0);
  pt_sound_resources.output = pt_sound_resources.samples;
  result = QueryPCMDriverCapabilities();
  if (result & 0x200u) {
    sound_pcm_auto_dma = 1;
    sound_pcm_dma_half = 1024;
    (void)SoundFillBytes(pt_sound_resources.samples, 2048, 128);
    pt_current_sound_dma_transfer()->samples = pt_pcm_resource(pt_sound_resources.samples);

    pt_current_sound_dma_transfer()->length = 2048;
    pt_current_sound_dma_transfer()->frequency = 11025;
    (void)SetPCMDriverAutoDMA(1);
    (void)PCMDriverPlayBuffer(pt_current_sound_dma_transfer());
  }
  (void)InstallSoundTimer(pt_sound_pcm_timer_callback(), 6214);
  sound_digital_initialized = 1;
  result = 1;
  goto done;
failed:
  result = 0;
done:;

  return result;
}
u32 ShutdownDigitalSound(void) {
  RestoreSoundTimer();
  sound_digital_initialized = 0;
  (void)SetPCMDriverAutoDMA(0);
  (void)UnloadPCMDriver();
  u32 result = 1;

  return result;
}
void SetDigitalSoundTimerPeriod(u32 period) {
  if (sound_digital_initialized) {
    RestoreSoundTimer();
    (void)InstallSoundTimer(pt_sound_pcm_timer_callback(), period);
  }
}

// Original FMDRV CS:0A78. Busy waits affect hardware timing only; the
// complete address/data OUT sequence remains in the independent port hash.

static void FMWriteOPL(u8 reg, u8 value) {
  fm_output.enabled = true;
  fm_output.registers[reg] = value;
  pt_opl_write(&fm_synth, reg, value);
}
// Original CS:0AF2 resets the MIDI assignments and emits its exact default
// register sequence, including the final operator-map-plus-voice register.

static void FMResetPrograms(PTFMResident *driver) {
  memset(driver->programs, 0, sizeof(driver->programs));
  memset(driver->voices.channels, 255, sizeof(driver->voices.channels));
  for (unsigned voice = 0; voice < 9; ++voice) {
    FMWriteOPL(0xbd, 0xc0);
    FMWriteOPL(8, 0);
    u8 reg = pt_fm_operator_map[voice];
    const u8 *instrument = pt_fm_default_instrument;
    for (unsigned pair = 0; pair < 4; ++pair) {
      reg = (u8)(reg + 0x20u);
      FMWriteOPL(reg, *instrument++);
      reg = (u8)(reg + 3u);
      FMWriteOPL(reg, *instrument++);
      reg = (u8)(reg - 3u);
    }
    reg = (u8)(reg + 0x60u);
    FMWriteOPL(reg, *instrument++);
    reg = (u8)(reg + 3u);
    FMWriteOPL(reg, *instrument++);
    reg = (u8)(pt_fm_operator_map[voice] + voice);
    FMWriteOPL(reg, *instrument);
  }
}
static void FMResetRhythmMode(PTFMResident *driver) {
  driver->voice_count.word = 9;
  driver->rhythm.word = 0xc0;
  FMWriteOPL(0xbd, 0xc0);
}
// CS:0A67 drives the same retained PIT adapter used by digital sound.

static void FMProgramPIT(PTFMResident *driver, u16 period) {
  driver->current_period = period;
  sound_timer.reload = sound_timer.counter = period;
}
static void FMSilenceVoices(PTFMResident *driver) {
  unsigned voice = driver->voice_count.word;
  if (!voice || voice > PT_FM_VOICES)
    pt_resource_bounds_failure("FM voice count", voice, PT_FM_VOICES);
  do {
    --voice;
    FMWriteOPL((u8)(0x80u + voice), 0x13);
    if (driver->voices.channels[voice] <= 127) {
      u16 frequency = driver->voices.frequencies[voice];
      FMWriteOPL((u8)(0xa0u + voice), (u8)frequency);
      FMWriteOPL((u8)(0xb0u + voice), (u8)(frequency >> 8));
    }
  } while (voice);
  driver->rhythm.low = (u32)driver->rhythm.low & (u32)0xe0;
  FMWriteOPL(0xbd, driver->rhythm.low);
  for (unsigned reg = 0x90; reg <= 0x95; ++reg)
    FMWriteOPL((u8)reg, 255);
}
static void FMSetStatus(PTFMResident *driver, u8 value) {
  u8 *status = pt_fm_status_byte(driver);
  *status = value;
}
static void FMStopMusicInternal(PTFMResident *driver) {
  if (!driver->playing)
    return;
  driver->playing = 0;
  FMProgramPIT(driver, driver->restore_period);
  FMSilenceVoices(driver);
  FMSetStatus(driver, 0);
}
static void FMResetDriver(PTFMResident *driver) {
  driver->flags = 0;
  driver->volume_pair = 0xffff;
  FMStopMusicInternal(driver);
  FMResetRhythmMode(driver);
  for (unsigned i = 0; i < PT_FM_CHANNEL_PAIRS; ++i)
    driver->channel_pairs[i] = 0x101;
  driver->instrument_count = 16;
  if (!driver->default_instruments.base)
    driver->default_instruments = (PTFMBuffer){pt_fm_default_instruments, 0, sizeof(pt_fm_default_instruments)};
  driver->instruments = driver->default_instruments;
  FMResetPrograms(driver);
  driver->period = 0x48d3;
  driver->control = 0;
}
// Original CS:0C5E reads a wrapping 32-bit variable-length delta and
// preserves 16-bit SI wrap within the stream segment. Its first return
// patches both the playback immediate and this helper's own return path.

u32 FMReadDelta(PTFMResident *driver) {
  PTFMBuffer stream = driver->stream;
  u16 offset = stream.offset;
  u32 delta = 0;
  for (unsigned bytes = 0; bytes < 65536; ++bytes) {
    u8 value = *pt_fm_stream_bytes(stream, offset, 1);
    offset = (u16)(offset + 1u);
    delta += (u32)(value & 127u);
    if (value & 128u) {
      delta <<= 7;
      continue;
    }
    driver->stream.offset = offset;
    if (!fm_delta_initialized) {
      fm_delta_initialized = true;
      fm_start_automatically = true;
    }
    return delta;
  }

  return 0;
}
u16 FMStartMusic(PTFMResident *driver, PTFMBuffer stream) {
  if (driver->playing)
    return 0xfffe;
  driver->stream_start = stream;
  driver->stream = stream;
  memset(driver->pitch_offsets, 0, sizeof(driver->pitch_offsets));
  memset(driver->voices.channels, 255, 9);
  u32 delta = FMReadDelta(driver);
  driver->delta.low = (u16)delta;
  driver->delta.high = (u16)(delta >> 16);
  driver->position = driver->stream;
  driver->event_count = 0;
  FMProgramPIT(driver, driver->period);
  driver->bios_remainder = 0;
  FMResetRhythmMode(driver);
  driver->playing = fm_start_automatically;
  FMSetStatus(driver, 255);
  return 0;
}
static u16 FMPauseMusic(PTFMResident *driver) {
  if (driver->playing != 1)
    return 0xfffd;
  driver->playing = 2;
  FMSilenceVoices(driver);
  return 0;
}
static u16 FMResumeMusic(PTFMResident *driver) {
  if (driver->playing != 2)
    return 0xfffc;
  driver->playing = 1;
  return 0;
}
static i32 fm_signed_byte(u8 value) {
  return value < 128 ? value : (i32)value - 256;
}
static u16 fm_stream_word(PTFMBuffer window, u16 cursor, unsigned bytes) {
  const u8 *data = pt_fm_stream_bytes(window, cursor, bytes);
  return bytes == 1 ? data[0] : (u16)(data[0] | (u16)data[1] << 8);
}
// CS:0B66, original instrument bytes and channel/operator mapping.

static void FMApplyInstrument(PTFMResident *driver, unsigned voice, u8 program) {
  if (program >= driver->instrument_count)
    return;
  if (voice >= PT_FM_VOICES)
    pt_resource_bounds_failure("FM voice", voice, PT_FM_VOICES);
  PTFMBuffer instruments = driver->instruments;
  u16 offset = (u16)(instruments.offset + (u16)(fm_signed_byte(program) * 16));
  PTFMInstrumentTone data;
  bool defaults = instruments.base == driver->default_instruments.base;
  u16 relative = (u16)(offset - driver->default_instruments.offset);
  if (defaults && (u32)relative + sizeof(data) > sizeof(pt_fm_default_instruments))
    pt_resource_bounds_failure(
        "default FM instrument", fm_signed_byte(program), sizeof(pt_fm_default_instruments) / 16u);
  u8 *bytes = (u8 *)&data;
  for (unsigned i = 0; i < sizeof(data); ++i)
    bytes[i] =
        defaults ? pt_fm_default_instruments[relative + i] : (u8)fm_stream_word(instruments, (u16)(offset + i), 1);
  bool percussion = driver->rhythm.high != 0;
  u8 level = (percussion && voice >= 7 ? data.modulator_level : data.carrier_level);
  driver->voices.levels[voice] = level & 0xc0;
  driver->voices.original_volumes[voice] = (driver->voices.volumes[voice] = (u8)(63u - (level & 63u)));
  if (percussion && voice > 6) {
    u8 reg = pt_fm_percussion_operators[voice];
    FMWriteOPL((u8)(reg + 0x20), data.modulator_characteristics);
    FMWriteOPL((u8)(reg + 0x60), data.modulator_attack_decay);
    FMWriteOPL((u8)(reg + 0x80), data.modulator_sustain_release);
    FMWriteOPL((u8)(reg + 0xe0), data.modulator_waveform);
    FMWriteOPL((u8)(pt_fm_percussion_channels[voice] + 0xc0), data.feedback_connection);
  } else {
    u8 reg = pt_fm_operator_map[voice];
    FMWriteOPL((u8)(reg + 0x20), data.modulator_characteristics);
    FMWriteOPL((u8)(reg + 0x23), data.carrier_characteristics);
    FMWriteOPL((u8)(reg + 0x40), data.modulator_level);
    FMWriteOPL((u8)(reg + 0x60), data.modulator_attack_decay);
    FMWriteOPL((u8)(reg + 0x63), data.carrier_attack_decay);
    FMWriteOPL((u8)(reg + 0x80), data.modulator_sustain_release);
    FMWriteOPL((u8)(reg + 0x83), data.carrier_sustain_release);
    FMWriteOPL((u8)(reg + 0xe0), data.modulator_waveform);
    FMWriteOPL((u8)(reg + 0xe3), data.carrier_waveform);
    FMWriteOPL((u8)(voice + 0xc0), data.feedback_connection);
  }
}
// CS:0CBE prefers released voices of this channel, then unused/released
// voices, then the greatest wrapping event age.

static unsigned FMAllocateVoice(PTFMResident *driver, u8 channel) {
  const PTFMVoices *voices = &driver->voices;
  unsigned count = driver->voice_count.word;
  if (count > PT_FM_VOICES || !count)
    pt_resource_bounds_failure("FM voice count", count, PT_FM_VOICES);
  for (unsigned i = 0; i < count; ++i)
    if (voices->channels[i] == (u8)(channel | 128u))
      return i;
  for (unsigned i = 0; i < count; ++i)
    if (voices->channels[i] == 255)
      return i;
  for (unsigned i = 0; i < count; ++i)
    if (voices->channels[i] > 127)
      return i;
  unsigned selected = 0;
  u16 maximum = 0, event = driver->event_count;
  for (unsigned i = 0; i < count; ++i) {
    u16 age = (u16)(event - voices->ages[i]);
    if (age > maximum) {
      maximum = age;
      selected = i;
    }
  }
  u16 frequency = voices->frequencies[selected];
  FMWriteOPL((u8)(0xa0u + selected), (u8)frequency);
  FMWriteOPL((u8)(0xb0u + selected), (u8)(frequency >> 8));
  return selected;
}
// CS:1072 applies the channel pitch offset to the original note encoding.

static u16 FMComputeFrequency(PTFMResident *driver, unsigned voice, u8 encoded) {
  if (voice >= PT_FM_VOICES)
    pt_resource_bounds_failure("FM voice", voice, PT_FM_VOICES);
  if (driver->channel.word >= PT_FM_CHANNELS)
    pt_resource_bounds_failure("FM channel", driver->channel.word, PT_FM_CHANNELS);
  u8 block = (encoded & 0x70u) >> 2;
  u16 pitch = (u16)(((encoded & 15u) << 6) + driver->pitch_offsets[driver->channel.word]);
  if (pitch & 0x8000u) {
    pitch = (u16)(pitch + 0x300u);
    block = (u8)(block - 4u);
    if (block & 128u) {
      block = 0;
      pitch = 0;
    }
  } else if (pitch >= 0x300) {
    pitch = (u16)(pitch - 0x300u);
    block = (u8)(block + 4u);
    if (block > 0x1c) {
      pitch = 0x2ff;
      block = 0x1c;
    }
  }

  u16 frequency = pt_fm_frequency_table[pitch] | (u16)((u16)block << 8);
  driver->voices.frequencies[voice] = frequency;
  driver->voices.ages[voice] = driver->event_count;
  return frequency;
}
static u16 FMComputeNote(PTFMResident *driver, unsigned voice) {
  if (voice >= PT_FM_VOICES)
    pt_resource_bounds_failure("FM voice", voice, PT_FM_VOICES);
  u8 note = driver->event_note;
  driver->voices.keys[voice] = note;
  u16 adjusted = (u16)(driver->control + fm_signed_byte(note));
  if (adjusted & 0x8000u)
    adjusted = 0;
  else if (adjusted >= 128)
    adjusted = 127;
  u8 encoded = pt_fm_note_table[adjusted];
  driver->voices.notes[voice] = encoded;
  return FMComputeFrequency(driver, voice, encoded);
}
static u8 FMNoteLevel(PTFMResident *driver, unsigned voice, bool percussion) {
  u8 velocity = driver->event_velocity;
  unsigned index = (velocity >> 3) + ((velocity >> 2) & 1u);
  u8 volume = driver->voices.volumes[voice];
  u16 level = (u16)((percussion ? volume : fm_signed_byte(volume)) + pt_fm_velocity_table[index]);
  if (level & 0x8000u)
    level = 0;
  else if (level > 63)
    level = 63;
  return (u8)(63u - level) | driver->voices.levels[voice];
}
static void FMNoteOff(PTFMResident *driver) {
  u8 channel = driver->channel.low;
  if (channel >= PT_FM_CHANNELS)
    pt_resource_bounds_failure("FM channel", channel, PT_FM_CHANNELS);
  unsigned count = driver->voice_count.word;
  if (count > PT_FM_VOICES)
    pt_resource_bounds_failure("FM voice count", count, PT_FM_VOICES);
  if (count <= 6 && channel >= 11) {
    driver->rhythm.low = (u32)driver->rhythm.low & (u32)((u8)~pt_fm_percussion_operators[channel]);
    FMWriteOPL(0xbd, driver->rhythm.low);
    return;
  }
  for (unsigned voice = 0; voice < count; ++voice) {
    if (driver->voices.keys[voice] != driver->event_note || driver->voices.channels[voice] != channel)
      continue;
    driver->voices.channels[voice] = (u32)(driver->voices.channels[voice]) | (u32)128;
    u16 frequency = driver->voices.frequencies[voice];
    FMWriteOPL((u8)(0xa0u + voice), (u8)frequency);
    FMWriteOPL((u8)(0xb0u + voice), (u8)(frequency >> 8));
    break;
  }
}
static void FMNoteOn(PTFMResident *driver) {
  if (!driver->event_velocity) {
    FMNoteOff(driver);
    return;
  }
  u8 channel = driver->channel.low;
  if (channel >= PT_FM_CHANNELS)
    pt_resource_bounds_failure("FM channel", channel, PT_FM_CHANNELS);
  if (!driver->enabled_channels[channel])
    return;
  if (driver->rhythm.high && channel >= 11) {
    unsigned voice = channel - 5u;
    driver->rhythm.low = (u32)driver->rhythm.low | (u32)(pt_fm_percussion_bits[voice]);
    u8 reg = pt_fm_percussion_operators[voice];
    if (voice == 6)
      reg = (u8)(reg + 3u);
    FMWriteOPL((u8)(reg + 0x40u), FMNoteLevel(driver, voice, true));
    u16 frequency = FMComputeNote(driver, voice);
    unsigned output = pt_fm_percussion_channels[voice];
    FMWriteOPL((u8)(0xa0u + output), (u8)frequency);
    FMWriteOPL((u8)(0xb0u + output), (u8)(frequency >> 8));
    FMWriteOPL(0xbd, driver->rhythm.low);
    return;
  }
  unsigned voice = FMAllocateVoice(driver, channel);
  if (voice >= PT_FM_VOICES)
    pt_resource_bounds_failure("FM voice", voice, PT_FM_VOICES);
  u8 previous = driver->voices.channels[voice];
  driver->voices.channels[voice] = channel;
  if ((previous & 127u) != channel)
    FMApplyInstrument(driver, voice, driver->programs[channel]);
  FMWriteOPL((u8)(pt_fm_operator_map[voice] + 0x43u), FMNoteLevel(driver, voice, false));
  u16 frequency = FMComputeNote(driver, voice);
  FMWriteOPL((u8)(0xa0u + voice), (u8)frequency);
  FMWriteOPL((u8)(0xb0u + voice), (u8)((frequency >> 8) | 0x20u));
}
static void FMChangeProgram(PTFMResident *driver, u8 program) {
  u8 count = driver->instrument_count;

  while (program >= count)
    program = (u8)(program - count);
  u8 channel = driver->channel.low;
  if (channel >= PT_FM_CHANNELS)
    pt_resource_bounds_failure("FM channel", channel, PT_FM_CHANNELS);
  driver->programs[channel] = program;
  if (driver->rhythm.high && channel >= 11) {
    FMApplyInstrument(driver, channel - 5u, program);
    return;
  }
  unsigned voices = driver->voice_count.word;
  if (voices > PT_FM_VOICES)
    pt_resource_bounds_failure("FM voice count", voices, PT_FM_VOICES);
  for (unsigned i = 0; i < voices; ++i)
    if (driver->voices.channels[i] == (u8)(channel | 128u))
      driver->voices.channels[i] = 255;
  for (unsigned i = 0; i < voices; ++i)
    if (driver->voices.channels[i] == channel)
      FMApplyInstrument(driver, i, program);
}
static void FMSetRhythm(PTFMResident *driver, u8 value) {
  FMResetPrograms(driver);
  driver->rhythm.high = value;
  driver->rhythm.low = value ? 0xe0 : 0xc0;
  driver->voice_count.low = value ? 6 : 9;
  FMWriteOPL(0xbd, driver->rhythm.low);
}
static void FMSetPitch(PTFMResident *driver, u8 value) {
  i32 signed_value = fm_signed_byte(value), pitch = signed_value >= 0 ? signed_value / 4 : -((-signed_value + 3) / 4);
  u8 channel = driver->channel.low;
  if (channel >= PT_FM_CHANNELS)
    pt_resource_bounds_failure("FM channel", channel, PT_FM_CHANNELS);
  driver->pitch_offsets[channel] = (u16)pitch;
  if (driver->voice_count.word > PT_FM_VOICES)
    pt_resource_bounds_failure("FM voice count", driver->voice_count.word, PT_FM_VOICES);
  for (unsigned voice = 0; voice < driver->voice_count.word; ++voice) {
    if (driver->voices.channels[voice] != channel)
      continue;
    u16 frequency = FMComputeFrequency(driver, voice, driver->voices.notes[voice]);
    FMWriteOPL((u8)(0xa0u + voice), (u8)frequency);
    FMWriteOPL((u8)(0xb0u + voice), (u8)((frequency >> 8) | 0x20u));
  }
}
static void FMNegatePitch(PTFMResident *driver, u8 value) {
  FMSetPitch(driver, (u8)(0u - value));
}
static void FMControlChange(PTFMResident *driver, u8 controller, u8 value) {
  switch (controller) {
  case 0x66:
    FMSetStatus(driver, value);
    break;
  case 0x67:
    FMSetRhythm(driver, value);
    break;
  case 0x68:
    FMSetPitch(driver, value);
    break;
  case 0x69:
    FMNegatePitch(driver, value);
    break;
  default:
    break;
  }
}
static u16 FMAdvanceStream(PTFMResident *driver, u32 amount) {
  PTFMBuffer stream = driver->stream;
  u32 offset = (u32)stream.offset + (amount & 65535u);
  u32 pages = (amount >> 16) + (offset > 65535u);
  stream = pt_fm_buffer_rebase(stream, pages << 16);
  stream.offset = (u16)offset;
  driver->stream = stream;
  return stream.offset;
}
static u16 FMSysExEvent(PTFMResident *driver, u16 cursor) {
  driver->stream.offset = cursor;

  return FMAdvanceStream(driver, FMReadDelta(driver));
}
static u16 FMSkipSystemEvent(PTFMResident *driver, u16 cursor) {
  driver->stream.offset = cursor;
  return FMAdvanceStream(driver, FMReadDelta(driver));
}
static void FMEndSong(PTFMResident *driver) {
  FMStopMusicInternal(driver);
}
static u16 FMMetaEvent(PTFMResident *driver, PTFMBuffer window, u16 cursor) {
  u8 type = (u8)fm_stream_word(window, cursor, 1);
  ++cursor;
  if (type == 0x2f)
    FMStopMusicInternal(driver);
  driver->stream.offset = cursor;
  return FMAdvanceStream(driver, FMReadDelta(driver));
}
static u16 FMSystemEvent(PTFMResident *driver, PTFMBuffer window, u16 cursor, u8 subtype) {
  switch (subtype) {
  case 0:
    return FMSysExEvent(driver, cursor);
  case 7:
    return FMSkipSystemEvent(driver, cursor);
  case 15:
    return FMMetaEvent(driver, window, cursor);
  case 12:
    FMEndSong(driver);
    break;
  case 2:
    cursor = (u16)(cursor + 2u);
    break;
  case 1:
  case 3:
    ++cursor;
    break;
  default:
    break;
  }
  return cursor;
}
// CS:0E24 consumes zero-delta events in one timer action. Running status
// retains the original channel/event words and source segment arithmetic.

void FMProcessEvents(PTFMResident *driver) {
  driver->event_count = (u16)(driver->event_count + 1u);
  PTFMBuffer stream = driver->stream;
  u16 offset = stream.offset;
  stream = pt_fm_buffer_rebase(stream, offset & ~15u);
  stream.offset = offset & 15u;
  driver->stream = stream;
  for (unsigned event = 0; event < 65536; ++event) {
    PTFMBuffer window = driver->stream;
    u16 cursor = window.offset;
    driver->position = window;
    u8 status = (u8)fm_stream_word(window, cursor, 1);
    if (status & 128u) {
      ++cursor;
      driver->channel.low = status & 15u;
      driver->event_kind.low = (status >> 4) - 8u;
    }
    unsigned kind = driver->event_kind.word;
    u8 channel = driver->channel.low;

    if (kind == 0 || kind == 1) {
      u16 note = fm_stream_word(window, cursor, 2);
      cursor = (u16)(cursor + 2u);
      driver->event_note = (u8)note;
      driver->event_velocity = (u8)(note >> 8);
      if (kind == 0)
        FMNoteOff(driver);
      else
        FMNoteOn(driver);
    } else if (kind == 3) {
      u16 control = fm_stream_word(window, cursor, 2);
      cursor = (u16)(cursor + 2u);
      FMControlChange(driver, (u8)control, (u8)(control >> 8));
    } else if (kind == 4) {
      FMChangeProgram(driver, (u8)fm_stream_word(window, cursor, 1));
      ++cursor;
    } else if (kind == 5)
      ++cursor;
    else if (kind == 7)
      cursor = FMSystemEvent(driver, window, cursor, channel);
    else
      cursor = (u16)(cursor + 2u);
    driver->stream.offset = cursor;
    if (!driver->playing) {
      u16 loops = driver->flags;
      if (loops) {
        FMStartMusic(driver, driver->stream_start);
        if (loops != 65535)
          driver->flags = (u16)(loops - 1u);
      }
      return;
    }
    u32 delta = FMReadDelta(driver);
    driver->delta.low = (u16)delta;
    driver->delta.high = (u16)(delta >> 16);
    if (!delta)
      continue;
    --delta;
    driver->delta.low = (u16)delta;
    driver->delta.high = (u16)(delta >> 16);
    return;
  }
}

// CS:1472 clamps a signed WORD; CS:1483 returns the integer quotient
// in AH and the original rounded binary fraction in AL.

static u16 FMClampPercentage(u16 value) {
  if (value & 0x8000u)
    return 0;
  return value >= 100 ? 99 : value;
}
static u16 FMFraction(u16 value) {
  unsigned numerator = value & 255u, denominator = value >> 8;

  u8 remainder = (u8)(numerator % denominator), fraction = 0;
  for (unsigned bit = 0; bit < 8; ++bit) {
    bool carry = (remainder & 128u) != 0;
    remainder = (u8)(remainder * 2u);
    fraction = (u8)(fraction * 2u);
    if (carry || remainder >= denominator) {
      fraction |= 1u;
      remainder = (u8)(remainder - denominator);
    }
  }
  fraction = (u8)(fraction + ((remainder & 128u) != 0));
  return (u16)((numerator / denominator) << 8) | fraction;
}
static void FMScaleVoiceVolumes(PTFMResident *driver, u8 volume) {
  for (unsigned voice = 0; voice < PT_FM_VOICES; ++voice) {
    u16 product = (u16)driver->voices.original_volumes[voice] * volume;
    driver->voices.volumes[voice] = (u8)((product + 128u) >> 8);
  }
}
// CS:13D9 / service 14. AX=-1 retains the current level; DX is the
// destination percentage, CX the step percentage and DI the timer interval.

static u16 FMSetVolumeRamp(PTFMResident *driver, u16 start, u16 target, u16 step, u16 interval) {
  driver->fade.period = interval;
  driver->fade.counter = interval;
  driver->fade.step = (u8)FMFraction((u16)(0x6400u | FMClampPercentage(step)));
  u8 current = start == 65535 ? driver->fade.current : (u8)FMFraction((u16)(0x6400u | FMClampPercentage(start)));
  driver->fade.current = (driver->fade.target = current);
  FMScaleVoiceVolumes(driver, current);
  u16 destination = FMFraction((u16)(0x6400u | FMClampPercentage(target)));
  driver->fade.target = (u8)destination;
  return destination;
}
// Original bug: stock saturation can alternate around an interior target.
// The repaired rule also clamps a step that crosses the target.

static void FMFadeStep(PTFMResident *driver) {
  u8 current = driver->fade.current, target = driver->fade.target, step = driver->fade.step;
  if (current >= target)
    current = current < step || (unsigned)current - step < target ? target : (u8)(current - step);
  else
    current = (unsigned)current + step > target ? target : (u8)(current + step);
  driver->fade.current = current;
  FMScaleVoiceVolumes(driver, current);
}

PTFMResident *pt_fm_timer_device(void) {
  PTFMResident *driver = fm_devices.timer;
  return driver;
}
bool pt_fm_timer_installed(void) {
  return pt_fm_timer_device() != NULL;
}
static u32 fm_timer_tick(PTFMResident *driver) {
  if (driver->playing == 1) {
    if (driver->fade.current != driver->fade.target) {
      u16 remaining = (u16)(driver->fade.counter - 1u);
      driver->fade.counter = remaining;
      if (!remaining) {
        driver->fade.counter = driver->fade.period;
        FMFadeStep(driver);
      }
    }
    u32 delta = driver->delta.value;
    driver->delta.value = delta - 1u;
    if (!delta) {

      FMProcessEvents(driver);
    }
  }
  u16 period = driver->current_period, restore = driver->restore_period;
  driver->counter = (u16)(driver->counter + period);
  u32 remainder = (u32)driver->bios_remainder + period;
  driver->bios_remainder = (u16)remainder;
  if (remainder > 65535u || (u16)remainder >= restore) {
    do {
      remainder = (u16)(remainder - restore);
      driver->bios_remainder = (u16)remainder;
      ++pt_platform_clock()->ticks;
    } while (remainder > restore);
  }
  return pt_platform_clock()->ticks;
}

// Unsupported resident services stop before changing device state.

PTFMReply pt_fm_command(PTFMResident *driver, PTFMCommand command) {
  PTFMReply reply = {.value = command.value};
  if (driver->busy) {
    reply.value = 0xfff8;
    return reply;
  }
  driver->busy = 1;
  switch (command.service) {
  case PT_FM_VERSION:
    reply.value = driver->version;
    break;
  case PT_FM_INITIALIZE:
    reply.previous = driver->status;
    driver->status = command.resource;
    FMSetStatus(driver, 0);
    break;
  case PT_FM_INSTRUMENTS:
    driver->instrument_count = (u8)command.count;
    driver->instruments = command.resource;
    FMResetRhythmMode(driver);
    FMResetPrograms(driver);
    reply.value = 0;
    break;
  case PT_FM_RESTORE_PERIOD:
    driver->restore_period = command.value;
    break;
  case PT_FM_PERIOD:
    driver->period = command.value;
    FMProgramPIT(driver, command.value);
    reply.value = 0;
    break;
  case PT_FM_CONTROL:
    driver->control = command.value;
    reply.value = 0;
    break;
  case PT_FM_PLAY:
    reply.value = FMStartMusic(driver, command.resource);
    break;
  case PT_FM_STOP:
    reply.value = 0xfffd;
    if (driver->playing) {
      FMStopMusicInternal(driver);
      reply.value = 0;
    }
    break;
  case PT_FM_SHUTDOWN:
    FMResetDriver(driver);
    reply.value = 0;
    break;
  case PT_FM_PAUSE:
    reply.value = FMPauseMusic(driver);
    break;
  case PT_FM_RESUME:
    reply.value = FMResumeMusic(driver);
    break;
  case PT_FM_VOLUME:
    reply.value = FMSetVolumeRamp(driver, command.value, command.auxiliary, command.count, command.interval);
    break;
  case PT_FM_FLAGS:
    reply.value = driver->flags;
    driver->flags = command.value;
    break;
  default:
    abort();
  }
  driver->busy = 0;
  return reply;
}
// Game controls call the native resident with actual resource buffers.

u32 QueryFMDriverVersion(void) {
  return pt_fm_command(pt_fm_device(sound_driver_interrupt), (PTFMCommand){.service = PT_FM_VERSION}).value;
}
void InitializeFMDriver(u8 *status) {
  (void)pt_fm_command(pt_fm_device(sound_driver_interrupt),
      (PTFMCommand){.service = PT_FM_INITIALIZE, .resource = pt_fm_resource(status, 1)});
}
void UploadFMInstruments(u32 count, const u8 *instruments) {
  (void)pt_fm_command(pt_fm_device(sound_driver_interrupt),
      (PTFMCommand){.service = PT_FM_INSTRUMENTS,
        .count = (u16)count,
        .resource = pt_fm_resource(instruments, (u32)(u8)count * 16u)});
}
void SetFMMusicTimerPeriod(u32 period) {
  (void)pt_fm_command(
      pt_fm_device(sound_driver_interrupt), (PTFMCommand){.service = PT_FM_PERIOD, .value = (u16)period});
}
u32 PlayFMMusic(const u8 *stream, u32 capacity) {
  return pt_fm_command(pt_fm_device(sound_driver_interrupt),
      (PTFMCommand){.service = PT_FM_PLAY, .resource = pt_fm_resource(stream, capacity)})
      .value;
}
static u32 fm_control_wrapper(PTFMService service) {
  return pt_fm_command(pt_fm_device(sound_driver_interrupt), (PTFMCommand){.service = service}).value;
}
u32 PauseFMMusic(void) {
  return fm_control_wrapper(PT_FM_PAUSE);
}
u32 ResumeFMMusic(void) {
  return fm_control_wrapper(PT_FM_RESUME);
}
u32 StopFMMusic(void) {
  return fm_control_wrapper(PT_FM_STOP);
}
u32 ShutdownFMDriver(void) {
  return fm_control_wrapper(PT_FM_SHUTDOWN);
}
// Find a bound native device or an imported device at the platform boundary.

u32 FindFMDriver(void) {
  sound_driver_interrupt = 0;
  for (u8 interrupt = 0x80; interrupt <= 0xbf; ++interrupt)
    if (pt_fm_device_probe(interrupt)) {
      sound_driver_interrupt = interrupt;
      break;
    }
  return sound_driver_interrupt;
}
void InitSound(void) {
  if (!sound_config_flag)
    return;
  u32 driver;
  driver = FindFMDriver();
  if (!driver) {
    sound_config_flag = 0;
    return;
  }
  if (sound_var_3) {
    (void)InitializeDigitalSound();
  }
  pt_sound_resources.fm_status = pt_sound_buffer_allocate(PT_SOUND_BUFFER_STATUS);
  pt_sound_resources.music = pt_sound_buffer_allocate(PT_SOUND_BUFFER_MUSIC);
  if (!pt_sound_resources.music) {
    sound_config_flag = 0;
    return;
  }
  InitializeFMDriver(pt_sound_resources.fm_status);
}
u32 AllocateSoundBuffer(void) {
  if (!sound_config_flag)
    return 1;
  pt_sound_resources.bank = pt_sound_buffer_allocate(PT_SOUND_BUFFER_BANK);
  return pt_sound_resources.bank ? 1 : 0;
}
// Cancel the native VOC state before the separate software/resident driver shutdown boundary.

void StopSound(void) {
  if (!sound_config_flag)
    return;
  SoundStuff1();
  if (sound_var_3)
    (void)ShutdownDigitalSound();
  (void)ShutdownFMDriver();

  game_ptr->sound_enabled = 0;
  WaitForRetrace();
  pt_sound_buffer_release(pt_sound_resources.bank);
}
// Intro playback owns its buffer until FreeIntroBuffer.

u32 AllocateIntroBuffer(void) {
  if (!sound_config_flag)
    return 1;
  pt_sound_resources.bank = pt_sound_buffer_allocate(PT_SOUND_BUFFER_INTRO);
  return pt_sound_resources.bank ? 1 : 0;
}
void FreeIntroBuffer(void) {
  if (pt_sound_resources.bank) {
    pt_sound_buffer_release(pt_sound_resources.bank);
  }
}
u32 LoadIntroMusic(void) {
  if (!sound_config_flag)
    return 1;
  if (!pt_sound_resources.bank)
    return 0;
  u32 loaded;
  loaded = LoadMusicFileToBuffer(intro_music_filename, pt_sound_resources.bank);
  return (u16)loaded ? 1 : 0;
}
// File views form pointers only after proving their requested extent.

const u8 *pt_sound_bytes_at(PTSoundBytes bytes, u32 offset, u32 size) {
  if (offset > bytes.capacity || size > bytes.capacity - offset || (!bytes.data && (offset || size))) {
    fputs("Pizza Tycoon: sound file range exceeds its native buffer\n", stderr);
    abort();
  }
  if (!offset)
    return bytes.data;
  return bytes.data + offset;
}
static u32 sound_byte_position(PTSoundBytes bytes, const void *cursor) {
  uintptr_t offset = (uintptr_t)cursor - (uintptr_t)bytes.data;
  if (offset > bytes.capacity) {
    fputs("Pizza Tycoon: sound cursor is outside its native buffer\n", stderr);
    abort();
  }
  return (u32)offset;
}
void pt_sound_bytes_require(PTSoundBytes bytes, const void *cursor, u32 size) {
  (void)pt_sound_bytes_at(bytes, sound_byte_position(bytes, cursor), size);
}
const u8 *pt_sound_bytes_advance(PTSoundBytes bytes, const u8 *cursor, u32 size) {
  u32 offset = sound_byte_position(bytes, cursor);
  if (size > bytes.capacity - offset) {
    fputs("Pizza Tycoon: sound cursor advance exceeds its native buffer\n", stderr);
    abort();
  }
  return pt_sound_bytes_at(bytes, offset + size, 0);
}
PTMusicView pt_music_view(PTSoundBytes bytes) {
  const PTMusicHeader *header = (const PTMusicHeader *)pt_sound_bytes_at(bytes, 0, sizeof(*header));
  i32 instruments = header->instruments_offset, stream = header->stream_offset;
  u32 instrument_bytes = (u32)(u8)header->instrument_count * 16u;
  if (instruments < 0 || stream < 0 || (u32)stream > bytes.capacity || !header->rate) {
    fputs("Pizza Tycoon: invalid native music header\n", stderr);
    abort();
  }
  const u8 *bank = pt_sound_bytes_at(bytes, (u32)instruments, instrument_bytes);
  const u8 *events = pt_sound_bytes_at(bytes, (u32)stream, 0);
  u32 period = (u32)(i32)(i16)((u16)(1193180 / header->rate));
  return (PTMusicView){header, {bank, instrument_bytes}, {events, bytes.capacity - (u32)stream}, period};
}

// VOC block lengths are unsigned 24-bit little-endian values.

u32 ReadVOCBlockSize(const PTVOCBlockLength *length) {
  u32 size;
  size = 0;
  for (unsigned i = 0; i < 3; ++i)
    size += (u32)length->bytes[i] << (8u * i);
  u32 result = size;
  return result;
}
u32 StopVOCSound(void) {
  pt_sound_resources.repeat_start = NULL;
  pt_sound_resources.cursor = NULL;
  sound_pcm_remaining = 0;

  return 0;
}
// Retain its fixed 26-byte header skip and tag behavior, including the repeat-end pointer one
// byte before the tag. Unknown tags end the scan. Sample position and loop state belong to
// global guest memory.

PTSoundDMATransfer *PrepareVOCSound(const u8 *data, u32 capacity) {
  const PTSoundBytes bytes = {data, capacity};
  PTSoundDMATransfer *result = NULL;
  const u8 *cursor;
  bool finished;
  if (sound_digital_configuration == 3)
    goto done;
  pt_sound_resources.repeat_end = NULL;
  pt_sound_resources.repeat_start = NULL;
  sound_pcm_repeats = 0;
  pt_sound_resources.cursor = NULL;
  sound_pcm_remaining = 0;
  if (!data)
    pt_resource_bounds_failure("VOC data", 0, 0);
  cursor = pt_sound_bytes_at(bytes, PT_VOC_HEADER_BYTES, 0);
  finished = 0;
  for (; !finished;) {
    const u8 *block = cursor;
    pt_sound_bytes_require(bytes, block, sizeof(PTVOCMarker));

    u8 type = ((const PTVOCMarker *)block)->tag;

    u32 length;
    switch (type) {
    case PT_VOC_END:
      finished = 1;
      break;
    case PT_VOC_SOUND:
      pt_sound_bytes_require(bytes, block, sizeof(PTVOCSoundBlock));
      cursor = pt_sound_bytes_advance(bytes, block, sizeof(PTVOCMarker));
      pt_sound_resources.repeat_start = (u8 *)(pt_sound_bytes_advance(bytes, block, sizeof(PTVOCSoundBlock)));
      pt_sound_resources.cursor = pt_sound_resources.repeat_start;
      length = ReadVOCBlockSize(((const PTVOCBlockLength *)(cursor)));
      sound_pcm_remaining = length - 2u;
      length = ReadVOCBlockSize(((const PTVOCBlockLength *)(cursor)));
      cursor = pt_sound_bytes_advance(bytes, cursor, length + sizeof(PTVOCBlockLength));
      break;
    case PT_VOC_CONTINUATION:
      pt_sound_bytes_require(bytes, block, sizeof(PTVOCBlockHeader));
      pt_sound_resources.repeat_start = (u8 *)(pt_sound_bytes_advance(bytes, block, sizeof(PTVOCBlockHeader)));
      cursor = pt_sound_bytes_advance(bytes, block, sizeof(PTVOCMarker));
      length = ReadVOCBlockSize(((const PTVOCBlockLength *)(cursor)));
      cursor = pt_sound_bytes_advance(bytes, cursor, length + sizeof(PTVOCBlockLength));
      break;
    case PT_VOC_SILENCE:
    case PT_VOC_MARKER:
    case PT_VOC_TEXT:
      pt_sound_bytes_require(bytes, block, sizeof(PTVOCBlockHeader));
      cursor = pt_sound_bytes_advance(bytes, block, sizeof(PTVOCMarker));
      length = ReadVOCBlockSize(((const PTVOCBlockLength *)(cursor)));
      cursor = pt_sound_bytes_advance(bytes, cursor, length + sizeof(PTVOCBlockLength));
      break;
    case PT_VOC_REPEAT_BEGIN:
      pt_sound_bytes_require(bytes, block, sizeof(PTVOCRepeatBlock));
      cursor = pt_sound_bytes_advance(bytes, block, sizeof(PTVOCBlockHeader));

      sound_pcm_repeats = ((const PTVOCRepeatBlock *)(block))->repeats;
      cursor = pt_sound_bytes_advance(bytes, block, sizeof(PTVOCRepeatBlock));
      break;
    case PT_VOC_REPEAT_END:
      pt_sound_resources.repeat_end = (u8 *)(block - 1);
      cursor = pt_sound_bytes_advance(bytes, block, sizeof(PTVOCMarker));
      break;
    default:
      finished = 1;
      break;
    }
  }
  if (pt_sound_resources.cursor)
    pt_sound_bytes_require(bytes, pt_sound_resources.cursor, sound_pcm_remaining);
  result = pt_current_sound_dma_transfer();
  result->frequency = 11025;
done:;

  return result;
}
void SoundStuff1(void) {
  if (!sound_config_flag)
    return;
  if (sound_var_3)
    (void)StopVOCSound();

  if (pt_music_is_active())
    (void)StopFMMusic();
  state_slot_011 = 0;
  competition_sound_id = 255;
}
void SetSoundFlag(u8 value) {
  if (sound_config_flag)
    sound_flag = value;
}
u8 GetSoundFlag(void) {
  return sound_config_flag ? sound_flag : 0;
}

void SoundStuff2(u8 index) {
  if (!sound_config_flag || !prefs.field_2 || !pt_sound_resources.bank)
    goto done;

  if (!game_ptr->sound_enabled || state_slot_010)
    goto done;

  if (index >= pt_loaded_sound_bank()->count)
    goto done;
  const PTSoundBank *bank = pt_loaded_sound_bank();
  const PTSoundBankEntry *entry = &bank->entries[index];

  u32 type = entry->kind, offset = entry->offset, size = entry->size;
  if (type == 1) {
    if (!pt_sound_resources.music || size > pt_sound_buffer_size(PT_SOUND_BUFFER_MUSIC) ||
        offset > PT_SOUND_BANK_CAPACITY || size > PT_SOUND_BANK_CAPACITY - offset)
      pt_resource_bounds_failure("music resource", size, pt_sound_buffer_size(PT_SOUND_BUFFER_MUSIC));
    memmove(pt_sound_resources.music, (pt_sound_resources.bank + offset), size);
  }
  SoundStuff1();
  if (type == 1) {
    Delay(5);

    PTMusicView music =
        pt_music_view((PTSoundBytes){pt_sound_resources.music, pt_sound_buffer_size(PT_SOUND_BUFFER_MUSIC)});
    u32 count = (u32)(i32)music.header->instrument_count;
    const u8 *instruments = music.instruments.data;
    UploadFMInstruments(count, instruments);
    u32 period = music.timer_period;
    SetFMMusicTimerPeriod(period);
    if (sound_var_3)
      SetDigitalSoundTimerPeriod(period);
    (void)PlayFMMusic(music.stream.data, music.stream.capacity);
  } else {
    competition_sound_pending = 0;
    if (sound_var_3) {
      if (offset >= PT_SOUND_BANK_CAPACITY || size >= PT_SOUND_BANK_CAPACITY - offset)
        pt_resource_bounds_failure("VOC resource", size, PT_SOUND_BANK_CAPACITY);
      u8 *end = pt_sound_resources.bank + offset + size;
      (void)memset(end, 0, 1);
      (void)PrepareVOCSound(pt_sound_resources.bank + offset, size + 1u);
    }
  }
  state_slot_011 = 1;
  competition_sound_id = index;
done:;
}

// Any nonzero read count succeeds, including a short file. A failed open leaves read errors disabled.

u32 LoadMusicFileToBuffer(const u8 *filename, u8 *buffer) {
  u32 result = 0;
  if (!sound_config_flag || !buffer)
    goto done;

  if (!game_ptr->sound_enabled)
    goto done;
  DisableDiskReadErrors();
  u32 opened;
  opened = OpenFileAsCurrent(filename);
  if (!(u16)opened)
    goto done;
  if (state_slot_011 == 1)
    SoundStuff1();
  u32 amount;
  amount = ReadBytesFromCurrentFile(buffer, PT_SOUND_BANK_CAPACITY);
  CloseCurrentFile();
  EnableDiskReadErrors();
  if (amount)
    result = 1;
done:;

  return result;
}

// Reserves 80 bytes even on early returns. Track indices are byte arguments, and only
// successful loads change the current track.

u32 LoadMusic(u8 index) {
  u8 filename[80];
  u32 result = 0;
  if (!sound_config_flag) {
    result = 1;
    goto done;
  }
  if (!prefs.field_2)
    goto done;
  if (current_music_index == index) {
    result = 1;
    goto done;
  }

  if (!game_ptr->sound_enabled || !pt_sound_resources.bank)
    goto done;
  snprintf((char *)filename, sizeof(filename), "sound\\m%d", (unsigned)index);
  if (state_slot_011 == 1)
    SoundStuff1();
  u32 loaded;

  loaded = LoadMusicFileToBuffer(filename, pt_sound_resources.bank);

  if ((u16)loaded) {
    current_music_index = index;
    state_slot_007 = index;
    result = 1;
  }
done:;

  return result;
}

// The driver query executes even if no sample is active; it is never skipped by inference.

void SoundStuff3(u8 previous, u8 next) {
  if (!sound_config_flag)
    return;
  SetSoundFlag(next);
  u32 result;
  if (next > previous) {
    result = ResumeFMMusic();
    if (result && state_slot_011 && sound_var_3)
      SoundStuff2(competition_sound_id);
  } else {
    result = PauseFMMusic();
    if (result && state_slot_011 && sound_var_3)
      (void)StopVOCSound();
  }
  if (!next)
    state_slot_007 = 100;
}

void SoundCrap1(u8 index) {
  if (sound_config_flag && competition_sound_id != index) {
    SoundStuff2(index);
  }
}

// Sound tables.

// Readable resident music tables; no executable-byte lookup at runtime.

#include "sound_layout.h"
const u8 pt_fm_operator_map[9] = {
  0,
  1,
  2,
  8,
  9,
  10,
  16,
  17,
  18,
};

const u8 pt_fm_default_instrument[11] = {0};

// The original table overlaps percussion bits and channel assignments.
// Note-off uses MIDI channels 11..15, so retain all sixteen physical bytes.
// The previous eleven-byte C array caused an undefined out-of-bounds read.

const u8 pt_fm_percussion_operators[16] = {
  242,
  83,
  116,
  0,
  0,
  8,
  16,
  20,
  18,
  21,
  17,
  16,
  8,
  4,
  2,
  1,
};

const u8 pt_fm_percussion_bits[11] = {
  8,
  16,
  20,
  18,
  21,
  17,
  16,
  8,
  4,
  2,
  1,
};

const u8 pt_fm_percussion_channels[11] = {
  17,
  16,
  8,
  4,
  2,
  1,
  6,
  7,
  8,
  8,
  7,
};

const u8 pt_fm_note_table[128] = {
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  32,
  33,
  34,
  35,
  36,
  37,
  38,
  39,
  40,
  41,
  42,
  43,
  48,
  49,
  50,
  51,
  52,
  53,
  54,
  55,
  56,
  57,
  58,
  59,
  64,
  65,
  66,
  67,
  68,
  69,
  70,
  71,
  72,
  73,
  74,
  75,
  80,
  81,
  82,
  83,
  84,
  85,
  86,
  87,
  88,
  89,
  90,
  91,
  96,
  97,
  98,
  99,
  100,
  101,
  102,
  103,
  104,
  105,
  106,
  107,
  112,
  113,
  114,
  115,
  116,
  117,
  118,
  119,
  120,
  121,
  122,
  123,
  112,
  113,
  114,
  115,
  116,
  117,
  118,
  119,
  120,
  121,
  122,
  123,
  123,
  123,
  123,
  123,
  123,
  123,
  123,
  123,
};

const u16 pt_fm_frequency_table[768] = {
  343,
  343,
  344,
  344,
  344,
  344,
  345,
  345,
  345,
  346,
  346,
  346,
  347,
  347,
  347,
  348,
  348,
  348,
  349,
  349,
  349,
  349,
  350,
  350,
  350,
  351,
  351,
  351,
  352,
  352,
  352,
  353,
  353,
  353,
  354,
  354,
  354,
  355,
  355,
  355,
  356,
  356,
  356,
  356,
  357,
  357,
  357,
  358,
  358,
  358,
  359,
  359,
  359,
  360,
  360,
  360,
  361,
  361,
  361,
  362,
  362,
  362,
  363,
  363,
  363,
  364,
  364,
  364,
  365,
  365,
  365,
  366,
  366,
  366,
  367,
  367,
  367,
  368,
  368,
  368,
  369,
  369,
  369,
  370,
  370,
  370,
  371,
  371,
  371,
  372,
  372,
  372,
  373,
  373,
  373,
  374,
  374,
  374,
  375,
  375,
  375,
  376,
  376,
  376,
  377,
  377,
  377,
  378,
  378,
  378,
  379,
  379,
  379,
  380,
  380,
  380,
  381,
  381,
  381,
  382,
  382,
  382,
  383,
  383,
  384,
  384,
  384,
  385,
  385,
  385,
  386,
  386,
  386,
  387,
  387,
  387,
  388,
  388,
  388,
  389,
  389,
  389,
  390,
  390,
  391,
  391,
  391,
  392,
  392,
  392,
  393,
  393,
  393,
  394,
  394,
  394,
  395,
  395,
  395,
  396,
  396,
  397,
  397,
  397,
  398,
  398,
  398,
  399,
  399,
  399,
  400,
  400,
  401,
  401,
  401,
  402,
  402,
  402,
  403,
  403,
  403,
  404,
  404,
  405,
  405,
  405,
  406,
  406,
  406,
  407,
  407,
  407,
  408,
  408,
  409,
  409,
  409,
  410,
  410,
  410,
  411,
  411,
  412,
  412,
  412,
  413,
  413,
  413,
  414,
  414,
  414,
  415,
  415,
  416,
  416,
  416,
  417,
  417,
  417,
  418,
  418,
  419,
  419,
  419,
  420,
  420,
  421,
  421,
  421,
  422,
  422,
  422,
  423,
  423,
  424,
  424,
  424,
  425,
  425,
  425,
  426,
  426,
  427,
  427,
  427,
  428,
  428,
  429,
  429,
  429,
  430,
  430,
  430,
  431,
  431,
  432,
  432,
  432,
  433,
  433,
  434,
  434,
  434,
  435,
  435,
  436,
  436,
  436,
  437,
  437,
  438,
  438,
  438,
  439,
  439,
  440,
  440,
  440,
  441,
  441,
  442,
  442,
  442,
  443,
  443,
  444,
  444,
  444,
  445,
  445,
  446,
  446,
  446,
  447,
  447,
  448,
  448,
  448,
  449,
  449,
  450,
  450,
  450,
  451,
  451,
  452,
  452,
  452,
  453,
  453,
  454,
  454,
  454,
  455,
  455,
  456,
  456,
  457,
  457,
  457,
  458,
  458,
  459,
  459,
  459,
  460,
  460,
  461,
  461,
  461,
  462,
  462,
  463,
  463,
  464,
  464,
  464,
  465,
  465,
  466,
  466,
  467,
  467,
  467,
  468,
  468,
  469,
  469,
  469,
  470,
  470,
  471,
  471,
  472,
  472,
  472,
  473,
  473,
  474,
  474,
  475,
  475,
  475,
  476,
  476,
  477,
  477,
  478,
  478,
  478,
  479,
  479,
  480,
  480,
  481,
  481,
  481,
  482,
  482,
  483,
  483,
  484,
  484,
  485,
  485,
  485,
  486,
  486,
  487,
  487,
  488,
  488,
  488,
  489,
  489,
  490,
  490,
  491,
  491,
  492,
  492,
  492,
  493,
  493,
  494,
  494,
  495,
  495,
  496,
  496,
  496,
  497,
  497,
  498,
  498,
  499,
  499,
  500,
  500,
  501,
  501,
  501,
  502,
  502,
  503,
  503,
  504,
  504,
  505,
  505,
  506,
  506,
  506,
  507,
  507,
  508,
  508,
  509,
  509,
  510,
  510,
  511,
  511,
  511,
  512,
  512,
  513,
  513,
  514,
  514,
  515,
  515,
  516,
  516,
  517,
  517,
  518,
  518,
  518,
  519,
  519,
  520,
  520,
  521,
  521,
  522,
  522,
  523,
  523,
  524,
  524,
  525,
  525,
  526,
  526,
  526,
  527,
  527,
  528,
  528,
  529,
  529,
  530,
  530,
  531,
  531,
  532,
  532,
  533,
  533,
  534,
  534,
  535,
  535,
  536,
  536,
  537,
  537,
  538,
  538,
  538,
  539,
  539,
  540,
  540,
  541,
  541,
  542,
  542,
  543,
  543,
  544,
  544,
  545,
  545,
  546,
  546,
  547,
  547,
  548,
  548,
  549,
  549,
  550,
  550,
  551,
  551,
  552,
  552,
  553,
  553,
  554,
  554,
  555,
  555,
  556,
  556,
  557,
  557,
  558,
  558,
  559,
  559,
  560,
  560,
  561,
  561,
  562,
  562,
  563,
  563,
  564,
  564,
  565,
  565,
  566,
  566,
  567,
  567,
  568,
  568,
  569,
  569,
  570,
  571,
  571,
  572,
  572,
  573,
  573,
  574,
  574,
  575,
  575,
  576,
  576,
  577,
  577,
  578,
  578,
  579,
  579,
  580,
  580,
  581,
  581,
  582,
  582,
  583,
  584,
  584,
  585,
  585,
  586,
  586,
  587,
  587,
  588,
  588,
  589,
  589,
  590,
  590,
  591,
  591,
  592,
  593,
  593,
  594,
  594,
  595,
  595,
  596,
  596,
  597,
  597,
  598,
  598,
  599,
  600,
  600,
  601,
  601,
  602,
  602,
  603,
  603,
  604,
  604,
  605,
  606,
  606,
  607,
  607,
  608,
  608,
  609,
  609,
  610,
  610,
  611,
  612,
  612,
  613,
  613,
  614,
  614,
  615,
  615,
  616,
  617,
  617,
  618,
  618,
  619,
  619,
  620,
  620,
  621,
  622,
  622,
  623,
  623,
  624,
  624,
  625,
  626,
  626,
  627,
  627,
  628,
  628,
  629,
  629,
  630,
  631,
  631,
  632,
  632,
  633,
  633,
  634,
  635,
  635,
  636,
  636,
  637,
  637,
  638,
  639,
  639,
  640,
  640,
  641,
  642,
  642,
  643,
  643,
  644,
  644,
  645,
  646,
  646,
  647,
  647,
  648,
  649,
  649,
  650,
  650,
  651,
  651,
  652,
  653,
  653,
  654,
  654,
  655,
  656,
  656,
  657,
  657,
  658,
  659,
  659,
  660,
  660,
  661,
  662,
  662,
  663,
  663,
  664,
  665,
  665,
  666,
  666,
  667,
  668,
  668,
  669,
  669,
  670,
  671,
  671,
  672,
  672,
  673,
  674,
  674,
  675,
  675,
  676,
  677,
  677,
  678,
  678,
  679,
  680,
  680,
  681,
  682,
  682,
  683,
  683,
  684,
  685,
  685,
};

const u16 pt_fm_velocity_table[33] = {
  65520,
  65522,
  65524,
  65526,
  65528,
  65532,
  65534,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  22608,
  40131,
  20510,
  49195,
  55438,
  53592,
  53731,
  64227,
  1929,
  22409,
  7938,
  50077,
  7836,
  49195,
  55438,
  58321,
  58321,
};

const u8 pt_fm_default_instruments[256] = {0};

// Initial scalar data of the bundled PCM device; no resident code image is a
// live native state object.

const PTPCMResident pt_pcm_initial_state = {.version = 340,
  .signature = {'D', 'I', 'G', 'P', 'A', 'K'},
  .playback = {
    .auto_dma = 0,
    .playing = 0,
    .callback_enabled = 0,
    .stereo = 0,
    .sample_format = 0,
    .flat_pointers = 0,
    .stop_count = 0,
    .queued = 0,
    .active_half = 1,
    .extended_dsp = 0,
  }};

// Start native device state; musical instruments are supplied by the user's CMF banks.
void pt_native_audio_start(void) {
  static PTFMResident resident;
  static u8 status;
  resident = (PTFMResident){
    .version = 256, .opl_port = 0x388, .restore_period = 65535, .current_period = 65535, .status = {&status, 0, 1}};
  pt_opl_reset(&fm_synth);
  fm_output = (PTFMOutputState){.enabled = true};
  FMResetDriver(&resident);
  FMProgramPIT(&resident, 65535);
  pt_fm_device_bind(0x80, &resident);
  pt_fm_timer_bind(&resident);
  sound_config_flag = 1;
  sound_var_3 = 1;
}
