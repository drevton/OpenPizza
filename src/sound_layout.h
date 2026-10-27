// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_SOUND_LAYOUT_H
#define PT_SOUND_LAYOUT_H
#include "memory.h"
#include "render_types.h"
#include "sound_types.h"
#include "sound_types.h"

enum { PT_SOUND_BANK_CAPACITY = 230000 };
enum { PT_SOUND_INTERRUPT = 0x66, PT_PCM_IRQ_INTERRUPT = 0x0f, PT_BIOS_TIMER_INTERRUPT = 8 };
typedef struct PTPCMSampleBuffer {
  const u8 *bytes;
} PTPCMSampleBuffer;
typedef struct PTSoundDMATransfer {
  PTPCMSampleBuffer samples;
  u16 length;
  u16 frequency;
} PTSoundDMATransfer;
// Playback owns native scalars; DOS code gaps, vectors and encoded callbacks
// belong to the saved resident format.

typedef struct PTPCMPlayback {
  u16 auto_dma, playing, callback_enabled, stereo, sample_format, flat_pointers;
  u16 stop_count, queued;
  u8 active_half;
  u16 extended_dsp;
  PTSoundDMATransfer queued_record;
} PTPCMPlayback;
typedef struct PTPCMResident {
  u16 version;
  u8 signature[6];
  PTPCMPlayback playback;
} PTPCMResident;
// A native bridge contains its selected endpoint and asset identity only.

struct PTSoundVector {
  PTSoundEndpointKind endpoint_kind;
  bool valid_asset;
};
typedef struct __attribute__((packed, may_alias)) PTSoundBankEntry {
  u32 kind, offset, size;
} PTSoundBankEntry;
typedef struct __attribute__((packed, may_alias)) PTSoundBank {
  u32 count;
  PTSoundBankEntry entries[];
} PTSoundBank;
_Static_assert(sizeof(PTSoundBank) == 4 && sizeof(PTSoundBankEntry) == 12, "sound bank records");
typedef struct __attribute__((packed, may_alias)) PTMusicHeader {
  u8 unknown0[6];
  i16 instruments_offset, stream_offset;
  u8 unknowna[2];
  i16 rate;
  u8 unknowne[36 - 14];
  i16 instrument_count;
} PTMusicHeader;
_Static_assert(sizeof(PTMusicHeader) == 38 && offsetof(PTMusicHeader, instrument_count) == 36, "music header");
// Bounded, borrowed file bytes; no original address or allocator identity.

typedef struct PTSoundBytes {
  const u8 *data;
  u32 capacity;
} PTSoundBytes;
typedef struct PTMusicView {
  const PTMusicHeader *header;
  PTSoundBytes instruments, stream;
  u32 timer_period;
} PTMusicView;
enum { PT_VOC_HEADER_BYTES = 26 };
typedef enum PTVOCBlockKind {
  PT_VOC_END,
  PT_VOC_SOUND,
  PT_VOC_CONTINUATION,
  PT_VOC_SILENCE,
  PT_VOC_MARKER,
  PT_VOC_TEXT,
  PT_VOC_REPEAT_BEGIN,
  PT_VOC_REPEAT_END
} PTVOCBlockKind;
typedef struct __attribute__((packed, may_alias)) PTVOCBlockLength {
  u8 bytes[3];
} PTVOCBlockLength;
typedef struct __attribute__((packed, may_alias)) PTVOCMarker {
  u8 tag;
} PTVOCMarker;
typedef struct __attribute__((packed, may_alias)) PTVOCBlockHeader {
  PTVOCMarker marker;
  PTVOCBlockLength length;
} PTVOCBlockHeader;
typedef struct __attribute__((packed, may_alias)) PTVOCSoundBlock {
  PTVOCBlockHeader header;
  u8 time_constant, compression;
} PTVOCSoundBlock;
_Static_assert(sizeof(PTVOCBlockHeader) == 4 && sizeof(PTVOCSoundBlock) == 6, "VOC block prefixes");
typedef struct __attribute__((packed, may_alias)) PTSoundSelectorOutput {
  u16 selector;
} PTSoundSelectorOutput;
typedef struct __attribute__((packed, may_alias)) PTSoundFileSize {
  u32 bytes;
} PTSoundFileSize;
_Static_assert(sizeof(PTVOCBlockLength) == 3 && sizeof(PTVOCMarker) == 1 && sizeof(PTSoundSelectorOutput) == 2 &&
        sizeof(PTSoundFileSize) == 4,
    "sound scalar records");
typedef struct __attribute__((packed, may_alias)) PTVOCRepeatBlock {
  u8 tag, length[3];
  u16 repeats;
} PTVOCRepeatBlock;
_Static_assert(sizeof(PTVOCRepeatBlock) == 6 && offsetof(PTVOCRepeatBlock, repeats) == 4, "VOC repeat block");
enum { PT_FM_CHANNELS = 16, PT_FM_VOICES = 11, PT_FM_CHANNEL_PAIRS = 8 };

typedef struct PTFMFadeState {
  u8 current, target;
  u16 counter, period;
  u8 step;
} PTFMFadeState;
typedef struct PTFMVoices {
  u8 channels[PT_FM_VOICES], notes[PT_FM_VOICES], keys[PT_FM_VOICES], levels[PT_FM_VOICES], volumes[PT_FM_VOICES],
      original_volumes[PT_FM_VOICES];
  u16 frequencies[PT_FM_VOICES], ages[PT_FM_VOICES];
} PTFMVoices;
typedef union PTFMWord {
  u16 word;
  struct {
    u8 low, high;
  };
} PTFMWord;
typedef union PTFMDword {
  u32 value;
  struct {
    u16 low, high;
  };
} PTFMDword;
typedef struct PTFMResident PTFMResident;

// Native resource cursors retain their buffer and wrapping WORD offset.

typedef struct PTFMInstrumentTone {
  u8 modulator_characteristics, carrier_characteristics;
  u8 modulator_level, carrier_level;
  u8 modulator_attack_decay, carrier_attack_decay;
  u8 modulator_sustain_release, carrier_sustain_release;
  u8 modulator_waveform, carrier_waveform;
  u8 feedback_connection;
} PTFMInstrumentTone;
_Static_assert(sizeof(PTFMInstrumentTone) == 11, "FM tone file bytes");
typedef struct PTFMBuffer {
  const u8 *base;
  u16 offset;
  u32 capacity;
} PTFMBuffer;
// Live resident state has no fixed DOS stride.

struct PTFMResident {

  u16 version;
  u16 opl_port;
  u16 flags;
  PTFMBuffer instruments, stream, position, stream_start;
  PTFMDword delta;
  PTFMBuffer status;
  u16 address_delay, data_delay;
  u16 restore_period, period, current_period, bios_remainder, counter;
  PTFMWord voice_count, channel, event_kind;
  u16 event_count, control;
  PTFMVoices voices;
  union {
    PTFMFadeState fade;
    u16 volume_pair;
  };
  u8 instrument_count, playing, event_note, event_velocity, busy;
  PTFMWord rhythm;
  u8 programs[PT_FM_CHANNELS];
  u16 pitch_offsets[PT_FM_CHANNELS];
  union {
    u8 enabled_channels[PT_FM_CHANNELS];
    u16 channel_pairs[PT_FM_CHANNEL_PAIRS];
  };
  PTFMBuffer default_instruments;
};
PTFMBuffer pt_fm_buffer_rebase(PTFMBuffer buffer, u32 bytes);

_Noreturn void pt_resource_bounds_failure(const char *, i64, size_t);

u8 *pt_fm_status_byte(const PTFMResident *driver);
const u8 *pt_fm_stream_bytes(PTFMBuffer buffer, u16 offset, size_t size);
// The driver's vector query accepts a signed byte slot; vector assignment
// accepts an unsigned byte slot. Preserve the resulting physical window.

// Live audio operations resolve the resource owner once at this interface.

PTSoundDMATransfer *pt_current_sound_dma_transfer(void);
const PTSoundBank *pt_loaded_sound_bank(void);
#endif
