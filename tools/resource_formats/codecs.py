"""Format dispatch and compound resources. Every original byte has one authoritative representation."""
from pathlib import PurePosixPath
import re
import struct
from . import audio, images, records
from .common import bounded, safe_path, read_json, write_json, save_hex, load_hex, png_read, png_write, png_image
from .common import read_related


def byte_table(data, root, description='Unidentified byte table; numeric values are authoritative'):
  write_json(root / 'bytes.json', dict(description=description, rows=[list(data[at:at + 32])
      for at in range(0, len(data), 32)]))
  return dict(codec='byte-table', table='bytes.json', semantic_layout='unidentified')


def opaque(data, root, reason):
  return dict(codec='opaque-hex', hex=save_hex(root, 'bytes.hex', data), reason=reason,
      semantic_layout='uninterpreted; readable hex and ASCII columns, not a decoded media format')


def decode(data, root, name, palette=None):
  root.mkdir(parents=True, exist_ok=False)
  path = PurePosixPath(name.lower())
  if path.suffix == '.vga':
    result = images.decode_vga(data, root, palette)
  elif path.suffix == '.bob':
    result = images.decode_bob(data, root, palette)
  elif path.suffix == '.fnt':
    result = images.decode_font(data, root)
  elif path.name == 'palette.e':
    result = images.decode_palettes(data, root)
  elif data.startswith(b'Creative Voice File\x1a'):
    result = audio.decode_voc(data, root)
  elif data.startswith(b'CTMF'):
    result = audio.decode_cmf(data, root)
  elif re.fullmatch(r'm\d+', path.name) and path.parent.name == 'sound':
    result = decode_bank(data, root, palette)
  elif path.suffix == '.cat':
    result = decode_cat(data, root, palette)
  elif path.parent.name == 'chr' and path.suffix == '.pc':
    bounded(data, 0, 3177)
    result = dict(codec='character', header=list(data[:5]),
        portrait=png_write(root, 'portrait.png', 52, 59, data[5:3073], palette),
        character=records.decode('Character', data[3073:3177]), tail_hex=data[3177:].hex())
  elif path.parent.name == 'dat' and re.fullmatch(r'(?:map|bez)\d+\.dat', path.name):
    width, height = (160, 120) if path.name.startswith('map') else (20, 15)
    pixels = bounded(data, 0, width * height)
    result = dict(codec='cell-map', image=png_write(root, 'cells.png', width, height, pixels, mode='L'),
        tail_hex=data[len(pixels):].hex(), note='Grayscale values are cell IDs, not a rendered town map.')
  elif path.parent.name == 'gfx' and re.fullmatch(r'offsets\d+', path.name):
    if len(data) % 4:
      raise ValueError('Partial sprite rectangle')
    result = dict(codec='rectangles', rectangles=[records.decode('SpriteRect', data[at:at + 4])
        for at in range(0, len(data), 4)])
  elif path.parent.name in ('f', 'q') and re.fullmatch(r'f\d+\.pc', path.name):
    result = decode_restaurant(data)
  elif path.parent.name == 'd' and path.name == 'kachinfo.pc':
    count = bounded(data, 0, 1)[0]
    patterns = [records.decode('FloorPattern', bounded(data, 1 + index * 6, 6)) for index in range(count)]
    result = dict(codec='floor-patterns', patterns=patterns, tail_hex=data[1 + count * 6:].hex())
  elif path.parent.name == 'chr' and path.name == 'amarkt':
    count = bounded(data, 0, 1)[0]
    roles = bounded(data, 1, count)
    result = dict(codec='staff-market', roles=list(roles), tail_hex=data[1 + count:].hex(),
        note='One original role byte per character index; values are retained without invented labels.')
  elif path.name == 'prefs':
    size = records.SCHEMAS['PTPreferencesFileImage']['size']
    result = dict(codec='preferences', preferences=records.decode('PTPreferencesFileImage', bounded(data, 0, size)),
        tail_hex=data[size:].hex())
  elif path.suffix == '.mis':
    result = dict(codec='missions', columns=records.decode('PTMissionGoals', bounded(data, 0, 400)),
        tail_hex=data[400:].hex())
  elif path.parent.name == 'r' and path.suffix == '.pc':
    size = struct.unpack('<I', bounded(data, 0, 4))[0]
    payload = bounded(data, 4, size)
    if size < 292 or (size - 292) % 4:
      raise ValueError('Recipe does not contain complete four-byte entries')
    result = dict(codec='recipe', header=records.decode('PTSavedRecipe', payload[:292], extent=292),
        entries=[records.decode('RecipeEntry', payload[at:at + 4]) for at in range(292, size, 4)],
        tail_hex=data[4 + size:].hex())
  elif path.parent.name == 'dat' and re.fullmatch(r'city\d+\.pc', path.name):
    result = decode_city(data, root)
  elif path.suffix == '.piz':
    if len(data) % 86:
      raise ValueError('Partial high-score record')
    result = dict(codec='high-scores', scores=[records.decode('HighScoreRecord', data[at:at + 86])
        for at in range(0, len(data), 86)])
  elif path.suffix == '.cfg' and path.name == 'sound.cfg':
    result = dict(codec='sound-config', configuration=records.decode('PTSoundConfiguration', bounded(data, 0, 18)),
        tail_hex=data[18:].hex())
  elif path.suffix in ('.txt', '.bat', '.ini', '.idc') or (path.suffix == '.e' and path.name != 'palette.e'):
    with (root / 'text.txt').open('w', encoding='utf-8', newline='') as output:
      output.write(data.decode('cp437'))
    result = dict(codec='dos-text', text='text.txt', encoding='cp437',
        note='UTF-8 editing copy; original CR/LF and control characters must be retained for exact reconstruction.')
  elif path.suffix in ('.exe', '.com', '.pif', '.gif', '.db'):
    result = opaque(data, root, 'Program, installer, or auxiliary format')
    if path.suffix == '.gif':
      from PIL import Image
      import io
      with Image.open(io.BytesIO(data)) as image:
        image.convert('RGBA').save(root / 'preview.png')
      result['preview'] = 'preview.png'
      result['note'] = 'PNG is a viewing copy; the original GIF compression is preserved in bytes.hex.'
  else:
    result = byte_table(data, root)
  write_json(root / 'resource.json', dict(format=1, **result))
  return result['codec']


def encode(root, allow_edits=False):
  meta = read_json(safe_path(root, 'resource.json'))
  if meta.get('format') != 1:
    raise ValueError('Unsupported resource metadata version')
  codec = meta['codec']
  if codec == 'vga':
    return images.encode_vga(meta, root, allow_edits)
  if codec == 'bob':
    return images.encode_bob(meta, root)
  if codec == 'font':
    return images.encode_font(meta, root)
  if codec == 'palettes':
    return images.encode_palettes(meta, root)
  if codec == 'cmf':
    return audio.encode_cmf(meta, root)
  if codec == 'voc':
    return audio.encode_voc(meta, root)
  if codec == 'sound-bank':
    return encode_bank(meta, root, allow_edits)
  if codec == 'driver-catalog':
    return encode_cat(meta, root, allow_edits)
  if codec == 'character':
    header = bytes(meta['header'])
    if len(header) != 5:
      raise ValueError('Character header width changed')
    portrait = png_read(root, meta['portrait'])
    if len(portrait) != 52 * 59:
      raise ValueError('Character portrait size changed')
    return header + portrait + records.encode('Character', meta['character']) + bytes.fromhex(meta['tail_hex'])
  if codec == 'cell-map':
    return png_read(root, meta['image']) + bytes.fromhex(meta['tail_hex'])
  if codec == 'rectangles':
    return b''.join(records.encode('SpriteRect', row) for row in meta['rectangles'])
  if codec == 'restaurant-template':
    return encode_restaurant(meta)
  if codec == 'floor-patterns':
    return bytes((len(meta['patterns']),)) + b''.join(records.encode('FloorPattern', row)
        for row in meta['patterns']) + bytes.fromhex(meta['tail_hex'])
  if codec == 'staff-market':
    return bytes((len(meta['roles']),)) + bytes(meta['roles']) + bytes.fromhex(meta['tail_hex'])
  if codec == 'preferences':
    return records.encode('PTPreferencesFileImage', meta['preferences']) + bytes.fromhex(meta['tail_hex'])
  if codec == 'missions':
    return records.encode('PTMissionGoals', meta['columns']) + bytes.fromhex(meta['tail_hex'])
  if codec == 'recipe':
    payload = records.encode('PTSavedRecipe', meta['header'], extent=292)
    payload += b''.join(records.encode('RecipeEntry', row) for row in meta['entries'])
    return struct.pack('<I', len(payload)) + payload + bytes.fromhex(meta['tail_hex'])
  if codec == 'city':
    return encode_city(meta, root)
  if codec == 'high-scores':
    return b''.join(records.encode('HighScoreRecord', row) for row in meta['scores'])
  if codec == 'sound-config':
    return records.encode('PTSoundConfiguration', meta['configuration']) + bytes.fromhex(meta['tail_hex'])
  if codec == 'dos-text':
    with safe_path(root, meta['text']).open('r', encoding='utf-8', newline='') as source:
      return source.read().encode(meta['encoding'])
  if codec == 'byte-table':
    return b''.join(bytes(row) for row in read_related(root, meta['table'])['rows'])
  if codec == 'opaque-hex':
    payload = load_hex(root, meta['hex'])
    if meta.get('preview'):
      from PIL import Image
      import io
      reference = meta['preview']
      if isinstance(reference, str):
        with Image.open(safe_path(root, reference)) as image:
          reference = dict(file=reference, width=image.width, height=image.height, mode=image.mode)
      with Image.open(io.BytesIO(payload)) as original, png_image(root, reference) as preview:
        if original.size != preview.size or original.convert('RGBA').tobytes() != preview.convert('RGBA').tobytes():
          raise ValueError('GIF PNG is a viewing copy, not an editable source; original GIF bytes are in bytes.hex')
    return payload
  raise ValueError('Unknown resource codec: ' + str(codec))


def decode_bank(data, root, palette):
  count = struct.unpack('<I', bounded(data, 0, 4))[0]
  if count > 10000:
    raise ValueError('Unreasonable sound bank count')
  at, entries = 4 + 12 * count, []
  bounded(data, 0, at)
  for index in range(count):
    kind, offset, size = struct.unpack('<III', data[4 + index * 12:16 + index * 12])
    if offset < at:
      raise ValueError('Overlapping or out-of-order sound bank entries')
    payload = bounded(data, offset, size)
    name = f'entry-{index:03d}'
    if kind not in (1, 2):
      raise ValueError('Unidentified sound bank kind')
    codec = decode(payload, root / name, 'track.cmf' if kind == 1 else 'sample.voc', palette)
    entries.append(dict(kind=kind, directory=name, codec=codec, gap_hex=data[at:offset].hex()))
    at = offset + size
  return dict(codec='sound-bank', entries=entries, tail_hex=data[at:].hex())


def encode_bank(meta, root, allow_edits):
  at = 4 + 12 * len(meta['entries'])
  table, payloads = bytearray(), bytearray()
  for row in meta['entries']:
    gap = bytes.fromhex(row['gap_hex'])
    payload = encode(safe_path(root, row['directory']), allow_edits)
    at += len(gap)
    table.extend(struct.pack('<III', row['kind'], at, len(payload)))
    payloads.extend(gap + payload)
    at += len(payload)
  return struct.pack('<I', len(meta['entries'])) + table + payloads + bytes.fromhex(meta['tail_hex'])


def decode_cat(data, root, palette):
  first = struct.unpack('<I', bounded(data, 0, 4))[0]
  if first % 8 or first > len(data) or first > 80000:
    raise ValueError('Invalid driver catalog table')
  entries, at = [], first
  for index in range(first // 8):
    offset, size = struct.unpack('<II', data[index * 8:index * 8 + 8])
    if offset < at:
      raise ValueError('Overlapping driver catalog entries')
    name_length = bounded(data, offset, 1)[0]
    filename = bounded(data, offset + 1, name_length)
    payload = bounded(data, offset + 1 + name_length, size)
    directory = f'entry-{index:03d}'
    readable = filename.rstrip(b'\0').decode('cp437')
    codec = decode(payload, root / directory, readable or 'driver.com', palette)
    entries.append(dict(directory=directory, filename_cp437=filename.decode('cp437'), codec=codec,
        gap_hex=data[at:offset].hex()))
    at = offset + 1 + name_length + size
  return dict(codec='driver-catalog', entries=entries, tail_hex=data[at:].hex())


def encode_cat(meta, root, allow_edits):
  at = len(meta['entries']) * 8
  table, body = bytearray(), bytearray()
  for row in meta['entries']:
    gap = bytes.fromhex(row['gap_hex'])
    name = row['filename_cp437'].encode('cp437')
    payload = encode(safe_path(root, row['directory']), allow_edits)
    table.extend(struct.pack('<II', at + len(gap), len(payload)))
    body.extend(gap + bytes((len(name),)) + name + payload)
    at += len(gap) + 1 + len(name) + len(payload)
  return bytes(table + body) + bytes.fromhex(meta['tail_hex'])


def decode_city(data, root):
  at = records.SCHEMAS['PTSavedTown']['size']
  town = records.decode('PTSavedTown', bounded(data, 0, at))
  zones, suppliers = [], []
  for name, count, output in (('PTSavedZone', town['fields']['num_zones'], zones),
      ('PTSavedSupplier', town['fields']['num_supplier'], suppliers)):
    size = records.SCHEMAS[name]['size']
    for _ in range(count):
      output.append(records.decode(name, bounded(data, at, size)))
      at += size
  extra = byte_table(data[at:], root, 'Remaining town extra-data table; field meanings are not fully identified')
  return dict(codec='city', town=town, zones=zones, suppliers=suppliers, extra=extra['table'])


def encode_city(meta, root):
  if len(meta['zones']) != meta['town']['fields']['num_zones'] or \
      len(meta['suppliers']) != meta['town']['fields']['num_supplier']:
    raise ValueError('Town zone/supplier counts differ from their records')
  output = records.encode('PTSavedTown', meta['town'])
  output += b''.join(records.encode('PTSavedZone', row) for row in meta['zones'])
  output += b''.join(records.encode('PTSavedSupplier', row) for row in meta['suppliers'])
  output += b''.join(bytes(row) for row in read_related(root, meta['extra'])['rows'])
  return output


def decode_restaurant(data):
  names = ['size_x', 'size_y', 'kitchen_width', 'kitchen_height', 'door_x_start', 'left_door_y_end', 'byte1A']
  room = dict(zip(names, bounded(data, 0, 7)))
  floor = records.decode('RestaurantFloorState', bounded(data, 7, 32))
  furniture_count, oven_count = bounded(data, 39, 2)
  furniture, ovens, at = [], [], 41
  for count, output in ((furniture_count, furniture), (oven_count, ovens)):
    for _ in range(count):
      output.append(records.decode('Furniture', bounded(data, at, 9)))
      at += 9
  return dict(codec='restaurant-template', room=room, floor=floor, furniture=furniture, ovens=ovens,
      tail_hex=data[at:].hex())


def encode_restaurant(meta):
  names = ['size_x', 'size_y', 'kitchen_width', 'kitchen_height', 'door_x_start', 'left_door_y_end', 'byte1A']
  output = bytes(meta['room'][name] for name in names) + records.encode('RestaurantFloorState', meta['floor'])
  output += bytes((len(meta['furniture']), len(meta['ovens'])))
  output += b''.join(records.encode('Furniture', row) for row in meta['furniture'])
  output += b''.join(records.encode('Furniture', row) for row in meta['ovens'])
  return output + bytes.fromhex(meta['tail_hex'])
