"""Sprite relationships from native renderer formulas and verified original catalog records."""
from collections import Counter
import re
import struct
from .common import bounded, sha256
from .palettes import EXE_HASH

CATALOGS = {
    'furniture': ([(425429, 1579), (427008, 4096), (431104, 1541)],
      '6ec0080432bd2c2353f68ac3fed3e1b4be78ca4e16ea12e2ec2f3f56586f3c70'),
    'ingredients': ([(437965, 1331), (439296, 4096), (443392, 45)],
      '0f042320a385de5b8714e14e7bbdf2b9adfc5cb4116ff0025426732e7b84a1a4')}


def catalog_labels(installation):
  executable = next((p for p in installation.iterdir() if p.is_file() and
      p.name.lower() in ('pt.exe', 'pt_original.exe') and sha256(p.read_bytes()) == EXE_HASH), None)
  if executable is None:
    return {}
  data, catalogs = executable.read_bytes(), {}
  for name, (slices, checksum) in CATALOGS.items():
    raw = b''.join(bounded(data, offset, size) for offset, size in slices)
    if sha256(raw) != checksum:
      raise ValueError('Original sprite catalog differs: ' + name)
    catalogs[name] = raw
  return furniture_labels(catalogs['furniture']) | ingredient_labels(catalogs['ingredients'])


def furniture_labels(data):
  result, counts, shared = {}, Counter(), {}
  for index in range(len(data) // 44):
    record = data[index * 44:(index + 1) * 44]
    name = record[3:22].split(b'\0')[0].decode('cp437').strip()
    if not name:
      continue
    category = record[33]
    kind = {0: 'ashes', 1: 'table', 2: 'chair', 5: 'arcade', 6: 'vending machine',
        8: 'oven', 9: 'ashes'}.get(category, 'decoration')
    if category in (3, 4, 6):
      lowered = name.lower()
      for word in ('fountain', 'wall', 'table plant', 'floor plant', 'candle', 'jukebox', 'tv', 'telephone'):
        if word in lowered:
          kind = word
          break
    source = 'BOB/KUEBOBS.BOB' if category == 8 else 'BOB/MOEBEL.BOB'
    images = struct.unpack_from('<4h', record, 24)
    identity = (source, tuple(dict.fromkeys(images)))
    if identity in shared:
      for image in dict.fromkeys(images):
        key = (source, image)
        result[key]['description'] += '; ' + name
      continue
    number = counts[kind]
    counts[kind] += 1
    group = f'{kind} {number}'
    shared[identity] = group
    for orientation, image in enumerate(images):
      result.setdefault((source, image), dict(group=group, label=f'{group} rotation {orientation}',
          description=name, row=0, column=orientation, rank=index))
  result[('BOB/MOEBEL.BOB', 340)] = dict(group='guest plate', label='guest plate',
      description='Food layer used by restaurant.c draw_guests(part=2)', row=0, column=0, rank=999)
  return result


def ingredient_labels(data):
  result = {}
  for index in range(len(data) // 57):
    record = data[index * 57:(index + 1) * 57]
    name = record[29:42].split(b'\0')[0].decode('cp437').strip()
    if not name:
      continue
    # recipe_editor.c and recipe_info.c index sprite_images by preparation*4+rotation.
    for variation, image in enumerate(struct.unpack_from('<12h', record)):
      if image >= 0:
        result.setdefault(('BOB/ZUTATEN.BOB', image), dict(group=name.lower(),
            label=f'{name.lower()} preparation {variation // 4} rotation {variation % 4}',
            description=name, row=variation // 4, column=variation % 4, rank=index))
  return result


def describe(source, filename, codec, catalog):
  source = source.upper()
  match = re.fullmatch(r'frame-(\d+)\.png', filename)
  index = int(match[1]) if match else 0
  if (source, index) in catalog and match:
    return dict(catalog[(source, index)])
  stem = source.rsplit('/', 1)[-1].rsplit('.', 1)[0].lower()
  if source == 'BOB/GAESTE.BOB':
    # restaurant.c uses bands 48/96 for upper/lower layers; restaurant_scroll.c
    # uses band 0 for body and 144+48*pose for animated overlays, all with four directions.
    band, guest, direction = index // 48, index % 48 // 4, index % 4
    part = ('body', 'upper', 'lower', 'pose 0', 'pose 1', 'pose 2', 'pose 3')[band]
    return dict(group=f'guest {guest}', label=f'guest {guest} {part} direction {direction}',
        description=part, row=band, column=direction, rank=guest)
  weapon = re.fullmatch(r'nightw(\d+)', stem)
  if weapon:
    number = int(weapon[1])
    name = {0: 'knife', 1: 'spray', 2: 'gun', 3: 'bomb'}.get(number, 'weapon')
    if number == 3:
      group, row, column = ('bomb flight' if index < 2 else 'bomb explosion'), 0, index
    elif index < 4:
      group, row, column = name + ' held', 0, index
    elif number in (0, 1) and index < 32:
      group, row, column = name + ' attack', index % 4, index // 4 - 1
    elif number == 2 and 8 <= index < 16:
      group, row, column = 'gun projectile', index % 4, (index - 8) // 4
    elif number == 2 and 16 <= index < 20:
      group, row, column = 'gun impact', 0, index - 16
    else:
      group, row, column = name + ' effect', 0, index
    return dict(group=group, label=f'{group} frame {index}', description='interactive_attack.c weapon renderer',
        row=row, column=column, rank=min(index, 32))
  if stem == 'nightp':
    if index < 24:
      actor, direction, phase = index // 12, index % 12 // 3, index % 3
      group, row, column = f'attacker {actor} walk', direction, phase
    elif index < 32:
      group, row, column = f'attacker {(index - 24) // 4} head', 0, index % 4
    else:
      group, row, column = f'attacker {(index - 32) // 7} fall', 0, (index - 32) % 7
    return dict(group=group, label=f'{group} frame {index}', description='interactive_attack.c actor renderer',
        row=row, column=column, rank=index)
  face = re.fullmatch(r'typ(\d+)e?', stem)
  if face:
    return dict(group=f'face {int(face[1])}', label=f'face {int(face[1])} expression {index}',
        description='Phone face expressions', row=0, column=index, rank=index)
  if codec == 'font':
    glyph = re.fullmatch(r'glyph-(\d+)\.png', filename)
    label = f'{int(glyph[1]):02X}' if glyph else 'spare'
  elif codec == 'character':
    label = 'portrait ' + stem
  elif codec == 'vga':
    label = re.sub(r'back(\d+)$', r'background \1', stem).replace('_', ' ')
  elif codec == 'palettes':
    label = 'palette ' + filename.removesuffix('.png').removeprefix('palette-').lstrip('0')
    if label == 'palette ':
      label += '0'
  else:
    role = {'moebel': 'furniture', 'gaeste': 'guest', 'kuebobs': 'oven', 'zutaten': 'ingredient',
        'menubobs': 'menu icon', 'stanbobs': 'interface icon', 'stabo': 'statistics icon',
        'lebbo': 'production icon'}.get(stem, 'sprite')
    label = f'{role} {index}'
  return dict(group=label, label=label, description='Unidentified frame' if codec == 'bob' else label,
      row=0, column=0, rank=index)


def frame_ranges(numbers):
  result, start, end = [], None, None
  for number in numbers:
    if start is None:
      start = end = number
    elif number == end + 1:
      end = number
    else:
      result.append(str(start) if start == end else f'{start}-{end}')
      start = end = number
  if start is not None:
    result.append(str(start) if start == end else f'{start}-{end}')
  return ','.join(result)


def parse_frames(text):
  result = []
  for part in text.split(','):
    match = re.fullmatch(r'(\d+)(?:-(\d+))?', part.strip())
    if not match:
      raise ValueError('Invalid frame range: ' + part)
    first, last = int(match[1]), int(match[2] or match[1])
    if not 0 <= first <= last < 10000:
      raise ValueError('Invalid frame range bounds')
    result.extend(range(first, last + 1))
  if len(result) != len(set(result)):
    raise ValueError('Duplicate frame in label range')
  return result


def legend_rows(sprites):
  groups = {}
  for identifier, info in sprites.items():
    source, separator, frame = identifier.rpartition('#')
    if not separator or not source.lower().endswith('.bob') or not frame.isdigit() or not info.get('group'):
      continue
    group = groups.setdefault((source, info['group']), dict(members=[], descriptions=[]))
    group['members'].append((info.get('row', 0), info.get('column', 0), int(frame)))
    description = info.get('description', '').strip()
    if description and description.casefold() != info['group'].casefold() and description not in (
        'Unidentified frame', 'body', 'upper', 'lower', 'pose 0', 'pose 1', 'pose 2', 'pose 3',
        'Phone face expressions') and '.c ' not in description:
      if description not in group['descriptions']:
        group['descriptions'].append(description)
  rows = []
  for (source, name), group in groups.items():
    numbers = [item[2] for item in sorted(group['members'])]
    rows.append(dict(resource=source, name=name, description='; '.join(group['descriptions']),
        frames=frame_ranges(numbers)))
  return sorted(rows, key=lambda row: (row['resource'], parse_frames(row['frames'])[0]))
