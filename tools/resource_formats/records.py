"""Named wire records with explicit preservation of unmodeled padding."""
import json
import math
from pathlib import Path
import struct

SCHEMAS = json.loads(Path(__file__).with_name('record_layouts.json').read_text())['types']


def resolve(ctype):
  schema = SCHEMAS[ctype]
  while schema['kind'] == 'alias':
    schema = SCHEMAS[schema['target']]
  return schema


def decode(ctype, data, field_name='', extent=None):
  schema = resolve(ctype)
  size = schema['size'] if extent is None else extent
  if len(data) != size:
    raise ValueError('Record width differs: ' + ctype)
  if schema['kind'] == 'scalar':
    value, = struct.unpack('<' + schema['format'], data)
    if isinstance(value, float) and not math.isfinite(value):
      return dict(ieee754_le=data.hex(), description=str(value))
    return value
  if schema['kind'] == 'array':
    child = resolve(schema['element'])
    if child.get('format') in ('b', 'B') and any(word in field_name.lower() for word in ('name', 'text', 'label')):
      return dict(cp437=data.decode('cp437'))
    width = child['size']
    return [decode(schema['element'], data[at:at + width], field_name) for at in range(0, size, width)]
  if schema['kind'] != 'record':
    raise ValueError('Unsupported record kind')
  fields, covered = {}, bytearray(size)
  for field in schema['fields']:
    start, width = field['offset'], SCHEMAS[field['type']]['size']
    if start >= size:
      continue
    if width > size - start:
      raise ValueError('Record extent splits a field: ' + field['name'])
    fields[field['name']] = decode(field['type'], data[start:start + width], field['name'])
    covered[start:start + width] = b'\x01' * width
  padding, at = [], 0
  while at < size:
    if covered[at]:
      at += 1
      continue
    end = at + 1
    while end < size and not covered[end]:
      end += 1
    padding.append(dict(offset=at, hex=data[at:end].hex()))
    at = end
  return dict(type=ctype, size=size, fields=fields, padding=padding)


def encode(ctype, value, extent=None):
  schema = resolve(ctype)
  if schema['kind'] == 'scalar':
    if isinstance(value, dict):
      result = bytes.fromhex(value['ieee754_le'])
      if len(result) != schema['size'] or schema['format'] not in ('f', 'd'):
        raise ValueError('Invalid IEEE-754 bit view')
      return result
    return struct.pack('<' + schema['format'], value)
  if schema['kind'] == 'array':
    if isinstance(value, dict):
      result = value['cp437'].encode('cp437')
    else:
      result = b''.join(encode(schema['element'], item) for item in value)
    if len(result) != schema['size']:
      raise ValueError('Array/text width changed; retain its exact fixed-width padding')
    return result
  size = value['size']
  expected = schema['size'] if extent is None else extent
  if value['type'] != ctype or size != expected or not 0 <= size <= schema['size']:
    raise ValueError('Invalid record extent/type')
  result, covered = bytearray(size), bytearray(size)
  for field in schema['fields']:
    start, width = field['offset'], SCHEMAS[field['type']]['size']
    if start >= size:
      continue
    payload = encode(field['type'], value['fields'][field['name']])
    if len(payload) != width or start + width > size:
      raise ValueError('Record field width changed')
    result[start:start + width] = payload
    covered[start:start + width] = b'\x01' * width
  for padding in value['padding']:
    start = padding['offset']
    payload = bytes.fromhex(padding['hex'])
    if start < 0 or start + len(payload) > size or any(covered[start:start + len(payload)]):
      raise ValueError('Padding overlaps a decoded field')
    result[start:start + len(payload)] = payload
    covered[start:start + len(payload)] = b'\x01' * len(payload)
  if not all(covered):
    raise ValueError('Record bytes are missing')
  return bytes(result)
