"""Declare each TSV column's type once, rather than tagging every value."""
from . import tables

TYPES = {'o': 'object', 'a': 'array', 'u': 'ints', 's': 'text', 'n': 'null',
    'b': 'bool', 'i': 'int', 'f': 'float', 'r': 'reference'}


def write_rows(path, prefix, fields, rows):
  rows = list(rows)
  kinds = {}
  for field in fields:
    present = {row[field][0] for row in rows if row.get(field)}
    kinds[field] = next(iter(present)) if len(present) == 1 else None
  names = {field: field + ':' + TYPES[kinds[field]] if kinds[field] else field + ':value' for field in fields}
  result = []
  for row in rows:
    output = {field: row[field] for field in prefix}
    for field in fields:
      cell = row.get(field, '')
      if cell and kinds[field]:
        cell = cell[2:]
        # Blank is absent. A leading colon escapes empty values and literal colons.
        if not cell or cell.startswith(':'):
          cell = ':' + cell
      output[names[field]] = cell
    result.append(output)
  tables.write_rows(path, list(prefix) + list(names.values()), result)


def read_rows(path, prefix, headers):
  reverse = {name: kind for kind, name in TYPES.items()}
  columns, seen = {}, set(prefix)
  for header in headers[len(prefix):]:
    field, separator, kind = header.rpartition(':')
    if not separator or kind not in (*reverse, 'value') or field in seen:
      raise ValueError('Invalid typed TSV column: ' + header)
    seen.add(field)
    columns[header] = (field, reverse.get(kind))
  for row in tables.read_rows(path, headers):
    output = {field: row[field] for field in prefix}
    for header, (field, kind) in columns.items():
      cell = row[header]
      if cell and kind:
        if cell.startswith(':'):
          cell = cell[1:]
          if cell and not cell.startswith(':'):
            raise ValueError('Invalid leading-colon escape in typed TSV cell')
        cell = kind + ':' + cell
      output[field] = cell
    yield output
