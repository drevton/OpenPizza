"""Shared lossless artifact I/O and strict bounds checks."""
import hashlib
import json
from pathlib import Path, PurePosixPath
import struct
from PIL import Image

MAX_PIXELS = 16 * 1024 * 1024


def sha256(data):
  return hashlib.sha256(data).hexdigest()


def bounded(data, offset, size):
  if offset < 0 or size < 0 or offset > len(data) or size > len(data) - offset:
    raise ValueError(f'Truncated resource at file offset {offset}, requesting {size} bytes')
  return data[offset:offset + size]


def safe_path(root, name):
  if not isinstance(name, str) or '\\' in name or '\0' in name:
    raise ValueError('Invalid artifact path')
  path = PurePosixPath(name)
  if not name or path.is_absolute() or any(part in ('.', '..') for part in path.parts) or str(path) != name:
    raise ValueError('Unsafe artifact path: ' + name)
  from .flat import current
  package = current()
  if package is not None and root.is_relative_to(package.root) and path.parts[0] in ('graphics', 'audio', 'data'):
    root = package.root
  result = root / path
  current = result
  while current != root:
    if current.is_symlink():
      raise ValueError('Symlink artifact path: ' + name)
    current = current.parent
  if root.is_symlink():
    raise ValueError('Symlink artifact root')
  return result


def pretty_json(value, indent=0):
  simple = json.dumps(value, ensure_ascii=False, allow_nan=False)
  if len(simple) + indent <= 120 or not isinstance(value, (dict, list)):
    return simple
  pad = ' ' * (indent + 2)
  if isinstance(value, dict):
    rows = [pad + json.dumps(key, ensure_ascii=False) + ': ' + pretty_json(item, indent + 2)
        for key, item in value.items()]
    return '{\n' + ',\n'.join(rows) + '\n' + ' ' * indent + '}'
  if all(not isinstance(item, (dict, list)) for item in value):
    rows, row = [], ''
    for item in value:
      token = json.dumps(item, ensure_ascii=False, allow_nan=False)
      if row and len(row) + len(token) + indent + 4 > 120:
        rows.append(pad + row)
        row = ''
      row = row + ', ' + token if row else token
    if row:
      rows.append(pad + row)
  else:
    rows = [pad + pretty_json(item, indent + 2) for item in value]
  return '[\n' + ',\n'.join(rows) + '\n' + ' ' * indent + ']'


def write_json(path, value):
  path.parent.mkdir(parents=True, exist_ok=True)
  path.write_text(pretty_json(value) + '\n', encoding='utf-8')


def read_json(path):
  from .flat import current
  package = current()
  if package is not None and path.is_relative_to(package.root):
    return package.document(path)
  return json.loads(path.read_text(encoding='utf-8'))


def read_related(root, name):
  from .flat import current
  package = current()
  if package is not None:
    return package.related(name)
  return read_json(safe_path(root, name))


def read_events(root, name):
  from .flat import current
  if current() is not None:
    return read_related(root, name)
  return [json.loads(line) for line in safe_path(root, name).read_text(encoding='utf-8').splitlines() if line.strip()]


def hex_text(data):
  rows = []
  for offset in range(0, len(data), 16):
    chunk = data[offset:offset + 16]
    display = ''.join(chr(byte) if 32 <= byte < 127 else '.' for byte in chunk)
    rows.append(f'{offset:08x}  {chunk.hex(" "):<47}  |{display}|')
  return '\n'.join(rows) + ('\n' if rows else '')


def read_hex(text):
  result = bytearray()
  for line in text.splitlines():
    if not line.strip():
      continue
    offset, rest = line.split('  ', 1)
    if int(offset, 16) != len(result):
      raise ValueError('Noncontiguous hex dump')
    payload = rest.split('  |', 1)[0].strip()
    chunk = bytes.fromhex(payload)
    if not 1 <= len(chunk) <= 16:
      raise ValueError('Invalid hex row width')
    result.extend(chunk)
  return bytes(result)


def save_hex(root, name, data):
  safe_path(root, name).write_text(hex_text(data), encoding='ascii')
  return name


def load_hex(root, name):
  return read_hex(safe_path(root, name).read_text(encoding='ascii'))


def png_write(root, name, width, height, pixels, palette=None, mode='P'):
  if width <= 0 or height <= 0 or width * height > MAX_PIXELS or len(pixels) != width * height:
    raise ValueError('Invalid image dimensions or pixel count')
  path = safe_path(root, name)
  path.parent.mkdir(parents=True, exist_ok=True)
  image = Image.frombytes(mode, (width, height), pixels)
  if mode == 'P':
    image.putpalette(palette or bytes(value for index in range(256) for value in (index, index, index)))
  image.save(path, format='PNG', optimize=False, bits=8)
  return dict(file=name, width=width, height=height, mode=mode)


def png_image(root, reference):
  if 'sprite' in reference:
    from .sheets import read_sprite
    return read_sprite(reference)
  path = safe_path(root, reference['file'])
  with Image.open(path) as image:
    if image.format != 'PNG' or image.size != (reference['width'], reference['height']):
      raise ValueError('PNG dimensions changed: ' + reference['file'])
    if image.width * image.height > MAX_PIXELS:
      raise ValueError('Image is too large')
    if image.mode != reference['mode']:
      raise ValueError('PNG must retain ' + reference['mode'] + ' mode: ' + reference['file'])
    return image.copy()


def png_read(root, reference):
  with png_image(root, reference) as image:
    return image.tobytes()


def unpack_fields(data, fields):
  result, offset = {}, 0
  for name, fmt in fields:
    size = struct.calcsize('<' + fmt)
    values = struct.unpack('<' + fmt, bounded(data, offset, size))
    result[name] = values[0] if len(values) == 1 else list(values)
    offset += size
  return result


def pack_fields(value, fields):
  result = bytearray()
  for name, fmt in fields:
    items = value[name] if isinstance(value[name], list) else [value[name]]
    result.extend(struct.pack('<' + fmt, *items))
  return bytes(result)
