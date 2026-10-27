// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_SOUND_H
#define PT_SOUND_H
#include "command.h"
#include "state_types.h"
#include "runtime_state.h"
#include "sound_layout.h"
PTPCMPlayback *pt_pcm_playback(PTPCMResident *image);

typedef enum PTFMService {
  PT_FM_VERSION,
  PT_FM_INITIALIZE,
  PT_FM_INSTRUMENTS,
  PT_FM_RESTORE_PERIOD,
  PT_FM_PERIOD,
  PT_FM_CONTROL,
  PT_FM_PLAY,
  PT_FM_STOP,
  PT_FM_SHUTDOWN,
  PT_FM_PAUSE,
  PT_FM_RESUME,
  PT_FM_VOLUME = 14,
  PT_FM_FLAGS
} PTFMService;
typedef struct PTFMCommand {
  PTFMService service;
  u16 value, auxiliary, count, interval;
  PTFMBuffer resource;
} PTFMCommand;
typedef struct PTFMReply {
  u16 value;
  PTFMBuffer previous;
} PTFMReply;
PTFMReply pt_fm_command(PTFMResident *driver, PTFMCommand command);
void pt_fm_devices_reset(void);
void pt_fm_device_bind(u8 interrupt, PTFMResident *driver);
typedef struct PTFMDeviceBindings {
  PTFMResident *drivers[256], *timer;
} PTFMDeviceBindings;
void pt_fm_timer_bind(PTFMResident *driver);
PTFMResident *pt_fm_device(u8 interrupt);
bool pt_fm_device_probe(u8 interrupt);
PTFMBuffer pt_fm_resource(const u8 *bytes, u32 capacity);
PTFMResident *pt_fm_timer_device(void);
u32 FMReadDelta(PTFMResident *driver);
u16 FMStartMusic(PTFMResident *driver, PTFMBuffer stream);
void FMProcessEvents(PTFMResident *driver);
u32 LoadMusic(u8 index);
u32 LoadMusicFileToBuffer(const u8 *filename, u8 *buffer);
// PCM samples and playback positions are independent of host allocation addresses.

typedef struct PTPCMPlaybackState {
  u32 installed, active, queued;
  PTPCMSampleBuffer queued_samples, current_samples;
  u32 queued_length, queued_frequency;
  u32 current_length, current_frequency, current_position;
  u32 block_size, block_position;
} PTPCMPlaybackState;
typedef struct PTSoundEndpoint {
  bool bound;
  PTSoundEndpointKind kind, previous_pcm;
} PTSoundEndpoint;
void pt_sound_endpoint_bind(PTSoundEndpointKind kind, PTSoundEndpointKind previous_pcm);
// Native transfer owners can be bound without a serialized packet codec.

PTPCMSampleBuffer pt_pcm_resource(const void *bytes);
PTPCMSampleBuffer pt_pcm_transfer_samples(const PTSoundDMATransfer *record);
void pt_pcm_resident_storage_reset(void);
// General transfers are caller-owned; the audio device owns its DMA record.

PTSoundDMATransfer *pt_pcm_transfer_allocate(void);
PTSoundDMATransfer *pt_sound_dma_transfer_create(void);
void pt_sound_dma_transfer_reset(void);
void pt_pcm_driver_restore(const PTPCMPlaybackState *state);
const PTPCMPlaybackState *pt_pcm_driver_state(void);
typedef struct PTFMOutputState {
  bool enabled;
  u8 registers[256];
} PTFMOutputState;
const PTFMOutputState *pt_fm_output_state(void);
bool pt_fm_output_restore(const PTFMOutputState *state);
bool pt_fm_synth_load(const void *data, size_t size);
bool pt_audio_output(void *data, size_t capacity, size_t *size);
bool pt_audio_load_output(const void *data, size_t size);
void pt_pcm_driver_reset_platform(void);
enum { PT_PCM_OUTPUT_CAPACITY = 4096 };
u32 pt_pcm_buffer_length(u32 word);
u32 pt_pcm_driver_samples(u32 count);
PTCommandStatus pt_pcm_samples_ready(u32 count);
PTCommandStatus pt_sound_timer_ready(void);
bool pt_pcm_driver_load_output(const void *data, size_t size);
// Logical audio timer state owns actual interrupt handlers.

typedef struct PTSoundTimer {
  u32 reload, counter;
  PTRuntimeCallback handler, previous_handler;
} PTSoundTimer;
const PTSoundTimer *pt_sound_timer_state(void);
bool pt_sound_timer_restore(const PTSoundTimer *state);
PTRuntimeCallback pt_sound_timer_bios_handler(void);
PTRuntimeCallback pt_sound_timer_callback_handler(void);
bool pt_sound_timer_installed(void);
bool pt_sound_timer_uses_bios(void);
bool pt_fm_timer_installed(void);
u32 pt_sound_timer_tick(void);
PTRuntimeCallback InstallSoundTimer(PTRuntimeCallback callback, u32 period);
void RestoreSoundTimer(void);
PTRuntimeCallback pt_sound_pcm_timer_callback(void);
u8 SoundBCDToBinary(u8 value);
u32 InitializeDigitalSound(void);
u32 ShutdownDigitalSound(void);
u32 UninstallPCMDriver(PTPCMResident *image);
u32 TerminatePCMDriver(void);
u32 UnloadPCMDriver(void);
void SetDigitalSoundTimerPeriod(u32 period);
u32 QueryPCMDriverStatus(void);
u32 QueryPCMDriverPosition(void);
void PCMDriverCommit(PTSoundDMATransfer *record);
u32 PCMDriverQueueBuffer(PTSoundDMATransfer *record);
u32 PCMDriverPlayBuffer(PTSoundDMATransfer *record);
u32 SetPCMDriverAutoDMA(u32 enabled);
u32 SetPCMDriverSampleFormat(u32 format);
u32 CheckPCMDMABuffer(const void *buffer, u32 length);
u32 SubmitPCMBuffer(PTSoundDMATransfer *record);
u32 GetPCMOutputStatus(void);
PTActionResult SoundPCMCallback(void);
u32 SoundFillBytes(void *target, u32 size, u32 value);
void SoundCopyBytes(void *target, const void *source, u32 size);
u8 *DpmiAllocateDosBlock(u32 bytes, u16 *selector);
u32 DpmiFreeDosBlock(u32 selector);
u8 *AllocateSoundDOSBlock(u32 bytes);
u32 FreeSoundDOSBlock(const void *address);
u8 *LoadSoundDOSFile(const u8 *filename, PTSoundFileSize *length);
u32 InstallSoundVector(PTSoundVector *image);
u32 EnsureSoundVector(void);
u32 ProbePCMDriver(void);
u32 LoadPCMDriver(const u8 *filename);
u32 InstallPCMDriver(PTPCMResident *image);
u32 InitializePCMDriver(void);
u32 QueryPCMDriverVersion(void);
u32 QueryPCMDriverCapabilities(void);
void SetPCMDriverFlatPointers(u32 enabled);
const u8 *pt_sound_bytes_at(PTSoundBytes bytes, u32 offset, u32 size);
void pt_sound_bytes_require(PTSoundBytes bytes, const void *cursor, u32 size);
const u8 *pt_sound_bytes_advance(PTSoundBytes bytes, const u8 *cursor, u32 size);
PTMusicView pt_music_view(PTSoundBytes bytes);
u32 ReadVOCBlockSize(const PTVOCBlockLength *length);
PTSoundDMATransfer *PrepareVOCSound(const u8 *data, u32 capacity);
u32 StopVOCSound(void);
void SoundStuff1(void);
void SoundCrap1(u8 index);
void SoundStuff2(u8 index);
void SoundStuff3(u8 previous, u8 next);
void SetSoundFlag(u8 value);
u8 GetSoundFlag(void);
u32 QueryFMDriverVersion(void);
void InitializeFMDriver(u8 *status);
void UploadFMInstruments(u32 count, const u8 *instruments);
void SetFMMusicTimerPeriod(u32 period);
u32 PlayFMMusic(const u8 *stream, u32 capacity);
u32 PauseFMMusic(void);
u32 ResumeFMMusic(void);
u32 StopFMMusic(void);
u32 ShutdownFMDriver(void);
u32 FindFMDriver(void);
void InitSound(void);
void StopSound(void);
u16 WantSkipGameIntro(void);
u32 AllocateSoundBuffer(void);
u32 AllocateIntroBuffer(void);
void FreeIntroBuffer(void);
u32 LoadIntroMusic(void);
#endif
