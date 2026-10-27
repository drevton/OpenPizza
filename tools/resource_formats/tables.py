"""Typed TSV values without JSON cells or multiline/control-character surprises."""
import csv
import math
import re


def escape(value):
  return ''.join('\\\\' if char == '\\' else '\\t' if char == '\t' else '\\r' if char == '\r'
      else '\\n' if char == '\n' else f'\\x{ord(char):02x}' if ord(char) < 32 or ord(char) == 127
      else char for char in value)


def unescape(value):
  def replace(match):
    token = match.group()[1:]
    if token in ('\\', 't', 'r', 'n'):
      return {'\\': '\\', 't': '\t', 'r': '\r', 'n': '\n'}[token]
    if re.fullmatch(r'x[0-9a-fA-F]{2}', token):
      return chr(int(token[1:], 16))
    raise ValueError('Invalid TSV escape: ' + match.group())
  return re.sub(r'\\(?:x..|.|$)', replace, value)


def write_rows(path, fields, rows):
  path.parent.mkdir(parents=True, exist_ok=True)
  with path.open('w', encoding='utf-8', newline='') as output:
    writer = csv.writer(output, delimiter='\t', lineterminator='\n')
    writer.writerow(fields)
    for row in rows:
      writer.writerow([escape(str(row[field])) for field in fields])


def read_rows(path, fields):
  # Long numeric arrays remain one compact cell, rather than thousands of rows.
  csv.field_size_limit(16 * 1024 * 1024)
  with path.open(encoding='utf-8', newline='') as source:
    reader = csv.reader(source, delimiter='\t', strict=True)
    if next(reader, None) != list(fields):
      raise ValueError('Unexpected TSV columns: ' + str(path))
    for line, row in enumerate(reader, 2):
      if len(row) != len(fields):
        raise ValueError(f'{path}:{line}: incorrect TSV column count')
      yield dict(zip(fields, (unescape(value) for value in row)))


def token(key):
  return str(key).replace('~', '~0').replace('/', '~1')


def value_rows(document, value, field=''):
  if isinstance(value, dict):
    kind, text = 'object', ''
  elif isinstance(value, list):
    if value and all(type(item) is int for item in value):
      kind, text = 'integers', ' '.join(map(str, value))
    else:
      kind, text = 'array', str(len(value))
  elif isinstance(value, str):
    kind, text = 'text', value
  elif value is None:
    kind, text = 'null', ''
  elif isinstance(value, bool):
    kind, text = 'boolean', str(value).lower()
  elif isinstance(value, int):
    kind, text = 'integer', str(value)
  elif isinstance(value, float) and math.isfinite(value):
    kind, text = 'float', repr(value)
  else:
    raise ValueError('Unsupported TSV value type')
  yield dict(document=document, field=field, type=kind, value=text)
  if kind == 'object':
    for key, item in value.items():
      yield from value_rows(document, item, field + '/' + token(key))
  elif kind == 'array':
    for index, item in enumerate(value):
      yield from value_rows(document, item, field + '/' + str(index))


def write_documents(path, documents):
  write_rows(path, ('document', 'field', 'type', 'value'),
      (row for name, value in documents.items() for row in value_rows(name, value)))


def read_documents(path):
  return documents_from_rows(read_rows(path, ('document', 'field', 'type', 'value')))


def documents_from_rows(rows):
  result, seen, missing = {}, set(), object()

  def index(part, parent):
    value = int(part)
    if str(value) != part or not 0 <= value < len(parent):
      raise ValueError('TSV array index is outside its declared size')
    return value

  for row in rows:
    name, field, kind, text = (row[key] for key in ('document', 'field', 'type', 'value'))
    if (name, field) in seen:
      raise ValueError('Duplicate TSV field: ' + name + field)
    seen.add((name, field))
    if kind == 'object' and not text:
      value = {}
    elif kind == 'array':
      length = int(text)
      if not 0 <= length <= 1000000:
        raise ValueError('Invalid TSV array length')
      value = [missing] * length
    elif kind == 'integers':
      value = [int(item) for item in text.split()]
    elif kind == 'text':
      value = text
    elif kind == 'null' and not text:
      value = None
    elif kind == 'boolean' and text in ('true', 'false'):
      value = text == 'true'
    elif kind == 'integer':
      value = int(text)
    elif kind == 'float' and math.isfinite(float(text)):
      value = float(text)
    else:
      raise ValueError('Invalid TSV value type: ' + kind)
    if not field:
      result[name] = value
      continue
    if not field.startswith('/') or name not in result:
      raise ValueError('TSV field lacks its parent document')
    parts = [part.replace('~1', '/').replace('~0', '~') for part in field[1:].split('/')]
    parent = result[name]
    for part in parts[:-1]:
      parent = parent[index(part, parent)] if isinstance(parent, list) else parent[part]
    key = index(parts[-1], parent) if isinstance(parent, list) else parts[-1]
    if not isinstance(parent, (dict, list)):
      raise ValueError('TSV field parent is not a container')
    parent[key] = value

  def complete(value):
    if value is missing:
      raise ValueError('TSV array has missing elements')
    if isinstance(value, (list, dict)):
      for item in value.values() if isinstance(value, dict) else value:
        complete(item)
  complete(result)
  return result
