"""VOC PCM/WAV conversion and editable CMF MIDI events/instruments."""
import json
import struct
import wave
from .common import bounded, safe_path, save_hex, load_hex, write_json, read_related, read_events

EVENT_NAMES = {8: 'note_off', 9: 'note_on', 10: 'poly_pressure', 11: 'control_change',
    12: 'program_change', 13: 'channel_pressure', 14: 'pitch_bend'}
CMF_FIELDS = ['version', 'instruments_offset', 'stream_offset', 'ticks_per_quarter', 'ticks_per_second',
    'title_offset', 'composer_offset', 'remarks_offset']
INSTRUMENT_FIELDS = ['modulator_characteristic', 'carrier_characteristic', 'modulator_level', 'carrier_level',
    'modulator_attack_decay', 'carrier_attack_decay', 'modulator_sustain_release', 'carrier_sustain_release',
    'modulator_waveform', 'carrier_waveform', 'feedback_connection']


def vlq(value):
  if not isinstance(value, int) or not 0 <= value <= 0x0fffffff:
    raise ValueError('MIDI variable-length integer out of range')
  result = [value & 127]
  value >>= 7
  while value:
    result.insert(0, 128 | (value & 127))
    value >>= 7
  return bytes(result)


def read_vlq(data, at):
  start, value = at, 0
  for _ in range(4):
    byte = bounded(data, at, 1)[0]
    at += 1
    value = (value << 7) | (byte & 127)
    if not byte & 128:
      return value, at, data[start:at]
  raise ValueError('Overlong MIDI variable-length integer')


def preserved_vlq(event, key):
  value = event[key]
  original = bytes(event.get(key + '_encoding', []))
  if original:
    decoded, end, unused = read_vlq(original, 0)
    if end != len(original):
      raise ValueError('Invalid preserved VLQ bytes')
    if decoded == value:
      return original
  return vlq(value)


def midi_decode(data):
  at, running, events = 0, None, []
  while at < len(data):
    delta, at, encoded = read_vlq(data, at)
    status = bounded(data, at, 1)[0]
    implicit = status < 128
    if implicit:
      if running is None:
        raise ValueError('MIDI running status without a previous channel event')
      status = running
    else:
      at += 1
    event = dict(delta=delta)
    if encoded != vlq(delta):
      event['delta_encoding'] = list(encoded)
    if status < 240:
      running = status
      kind = status >> 4
      size = 1 if kind in (12, 13) else 2
      payload = bounded(data, at, size)
      if any(value >= 128 for value in payload):
        raise ValueError('Status byte inside MIDI channel data')
      event.update(type=EVENT_NAMES[kind], channel=status & 15, running_status=implicit)
      names = {8: ['note', 'velocity'], 9: ['note', 'velocity'], 10: ['note', 'pressure'],
          11: ['controller', 'value'], 12: ['program'], 13: ['pressure'], 14: ['lsb', 'msb']}[kind]
      event.update(zip(names, payload))
      at += size
    elif status in (240, 247, 255):
      if status == 255:
        event.update(type='meta', meta_type=bounded(data, at, 1)[0])
        at += 1
      else:
        event.update(type='sysex', status=status)
        running = None
      size, at, encoded = read_vlq(data, at)
      payload = bounded(data, at, size)
      event['length'] = size
      if encoded != vlq(size):
        event['length_encoding'] = list(encoded)
      if event['type'] == 'meta' and event['meta_type'] in range(1, 10):
        event['text'] = payload.decode('cp437')
      else:
        event['data'] = list(payload)
      at += size
    else:
      size = {241: 1, 242: 2, 243: 1, 246: 0, 248: 0, 250: 0, 251: 0, 252: 0, 254: 0}.get(status)
      if size is None:
        raise ValueError('Unsupported MIDI system event: ' + hex(status))
      event.update(type='system', status=status, data=list(bounded(data, at, size)))
      at += size
    events.append(event)
    if (event['type'] == 'meta' and event['meta_type'] == 47) or status == 252:
      break
  return events, data[at:]


def midi_encode(events):
  output, running = bytearray(), None
  for event in events:
    output.extend(preserved_vlq(event, 'delta'))
    kind = next((kind for kind, name in EVENT_NAMES.items() if name == event['type']), None)
    if kind:
      channel = event['channel']
      if not 0 <= channel <= 15:
        raise ValueError('MIDI channel outside 0..15')
      status = kind * 16 + channel
      if event['running_status']:
        if status != running:
          raise ValueError('Edited MIDI running status no longer matches; set running_status false')
      else:
        output.append(status)
      running = status
      names = {8: ['note', 'velocity'], 9: ['note', 'velocity'], 10: ['note', 'pressure'],
          11: ['controller', 'value'], 12: ['program'], 13: ['pressure'], 14: ['lsb', 'msb']}[kind]
      values = [event[name] for name in names]
      if any(not isinstance(value, int) or not 0 <= value < 128 for value in values):
        raise ValueError('MIDI channel data outside 0..127')
      output.extend(values)
    elif event['type'] in ('meta', 'sysex'):
      payload = event['text'].encode('cp437') if 'text' in event else bytes(event['data'])
      output.extend((255, event['meta_type']) if event['type'] == 'meta' else (event['status'],))
      sized = dict(event, length=len(payload))
      output.extend(preserved_vlq(sized, 'length'))
      output.extend(payload)
      if event['type'] == 'sysex':
        running = None
    elif event['type'] == 'system':
      output.append(event['status'])
      output.extend(event['data'])
    else:
      raise ValueError('Unknown MIDI event type')
  return bytes(output)


def decode_cmf(data, root):
  if bounded(data, 0, 4) != b'CTMF':
    raise ValueError('Expected CMF music')
  header = dict(zip(CMF_FIELDS, struct.unpack('<8H', bounded(data, 4, 16))))
  if header['version'] != 257:
    raise ValueError('Unsupported CMF version; expected the original game version 1.1')
  header['channels_used'] = list(bounded(data, 20, 16))
  header['instrument_count'], header['basic_tempo'] = struct.unpack('<HH', bounded(data, 36, 4))
  start, stream = header['instruments_offset'], header['stream_offset']
  end = start + header['instrument_count'] * 16
  if not 40 <= start <= end <= stream <= len(data):
    raise ValueError('CMF offsets overlap or exceed the file')
  instruments = []
  for at in range(start, end, 16):
    row = dict(zip(INSTRUMENT_FIELDS, data[at:at + 11]))
    row['reserved'] = list(data[at + 11:at + 16])
    instruments.append(row)
  events, tail = midi_decode(data[stream:])
  write_json(root / 'instruments.json', instruments)
  with (root / 'events.jsonl').open('w', encoding='utf-8', newline='') as output:
    for event in events:
      output.write(json.dumps(event, ensure_ascii=False, allow_nan=False) + '\n')
  return dict(codec='cmf', header=header, preamble=save_hex(root, 'preamble.hex', data[40:start]),
      instruments='instruments.json', instrument_gap=save_hex(root, 'instrument-gap.hex', data[end:stream]),
      events='events.jsonl', tail_hex=tail.hex(),
      note='CMF is FM instructions, not PCM; JSON instruments/events are reversible, a rendered WAV is not.')


def encode_cmf(meta, root):
  header = meta['header']
  preamble = load_hex(root, meta['preamble'])
  instruments = read_related(root, meta['instruments'])
  patches = bytearray()
  for row in instruments:
    reserved = bytes(row['reserved'])
    if len(reserved) != 5:
      raise ValueError('CMF reserved instrument width changed')
    patches.extend(bytes(row[name] for name in INSTRUMENT_FIELDS) + reserved)
  gap = load_hex(root, meta['instrument_gap'])
  # Preserve all original offset words, including invalid unused title pointers.
  if len(instruments) != header['instrument_count'] or header['instruments_offset'] != 40 + len(preamble):
    raise ValueError('CMF header/instrument count changed inconsistently')
  if header['stream_offset'] != 40 + len(preamble) + len(patches) + len(gap):
    raise ValueError('CMF stream offset differs from its sections')
  fields = struct.pack('<8H', *(header[name] for name in CMF_FIELDS))
  flags = bytes(header['channels_used'])
  if len(flags) != 16:
    raise ValueError('CMF channel flags width changed')
  fixed = b'CTMF' + fields + flags + struct.pack('<HH', header['instrument_count'], header['basic_tempo'])
  events = read_events(root, meta['events'])
  return fixed + preamble + patches + gap + midi_encode(events) + bytes.fromhex(meta['tail_hex'])


def write_wav(root, name, samples, rate, channels=1, width=1):
  with wave.open(str(safe_path(root, name)), 'wb') as output:
    output.setnchannels(channels)
    output.setsampwidth(width)
    output.setframerate(rate)
    output.writeframes(samples)
  return dict(file=name, rate=rate, channels=channels, sample_width=width)


def read_wav(root, reference):
  with wave.open(str(safe_path(root, reference['file'])), 'rb') as source:
    if (source.getframerate(), source.getnchannels(), source.getsampwidth(), source.getcomptype()) != \
        (reference['rate'], reference['channels'], reference['sample_width'], 'NONE'):
      raise ValueError('WAV must retain its declared PCM rate, channel count and sample width')
    result = source.readframes(source.getnframes())
    if len(result) != source.getnframes() * source.getnchannels() * source.getsampwidth():
      raise ValueError('Truncated WAV samples')
    return result


def decode_voc(data, root):
  if bounded(data, 0, 20) != b'Creative Voice File\x1a':
    raise ValueError('Expected Creative VOC signature')
  offset, version, checksum = struct.unpack('<HHH', bounded(data, 20, 6))
  if offset < 26:
    raise ValueError('Invalid VOC data offset')
  preamble = bounded(data, 26, offset - 26)
  at, rate, blocks = offset, 11025, []
  while at < len(data):
    tag = data[at]
    at += 1
    if tag == 0:
      blocks.append(dict(tag=0))
      break
    size = int.from_bytes(bounded(data, at, 3), 'little')
    at += 3
    payload = bounded(data, at, size)
    at += size
    block = dict(tag=tag)
    if tag == 1:
      time_constant, compression = bounded(payload, 0, 2)
      if compression != 0:
        raise ValueError('Unsupported VOC ADPCM compression; refusing a misleading PCM export')
      rate = round(1000000 / (256 - time_constant))
      block.update(time_constant=time_constant, compression=compression,
          wav=write_wav(root, f'block-{len(blocks):03d}.wav', payload[2:], rate))
    elif tag == 2:
      block['wav'] = write_wav(root, f'block-{len(blocks):03d}.wav', payload, rate)
    elif tag == 3:
      duration, constant = struct.unpack('<HB', bounded(payload, 0, 3))
      block.update(duration=duration, time_constant=constant, tail_hex=payload[3:].hex())
    elif tag == 4:
      block.update(marker=struct.unpack('<H', bounded(payload, 0, 2))[0], tail_hex=payload[2:].hex())
    elif tag == 5:
      block['text'] = payload.decode('cp437')
    elif tag == 6:
      block.update(repeats=struct.unpack('<H', bounded(payload, 0, 2))[0], tail_hex=payload[2:].hex())
    else:
      block['payload_hex'] = payload.hex()
    blocks.append(block)
  return dict(codec='voc', header=dict(data_offset=offset, version=version, checksum=checksum),
      preamble_hex=preamble.hex(), blocks=blocks, tail_hex=data[at:].hex(),
      note='Each PCM block is a WAV. Repeat/silence markers and original time constants stay in this JSON.')


def encode_voc(meta, root):
  header = meta['header']
  output = bytearray(b'Creative Voice File\x1a' + struct.pack('<HHH',
      header['data_offset'], header['version'], header['checksum']) + bytes.fromhex(meta['preamble_hex']))
  if len(output) != header['data_offset']:
    raise ValueError('VOC header offset differs')
  for block in meta['blocks']:
    tag = block['tag']
    if tag == 0:
      output.append(0)
      continue
    if tag == 1:
      payload = bytes((block['time_constant'], block['compression'])) + read_wav(root, block['wav'])
    elif tag == 2:
      payload = read_wav(root, block['wav'])
    elif tag == 3:
      payload = struct.pack('<HB', block['duration'], block['time_constant']) + bytes.fromhex(block['tail_hex'])
    elif tag == 4:
      payload = struct.pack('<H', block['marker']) + bytes.fromhex(block['tail_hex'])
    elif tag == 5:
      payload = block['text'].encode('cp437')
    elif tag == 6:
      payload = struct.pack('<H', block['repeats']) + bytes.fromhex(block['tail_hex'])
    else:
      payload = bytes.fromhex(block['payload_hex'])
    if len(payload) > 0xffffff:
      raise ValueError('VOC block exceeds its 24-bit length')
    output.extend(bytes((tag,)) + len(payload).to_bytes(3, 'little') + payload)
  return bytes(output) + bytes.fromhex(meta['tail_hex'])
