"""Purpose-specific, wide TSV records with shared column names and explicit raw-byte files."""
import csv
from collections import defaultdict
from pathlib import Path
import re
from .common import safe_path, save_hex, load_hex
from . import tables
from . import models
from . import columns

INDEX_FIELDS = ('file', 'kind', 'document', 'description')
PREFIX = {'object': 'o', 'array': 'a', 'integers': 'u', 'text': 's', 'null': 'n',
    'boolean': 'b', 'integer': 'i', 'float': 'f'}
PURPOSES = {'vga': 'graphics/images', 'bob': 'graphics/sprite-sets', 'font': 'graphics/fonts',
    'cell-map': 'graphics/maps', 'palettes': 'graphics/palettes', 'rectangles': 'graphics/rectangles',
    'character': 'graphics/portraits', 'recipe': 'data/recipes', 'city': 'data/towns',
    'restaurant-template': 'data/restaurants', 'floor-patterns': 'data/floor-patterns',
    'cmf': 'audio/music', 'voc': 'audio/voices', 'sound-bank': 'audio/banks'}
COLLECTIONS = {'high-scores': ('scores', {}), 'floor-patterns': ('patterns', {'tail_hex': ''}),
    'palettes': ('banks', {'rgb_scale': 4}), 'sound-bank': ('entries', {'tail_hex': ''}),
    'driver-catalog': ('entries', {'tail_hex': ''})}
RECTANGLE_FIELDS = ('resource', 'index', 'left', 'top', 'right', 'bottom')


def column(path):
  if not path:
    return '$'
  name = path.lstrip('/')
  return path if name in ('resource', 'index', 'record', 'parent', '$') or not name else name


def value_from_cells(identifier, row, infer_arrays=False):
  """Objects with fields are implicit; retain declarations only for actual empty objects/arrays."""
  reverse = {prefix: kind for kind, prefix in PREFIX.items()}
  values, refs, declared = {}, {}, set()
  for field, cell in row.items():
    if not cell:
      continue
    if len(cell) < 2 or cell[1] != ':' or cell[0] not in (*reverse, 'r'):
      raise ValueError('Invalid typed record cell')
    field = '' if field == '$' else field if field.startswith('/') else '/' + field
    if cell[0] == 'r':
      refs[field] = cell[2:]
      kind, text = 'null', ''
    else:
      kind, text = reverse[cell[0]], cell[2:]
    values[field] = dict(document=identifier, field=field, type=kind, value=text)
    declared.add(field)
  for field in list(declared):
    parts = field.split('/')
    for count in range(len(parts)):
      parent = '/'.join(parts[:count])
      if parent not in values:
        values[parent] = None
  # Numeric child paths imply arrays. Explicit object markers retain dictionaries with numeric keys.
  for field, value in list(values.items()):
    if value is not None:
      continue
    prefix = field + '/'
    children = [path[len(prefix):] for path in values if path.startswith(prefix) and '/' not in path[len(prefix):]]
    is_array = infer_arrays and children and all(part.isdecimal() and str(int(part)) == part for part in children)
    length = max(map(int, children)) + 1 if is_array else 0
    values[field] = dict(document=identifier, field=field, type='array' if is_array else 'object',
        value=str(length) if is_array else '')
  ordered = sorted(values.values(), key=lambda row: (row['field'].count('/'), row['field']))
  if not ordered:
    ordered = [dict(document=identifier, field='', type='object', value='')]
  return tables.documents_from_rows(ordered)[identifier], refs


def purpose(name, value):
  if name == '@catalog':
    return 'package'
  if name == '@graphics':
    return 'graphics/layout'
  if name.endswith('#instruments'):
    return 'audio/instruments'
  codec = value.get('codec') if isinstance(value, dict) else None
  return PURPOSES.get(codec, 'data/' + (codec or 'records'))


def branch(path):
  parts = [part for part in path.split('/') if part and part != 'fields' and not part.isdigit()]
  return '-'.join(re.sub(r'[^a-zA-Z0-9-]+', '-', part).strip('-').lower() for part in parts) or 'rows'


def write_package(root, documents):
  groups, manifest, seen, codecs, collections, empty = defaultdict(list), [], set(), {}, {}, defaultdict(list)
  rectangles = []
  byte_owners, collection_allowed = {}, {}
  normalized = {name: models.simplify_document(value) for name, value in documents.items() if name != '@graphics'}
  for name, simple in normalized.items():
    if isinstance(simple, dict) and simple.get('codec') == 'byte-table' and \
        set(simple) == {'codec', 'table'} and simple['table'] == name + '#bytes':
      byte_owners[simple['table']] = name
    codec = simple.get('codec') if isinstance(simple, dict) else None
    field, defaults = COLLECTIONS.get(codec, (None, {}))
    eligible = isinstance(simple, list) or field is not None and \
        set(simple) <= {'codec', field, *defaults} and \
        all(simple.get(key, item) == item for key, item in defaults.items())
    group = purpose(name, simple)
    collection_allowed[group] = collection_allowed.get(group, True) and eligible

  def record(identifier, value, group, parent='', ordinal='', owner_path=None):
    if identifier in seen:
      raise ValueError('Duplicate compact record ID: ' + identifier)
    seen.add(identifier)
    fields = {}

    def visit(item, path):
      if isinstance(item, (list, tuple)) and not isinstance(item, models.InlineArray) and item and \
          all(isinstance(child, (dict, list)) for child in item):
        owner = identifier + ':' + path
        child_group = group + '-' + branch(path)
        for index, child in enumerate(item):
          record(owner + ':' + str(index), child, child_group, owner, str(index), path)
        return
      integers = isinstance(item, list) and item and all(type(child) is int for child in item)
      numeric_object = isinstance(item, dict) and item and all(key.isdecimal() for key in item)
      if not isinstance(item, (dict, list)) or not item or integers or numeric_object:
        row = next(tables.value_rows('', item, path))
        fields[path] = PREFIX[row['type']] + ':' + row['value']
      if isinstance(item, dict):
        for key, child in item.items():
          visit(child, path + '/' + tables.token(key))
      elif isinstance(item, list) and not (item and all(type(child) is int for child in item)):
        for index, child in enumerate(item):
          visit(child, path + '/' + str(index))
    visit(value, '')
    groups[group].append(dict(record=identifier, parent=parent, index=ordinal, fields=fields, owner_path=owner_path))

  for name, value in normalized.items():
    if name == '@catalog':
      if value['format'] != 1:
        raise ValueError('Unsupported package format')
      manifest.append(dict(file='', kind='layout', document=value['layout'], description=''))
      continue
    codec = value.get('codec') if isinstance(value, dict) else None
    if name in byte_owners.values():
      continue
    if codec == 'rectangles':
      for index, rectangle in enumerate(value['rectangles']):
        rectangles.append(dict(resource=name, index=index, **rectangle))
      if not value['rectangles']:
        rectangles.append(dict(resource=name, index='', left='', top='', right='', bottom=''))
      continue
    group = purpose(name, value)
    field, defaults = COLLECTIONS.get(codec, (None, {}))
    collection = collection_allowed[group]
    if collection:
      field = field or ''
      children = value if isinstance(value, list) else value[field]
      collections[group] = (codec or '', field)
      owner = name + ':' + ('/' + field if field else '')
      for index, child in enumerate(children):
        record(owner + ':' + str(index), child, group, owner, str(index), '/' + field if field else '')
      if not children:
        groups[group]
        empty[group].append(name)
      continue
    if isinstance(value, dict) and set(value) <= {'rows', 'description'} and 'rows' in value:
      data = b''.join(bytes(row) for row in value['rows'])
      filename = 'data/raw-' + re.sub(r'[^a-zA-Z0-9._-]', '_', name) + '.hex'
      (root / 'data').mkdir(exist_ok=True)
      save_hex(root, filename, data)
      manifest.append(dict(file=filename, kind='resource-bytes' if name in byte_owners else 'bytes',
          document=byte_owners.get(name, name), description=value.get('description', '')))
    else:
      if codec:
        codecs[group] = codec
        value = {key: item for key, item in value.items() if key != 'codec'}
      record(name, value, group)
  if rectangles:
    tables.write_rows(root / 'graphics/rectangles.tsv', RECTANGLE_FIELDS, rectangles)
    manifest.append(dict(file='graphics/rectangles.tsv', kind='rectangles', document='', description=''))
  for group, records in sorted(groups.items()):
    paths = sorted({path for row in records for path in row['fields']}, key=lambda path: (path.count('/'), path))
    collection = group in collections
    child_paths = {row['owner_path'] for row in records}
    children = not collection and records and all(row['parent'] for row in records) and len(child_paths) == 1
    roots = not collection and not children and all(not row['parent'] for row in records)
    prefix = ['resource', 'index'] if collection or children else ['resource'] if roots \
        else ['record', 'parent', 'index']
    fields = [column(path) for path in paths]
    filename = group + '.tsv'
    rows = []
    for row in records:
      values = {column(path): row['fields'].get(path, '') for path in paths}
      if collection:
        codec, field = collections[group]
        suffix = ':' + ('/' + field if field else '')
        rows.append(dict(resource=row['parent'].removesuffix(suffix), index=row['index'], **values))
      elif children:
        suffix = ':' + row['owner_path']
        rows.append(dict(resource=row['parent'].removesuffix(suffix), index=row['index'], **values))
      elif roots:
        rows.append(dict(resource=row['record'], **values))
      else:
        rows.append(dict(record=row['record'], parent=row['parent'], index=row['index'], **values))
    if collection:
      rows.extend(dict(resource=name, index='', **{column(path): '' for path in paths})
          for name in empty[group])
    columns.write_rows(root / filename, prefix, fields, rows)
    kind = 'collection:' + ':'.join(collections[group]) if collection else \
        'children:' + next(iter(child_paths)) if children else \
        'codec:' + codecs[group] if group in codecs else 'objects' if roots else 'records'
    manifest.append(dict(file=filename, kind='columns:' + kind, document='', description=''))
  tables.write_rows(root / 'tables.tsv', INDEX_FIELDS, manifest)
  return manifest


def read_package(root):
  records, owners, documents, references, automatic = {}, defaultdict(dict), {}, {}, defaultdict(dict)
  reverse = {prefix: kind for kind, prefix in PREFIX.items()}
  for entry in tables.read_rows(safe_path(root, 'tables.tsv'), INDEX_FIELDS):
    if entry['kind'] == 'layout':
      if entry['file'] or '@catalog' in documents:
        raise ValueError('Invalid or duplicate package layout declaration')
      documents['@catalog'] = dict(format=1, layout=entry['document'])
      continue
    path = safe_path(root, entry['file'])
    if entry['kind'] == 'package':
      rows = list(tables.read_rows(path, ('format', 'layout')))
      if len(rows) != 1 or '@catalog' in documents:
        raise ValueError('Expected one package settings record')
      documents['@catalog'] = dict(format=int(rows[0]['format']), layout=rows[0]['layout'])
      continue
    if entry['kind'] == 'rectangles':
      for row in tables.read_rows(path, RECTANGLE_FIELDS):
        resource = documents.setdefault(row['resource'], dict(codec='rectangles', rectangles=[]))
        if row['index'] == '':
          if any(row[field] for field in RECTANGLE_FIELDS[2:]):
            raise ValueError('Empty rectangle resource contains coordinates')
          continue
        if int(row['index']) != len(resource['rectangles']):
          raise ValueError('Out-of-order rectangle index')
        coordinates = {field: int(row[field]) for field in RECTANGLE_FIELDS[2:]}
        if any(not 0 <= value <= 255 for value in coordinates.values()):
          raise ValueError('Rectangle coordinate is outside its original byte range')
        resource['rectangles'].append(coordinates)
      continue
    if entry['kind'] in ('bytes', 'resource-bytes'):
      data = load_hex(root, entry['file'])
      name = entry['document']
      if name in documents:
        raise ValueError('Duplicate compact document: ' + entry['document'])
      if entry['kind'] == 'resource-bytes':
        documents[name] = dict(codec='byte-table', table=name + '#bytes')
        name += '#bytes'
      documents[name] = dict(description=entry['description'],
          rows=[list(data[at:at + 32]) for at in range(0, len(data), 32)])
      continue
    typed_columns = entry['kind'].startswith('columns:')
    if typed_columns:
      entry['kind'] = entry['kind'].removeprefix('columns:')
    collection = entry['kind'].startswith('collection:')
    children = entry['kind'].startswith('children:')
    codec = entry['kind'].removeprefix('codec:') if entry['kind'].startswith('codec:') else None
    if not (entry['kind'] in ('records', 'objects') or collection or children or codec):
      raise ValueError('Unknown compact table kind')
    with path.open(encoding='utf-8', newline='') as source:
      headers = next(csv.reader(source, delimiter='\t'), [])
    roots = entry['kind'] == 'objects' or codec and headers[0] == 'resource'
    prefix = ['resource', 'index'] if collection or children else ['resource'] if roots \
        else ['record', 'parent', 'index']
    if headers[:len(prefix)] != prefix or len(headers) != len(set(headers)):
      raise ValueError('Invalid record table header: ' + entry['file'])
    source = columns.read_rows(path, prefix, headers) if typed_columns else tables.read_rows(path, headers)
    for row in source:
      if collection:
        unused, collection_codec, field = entry['kind'].split(':')
        resource, ordinal = row.pop('resource'), row.pop('index')
        parent = resource + ':' + ('/' + field if field else '')
        if resource not in records:
          records[resource] = dict(codec=collection_codec, **{field: []},
              **COLLECTIONS[collection_codec][1]) if collection_codec else []
          references[resource] = {}
          documents[resource] = records[resource]
        if ordinal == '':
          if any(row.values()):
            raise ValueError('Empty collection contains record fields')
          continue
        references[resource]['/' + field if field else ''] = parent
        identifier = parent + ':' + ordinal
      elif children:
        resource, ordinal = row.pop('resource'), row.pop('index')
        field = entry['kind'].removeprefix('children:')
        parent = resource + ':' + field
        identifier = parent + ':' + ordinal
        automatic[resource][field] = parent
      elif roots:
        identifier, parent, ordinal = row.pop('resource'), '', ''
      else:
        identifier, parent, ordinal = (row.pop(key) for key in ('record', 'parent', 'index'))
      if identifier in records or identifier in documents:
        raise ValueError('Duplicate compact record: ' + identifier)
      value, refs = value_from_cells(identifier, row, infer_arrays=typed_columns)
      if codec and not parent:
        value['codec'] = codec
      records[identifier], references[identifier] = value, refs
      if parent:
        number = int(ordinal)
        if number < 0 or number in owners[parent]:
          raise ValueError('Duplicate or negative child record number')
        owners[parent][number] = identifier
      else:
        if ordinal:
          raise ValueError('Root record has a child index')
        documents[identifier] = value

  for identifier, fields in automatic.items():
    if identifier not in records:
      raise ValueError('Child table refers to a missing parent record: ' + identifier)
    for field, owner in fields.items():
      if field in references[identifier] and references[identifier][field] != owner:
        raise ValueError('Conflicting child table relationship')
      references[identifier][field] = owner
  resolved, active = {}, set()

  def resolve(identifier):
    if identifier in resolved:
      return resolved[identifier]
    if identifier in active:
      raise ValueError('Cyclic record reference')
    active.add(identifier)
    value = records[identifier]
    for field, owner in references[identifier].items():
      children = owners[owner]
      if not children or sorted(children) != list(range(len(children))):
        raise ValueError('Missing or noncontiguous child records: ' + owner)
      replacement = [resolve(children[number]) for number in range(len(children))]
      if not field:
        value = replacement
      else:
        parts = [part.replace('~1', '/').replace('~0', '~') for part in field[1:].split('/')]
        target = value
        for part in parts[:-1]:
          target = target[int(part)] if isinstance(target, list) else target[part]
        target[int(parts[-1]) if isinstance(target, list) else parts[-1]] = replacement
    active.remove(identifier)
    resolved[identifier] = value
    return value
  for identifier in documents:
    if identifier in records:
      documents[identifier] = resolve(identifier)
  return {name: models.expand_document(value) for name, value in documents.items()}


def write_events(path, events):
  order = ['delta', 'type', 'channel', 'note', 'velocity', 'controller', 'value', 'program',
      'pressure', 'lsb', 'msb', 'running_status', 'meta_type', 'status', 'text', 'data',
      'length', 'delta_encoding', 'length_encoding']
  keys = {key for event in events for key in event}
  fields = [key for key in order if key in keys] + sorted(keys - set(order)) or ['delta', 'type']
  rows = []
  for event in events:
    row = {}
    for field in fields:
      if field not in event:
        row[field] = ''
        continue
      value = next(tables.value_rows('', event[field]))
      row[field] = PREFIX[value['type']] + ':' + value['value']
    rows.append(row)
  columns.write_rows(path, [], fields, rows)


def read_events(path):
  with path.open(encoding='utf-8', newline='') as source:
    fields = next(csv.reader(source, delimiter='\t'), [])
  legacy = bool(fields) and fields[0] == 'event'
  if not fields or len(fields) != len(set(fields)):
    raise ValueError('Invalid music event columns')
  reverse, events = {prefix: kind for kind, prefix in PREFIX.items()}, []
  source = tables.read_rows(path, fields) if legacy else columns.read_rows(path, [], fields)
  for row in source:
    if legacy and int(row.pop('event')) != len(events):
      raise ValueError('Out-of-order music event')
    values = [dict(document='event', field='', type='object', value='')]
    for field, cell in row.items():
      if cell:
        if len(cell) < 2 or cell[1] != ':' or cell[0] not in reverse:
          raise ValueError('Invalid music event cell')
        values.append(dict(document='event', field='/' + tables.token(field),
            type=reverse[cell[0]], value=cell[2:]))
    events.append(tables.documents_from_rows(values)['event'])
  return events
