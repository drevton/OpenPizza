"""Flat TSV resource packages; encoders consume sheets and media directly in memory."""
from collections import Counter
from contextlib import contextmanager
from contextvars import ContextVar
import json
from pathlib import Path
import re
import shutil
import struct
from urllib.parse import quote
from .common import read_json, safe_path, png_read, pack_fields, MAX_PIXELS
from .sheets import sprite_name
from .tables import read_rows, write_rows, read_documents, write_documents
from . import compact, images, sheets, labels

_ACTIVE = ContextVar('flat_resource_package', default=None)
RESOURCE_FIELDS = ('path', 'codec', 'size', 'sha256', 'mode', 'mtime_ns')
LEGACY_SHEET_FIELDS = ('sheet', 'file', 'width', 'height', 'mode')
SHEET_FIELDS = LEGACY_SHEET_FIELDS + ('data_mode', 'border_indices')
LEGACY_SPRITE_FIELDS = ('sprite', 'sheet', 'x', 'y', 'width', 'height')
MERGED_SPRITE_FIELDS = LEGACY_SPRITE_FIELDS + ('flags', 'unused_pointer')
ANNOTATED_SPRITE_FIELDS = MERGED_SPRITE_FIELDS + ('name', 'description')
SHEET_ONLY_FIELDS = ('file', 'mode', 'data_mode', 'border_indices')
LEGACY_UNIFIED_SPRITE_FIELDS = ANNOTATED_SPRITE_FIELDS + SHEET_ONLY_FIELDS
CATEGORY_SPRITE_FIELDS = tuple(field for field in LEGACY_UNIFIED_SPRITE_FIELDS if field != 'file')
COMBINED_SPRITE_FIELDS = ('id',) + tuple(field for field in CATEGORY_SPRITE_FIELDS if field not in ('sprite', 'sheet'))
FONT_FIELDS = ('bit_x', 'advance', 'alias')
GLYPH_SPRITE_FIELDS = COMBINED_SPRITE_FIELDS + FONT_FIELDS
FONT_FILE_FIELDS = tuple('font_' + field for field, unused_format in images.FONT_HEADER) + ('font_tail_hex',)
FONT_HEADER_SPRITE_FIELDS = GLYPH_SPRITE_FIELDS + FONT_FILE_FIELDS
BOB_FILE_FIELDS = ('bob_opaque', 'bob_tail_hex')
VGA_FIELDS = ('vga_encoding', 'vga_compressed_tail_hex', 'vga_file_tail_hex')
SPRITE_FIELDS = FONT_HEADER_SPRITE_FIELDS + BOB_FILE_FIELDS + VGA_FIELDS
FRAME_FIELDS = ('resource', 'frame', 'sprite', 'flags', 'unused_pointer', 'empty_width', 'empty_height')
LEGACY_LABEL_FIELDS = ('sprite', 'group', 'label', 'description', 'row', 'column')
LABEL_FIELDS = ('resource', 'name', 'description', 'frames')
RLE_FIELDS = ('resource', 'offset', 'controls')
FILE_KEYS = {'file', 'text', 'hex', 'preamble', 'instrument_gap', 'events', 'instruments', 'extra', 'table'}


def current():
  return _ACTIVE.get()


def document_name(path):
  name = path.as_posix().removeprefix('files/')
  if name.endswith('/resource.json'):
    return name.removesuffix('/resource.json')
  return path.parent.as_posix().removeprefix('files/') + '#' + path.stem


def sprite_reference(identifier, sheet):
  resource, separator, name = identifier.rpartition('#')
  if not separator:
    raise ValueError('Expected a legacy resource/sprite ID')
  glyph = re.fullmatch(r'glyph-(\d+)', name) if Path(resource).suffix.lower() == '.fnt' else None
  if glyph:
    name = str(int(glyph[1]))
  elif name.isdecimal():
    name = str(int(name))
  elif Path(resource).suffix.lower() != '.fnt':
    stem = re.sub(r'[^a-zA-Z0-9-]+', '-', Path(resource).stem).strip('-').lower()
    name = stem if name in ('image', 'portrait', 'preview') else stem + '-' + name
  return dict(sprite=sheet + ':' + name)


def merge_font_sprites(documents, sprites):
  originals = dict(sprites)
  for resource, document in documents.items():
    if not isinstance(document, dict) or document.get('codec') != 'font':
      continue
    glyphs, spare = document.pop('glyphs'), document.pop('spare')
    categories = [sheets.sprite_key(item['image'])[0] for item in glyphs + spare if item.get('image')]
    category = categories[0] if categories else sheets.family_name(resource)
    for glyph in glyphs:
      image = glyph.get('image')
      source = sheets.sprite_key(image) if image else None
      key = (source[0] if source else category, str(glyph['codepoint']))
      if source == key:
        info = dict(originals[source])
      elif source:
        info = dict(sheet=key[0], alias=image['sprite'])
      else:
        info = dict(sheet=key[0], width=glyph['ink_width'], height=document['header']['height'])
      sprites[key] = dict(info, bit_x=glyph['bit_x'], advance=glyph.get('advance', ''))
    for item in spare:
      key = sheets.sprite_key(item['image'])
      sprites[key] = dict(originals[key], bit_x=item['bit_x'])


def merge_font_headers(documents, sheet_index):
  for resource, document in list(documents.items()):
    if not isinstance(document, dict) or document.get('codec') != 'font':
      continue
    base = sheets.family_name(resource)
    categories = [name for name, info in sheet_index.items() if info.get('data_mode') == 'L' and
        re.fullmatch(re.escape(base) + r'(?:-palette-\d+)?(?:-\d{2,}-\d{2,})?', name)]
    if not categories:
      raise ValueError('Font file header has no sheet: ' + resource)
    info = sheet_index[categories[0]]
    for field, unused_format in images.FONT_HEADER:
      value = document['header'][field]
      info['font_' + field] = ' '.join(map(str, value)) if isinstance(value, list) else str(value)
    info['font_tail_hex'] = document.get('tail_hex', '')
    del documents[resource]


def merge_graphic_resources(documents, sprites, sheet_index):
  for resource, document in list(documents.items()):
    if not isinstance(document, dict):
      continue
    codec = document.get('codec')
    if codec == 'bob':
      base = sheets.family_name(resource)
      categories = [key[0] for key, info in sprites.items() if info.get('flags', '') != '' and
          re.fullmatch(re.escape(base) + r'(?:-palette-\d+)?(?:-\d{2,}-\d{2,})?', key[0])]
      category = categories[0] if categories else base
      info = sheet_index.setdefault(category, dict(width='', height='', mode='', data_mode='', border_indices=[]))
      info.update(bob_opaque=str(document['opaque']), bob_tail_hex=document.get('tail_hex', ''))
    elif codec == 'palettes':
      if document['rgb_scale'] != 4 or any(sheets.sprite_key(bank)[1] != str(index)
          for index, bank in enumerate(document['banks'])):
        raise ValueError('Palette bank IDs must match their original indices')
    elif codec == 'vga':
      image = document['image']
      if 'sprite' in image:
        key = sheets.sprite_key(image)
      else:
        category = Path(image['file']).stem
        if category in sheet_index:
          raise ValueError('Standalone image collides with a sheet: ' + category)
        sheet_index[category] = dict(file=image['file'], width=image['width'], height=image['height'],
            mode=image['mode'], data_mode=image['mode'], border_indices=[])
        key = category, '0'
        sprites[key] = dict(sheet=category, x=0, y=0, width=image['width'], height=image['height'])
      sprites[key].update(vga_encoding=document['encoding'],
          vga_compressed_tail_hex=document.get('compressed_tail_hex', ''),
          vga_file_tail_hex=document.get('file_tail_hex', ''))
    else:
      continue
    del documents[resource]


def write_sprite_rows(path, sheet_index, sprites):
  """Each sheet header precedes its sprites; shared settings are written once."""
  grouped, empty = {name: [] for name in sheet_index}, []
  for name, info in sprites.items():
    if isinstance(name, tuple) and name[0] != info['sheet']:
      raise ValueError('Scoped sprite ID belongs to a different sheet')
    identifier = name[1] if isinstance(name, tuple) else name
    row = {**dict.fromkeys(SPRITE_FIELDS, ''), **info, 'id': info['sheet'] + ':' + identifier}
    if info['sheet']:
      if info['sheet'] in grouped:
        grouped[info['sheet']].append(row)
      elif info['width'] * info['height']:
        raise ValueError('Sprite refers to an unknown sheet: ' + info['sheet'])
      else:
        empty.append(row)
    else:
      empty.append(row)
  rows = []
  for name, info in sheet_index.items():
    row = {**dict.fromkeys(SPRITE_FIELDS, ''), **info, 'id': name}
    row['border_indices'] = ' '.join(map(str, info.get('border_indices', [])))
    rows.append(row)
    rows.extend(grouped[name])
  rows.extend(empty)
  write_rows(path, SPRITE_FIELDS, rows)


class FlatPackage:
  def __init__(self, root):
    self.root = root
    self.documents = read_documents(safe_path(root, 'metadata.tsv')) if (root / 'metadata.tsv').is_file() \
        else compact.read_package(root)
    for value in self.documents.values():
      if isinstance(value, dict) and value.get('codec') == 'vga' and 'compressed' in value:
        value['packets'] = [] if value.pop('compressed') else None
    if (root / 'graphics/rle.tsv').is_file():
      for row in read_rows(safe_path(root, 'graphics/rle.tsv'), RLE_FIELDS):
        resource = self.documents[row['resource']]
        data = bytes.fromhex(row['controls'])
        if resource.get('codec') != 'vga' or not isinstance(resource['packets'], list) or \
            int(row['offset']) != len(resource['packets']) or not 1 <= len(data) <= 64:
          raise ValueError('Invalid or out-of-order RLE control row')
        resource['packets'].extend(data)
    for value in self.documents.values():
      if isinstance(value, dict) and value.get('codec') == 'bob':
        value['frames'] = []
    self.catalog = dict(self.documents.pop('@catalog'))
    layout = re.fullmatch(r'flat-tsv-v([1-9]\d*)', self.catalog.get('layout', ''))
    self.version = int(layout[1]) if layout else 0
    if not 1 <= self.version <= 13:
      raise ValueError('Unsupported flat TSV resource layout')
    self.catalog['format'] = 1
    self.catalog['files'] = []
    for row in read_rows(safe_path(root, 'resources.tsv'), RESOURCE_FIELDS):
      for field in ('size', 'mode', 'mtime_ns'):
        row[field] = int(row[field])
      self.catalog['files'].append(row)
    self.catalog['codecs'] = dict(Counter(row['codec'] for row in self.catalog['files']))
    if self.version >= 12:
      for entry in self.catalog['files']:
        if entry['codec'] == 'font' or self.version >= 13 and entry['codec'] in ('bob', 'palettes', 'vga'):
          if entry['path'] in self.documents:
            raise ValueError('Graphics file data must be stored in sprites.tsv')
          self.documents[entry['path']] = dict(format=1, codec=entry['codec'])
          if entry['codec'] == 'bob':
            self.documents[entry['path']]['frames'] = []
    self.index = dict(format=1, sheets={}, sprites={})
    if self.version >= 7:
      self.read_unified_sprites()
    else:
      fields = LEGACY_SHEET_FIELDS if self.catalog['layout'] == 'flat-tsv-v1' else SHEET_FIELDS
      for row in read_rows(safe_path(root, 'graphics/sheets.tsv'), fields):
        name = row.pop('sheet')
        if name in self.index['sheets']:
          raise ValueError('Duplicate sheet ID: ' + name)
        row.update(width=int(row['width']), height=int(row['height']))
        row['data_mode'] = row.get('data_mode', row['mode'])
        row['border_indices'] = [int(value) for value in row.get('border_indices', '').split()]
        self.index['sheets'][name] = row
      if self.catalog['layout'] in ('flat-tsv-v5', 'flat-tsv-v6'):
        self.read_merged_sprites()
      else:
        self.read_legacy_sprites()
    if self.version < 6 and (root / 'graphics/labels.tsv').is_file():
      path = safe_path(root, 'graphics/labels.tsv')
      with path.open(encoding='utf-8') as source:
        legacy = source.readline().startswith('sprite\t')
      if legacy:
        for row in read_rows(path, LEGACY_LABEL_FIELDS):
          name = row.pop('sprite')
          row.update(row=int(row['row']), column=int(row['column']))
          self.index['sprites'][name].update(row)
      else:
        labeled = set()
        for row in read_rows(path, LABEL_FIELDS):
          members = []
          for number in labels.parse_frames(row['frames']):
            identifier = row['resource'] + f'#{number:04d}'
            if identifier in labeled:
              raise ValueError('Sprite belongs to multiple label rows: ' + identifier)
            labeled.add(identifier)
            sprite = self.index['sprites'][identifier]
            sprite.update(group=row['name'], label=row['name'], description=row['description'] or row['name'])
            members.append(sprite)
          positions = sorted({item['y'] for item in members})
          for y in positions:
            for column, sprite in enumerate(sorted((item for item in members if item['y'] == y),
                key=lambda item: item['x'])):
              sprite.update(row=positions.index(y), column=column)
    self.events = {}

  def read_unified_sprites(self):
    graphic_files, font_files, font_metrics = self.version >= 13, self.version >= 12, self.version >= 11
    combined, categorized = self.version >= 10, self.version >= 9
    if graphic_files:
      fields = SPRITE_FIELDS
    elif font_files:
      fields = FONT_HEADER_SPRITE_FIELDS
    elif font_metrics:
      fields = GLYPH_SPRITE_FIELDS
    elif combined:
      fields = COMBINED_SPRITE_FIELDS
    else:
      fields = CATEGORY_SPRITE_FIELDS if categorized else LEGACY_UNIFIED_SPRITE_FIELDS
    rows = list(read_rows(safe_path(self.root, 'graphics/sprites.tsv'), fields))
    sprites, headers = [], set()
    for row in rows:
      if combined:
        identifier = row.pop('id')
        if ':' in identifier:
          row['sheet'], row['sprite'] = sheets.sprite_key(dict(sprite=identifier))
        else:
          row.update(sheet=identifier, sprite='')
      if row['sprite']:
        if any(row.get(field) for field in BOB_FILE_FIELDS):
          raise ValueError('BOB file headers belong on sheet header rows')
        if any(row.get(field) for field in FONT_FILE_FIELDS):
          raise ValueError('Font file headers belong on sheet header rows')
        if any(row.get(field) for field in SHEET_ONLY_FIELDS):
          raise ValueError('Sheet settings belong on the sheet header row: ' + row['sprite'])
        sprites.append(row)
        continue
      name = row['sheet']
      if name in headers:
        raise ValueError('Duplicate sheet ID: ' + name)
      headers.add(name)
      if any(row.get(field) for field in VGA_FIELDS):
        raise ValueError('VGA encoding belongs on an image row')
      if graphic_files and any(row[field] for field in BOB_FILE_FIELDS):
        self.read_bob_header(name, row)
      canvas_fields = ('width', 'height', 'mode', 'data_mode', 'border_indices')
      if graphic_files and all(row[field] == '' for field in canvas_fields):
        if row['bob_opaque'] == '' or any(row[field] for field in FONT_FILE_FIELDS + FONT_FIELDS) or \
            any(row[field] for field in ('x', 'y', 'flags', 'unused_pointer', 'name', 'description')):
          raise ValueError('Invalid resource-only BOB header')
        continue
      if categorized:
        row['file'] = 'graphics/' + name + '.png'
        safe_path(self.root, row['file'])
      if not name or not row['file'] or any(row[field] for field in
          ('x', 'y', 'flags', 'unused_pointer', 'name', 'description')) or \
          any(row.get(field) for field in FONT_FIELDS):
        raise ValueError('Invalid sheet header row')
      if name in self.index['sheets']:
        raise ValueError('Duplicate sheet ID: ' + name)
      info = {field: row[field] for field in ('file', 'width', 'height', 'mode', 'data_mode')}
      info.update(width=int(info['width']), height=int(info['height']))
      info['border_indices'] = [int(value) for value in row['border_indices'].split()]
      if info['width'] <= 0 or info['height'] <= 0 or info['mode'] not in ('P', 'L', 'RGB', 'RGBA') or \
          not (info['data_mode'] == info['mode'] or info['mode'] == 'P' and info['data_mode'] == 'L') or \
          len(info['border_indices']) != len(set(info['border_indices'])) or \
          any(not 0 <= value <= 255 for value in info['border_indices']):
        raise ValueError('Invalid sheet dimensions, modes or border indices: ' + name)
      self.index['sheets'][name] = info
      if font_files and any(row[field] for field in FONT_FILE_FIELDS):
        self.read_font_header(name, row)
    if font_files:
      for resource, document in self.documents.items():
        if isinstance(document, dict) and document.get('codec') == 'font' and 'header' not in document:
          raise ValueError('Missing font file header: ' + resource)
    if graphic_files:
      self.read_graphic_resources(sprites)
    if font_metrics:
      self.read_font_sprites(sprites)
    else:
      self.read_merged_sprites(sprites)

  def read_bob_header(self, category, row):
    resource = self.category_resource(category, 'bob')
    document = self.documents[resource]
    if 'opaque' in document:
      raise ValueError('Duplicate BOB file header: ' + resource)
    opaque = int(row['bob_opaque'])
    if not 0 <= opaque <= 0xffffffff:
      raise ValueError('BOB header word is outside its original range')
    bytes.fromhex(row['bob_tail_hex'])
    document.update(opaque=opaque, tail_hex=row['bob_tail_hex'])

  def read_graphic_resources(self, rows):
    banks = {}
    for row in rows:
      category, identifier = row['sheet'], row['sprite']
      if any(row[field] for field in VGA_FIELDS):
        if row['vga_encoding'] not in ('raw', 'native-rle') or row['bit_x'] or row['flags'] or row['alias']:
          raise ValueError('Invalid VGA image encoding fields')
        resource = self.vga_resource(category, identifier)
        document = self.documents[resource]
        if 'image' in document:
          raise ValueError('Duplicate VGA image: ' + resource)
        for field in VGA_FIELDS[1:]:
          bytes.fromhex(row[field])
        document.update(image=dict(sprite=category + ':' + identifier), encoding=row['vga_encoding'],
            compressed_tail_hex=row['vga_compressed_tail_hex'], file_tail_hex=row['vga_file_tail_hex'])
      if re.fullmatch(r'palettes(?:-palette-\d+)?(?:-\d{2,}-\d{2,})?', category):
        resource = self.category_resource(category, 'palettes')
        if not identifier.isdecimal() or identifier != str(int(identifier)):
          raise ValueError('Palette bank ID must be its numeric index')
        members = banks.setdefault(resource, {})
        if int(identifier) in members:
          raise ValueError('Duplicate palette bank index')
        members[int(identifier)] = dict(sprite=category + ':' + identifier)
    for resource, document in self.documents.items():
      if not isinstance(document, dict):
        continue
      codec = document.get('codec')
      if codec == 'bob' and 'opaque' not in document:
        raise ValueError('Missing BOB file header: ' + resource)
      if codec == 'vga' and 'image' not in document:
        raise ValueError('Missing VGA image: ' + resource)
      if codec == 'palettes':
        members = banks.get(resource, {})
        if sorted(members) != list(range(len(members))):
          raise ValueError('Missing or noncontiguous palette banks')
        document.update(banks=[members[index] for index in range(len(members))], rgb_scale=4)

  def vga_resource(self, category, identifier):
    matches = []
    standalone = {'gfx/euromap.vga': 'europe-map', 'gfx/permap.vga': 'person-panel'}
    for entry in self.catalog['files']:
      if entry['codec'] != 'vga':
        continue
      resource = entry['path']
      if resource.lower() in standalone:
        base, local = standalone[resource.lower()], '0'
      else:
        base = sheets.family_name(Path(resource).parent.as_posix() + '/interface')
        local = re.sub(r'[^a-zA-Z0-9-]+', '-', Path(resource).stem).strip('-').lower()
      if identifier == local and re.fullmatch(re.escape(base) + r'(?:-palette-\d+)?(?:-\d{2,}-\d{2,})?', category):
        matches.append(resource)
    if len(matches) != 1:
      raise ValueError('Sprite ID must identify one VGA resource: ' + category + ':' + identifier)
    return matches[0]

  def read_font_header(self, category, row):
    resource = self.category_resource(category, 'font')
    document = self.documents[resource]
    if 'header' in document:
      raise ValueError('Duplicate font file header: ' + resource)
    header = {}
    try:
      for field, fmt in images.FONT_HEADER:
        text = row['font_' + field]
        header[field] = [int(item) for item in text.split()] if fmt[0].isdecimal() else int(text)
      pack_fields(header, images.FONT_HEADER)
      bytes.fromhex(row['font_tail_hex'])
    except (ValueError, struct.error) as error:
      raise ValueError('Invalid font file header: ' + resource) from error
    if header['height'] <= 0 or header['row_stride'] <= 0 or header['last_char'] - header['first_char'] + 2 <= 0 or \
        header['height'] * header['row_stride'] * 8 > MAX_PIXELS:
      raise ValueError('Invalid font file dimensions: ' + resource)
    document.update(header=header, tail_hex=row['font_tail_hex'])

  def category_resource(self, category, codec):
    matches = []
    for entry in self.catalog['files']:
      if entry['codec'] != codec:
        continue
      base = 'palettes' if codec == 'palettes' else sheets.family_name(entry['path'])
      if re.fullmatch(re.escape(base) + r'(?:-palette-\d+)?(?:-\d{2,}-\d{2,})?', category):
        matches.append(entry['path'])
    if len(matches) != 1:
      label = 'BOB' if codec == 'bob' else codec
      raise ValueError('Sprite category must identify one ' + label + ' resource: ' + category)
    return matches[0]

  def read_font_sprites(self, rows):
    pixels, aliases, glyphs, spare, seen = [], {}, {}, {}, set()
    for row in rows:
      key = row['sheet'], row['sprite']
      if key in seen:
        raise ValueError('Empty or duplicate sprite ID: ' + row['sprite'])
      seen.add(key)
      if row['bit_x'] == '':
        if row['advance'] or row['alias']:
          raise ValueError('Font metrics require bit_x: ' + row['sprite'])
        pixels.append(row)
        continue
      category, identifier = row['sheet'], row['sprite']
      key = category, identifier
      if row['flags'] or row['unused_pointer']:
        raise ValueError('Invalid font sprite header fields: ' + category + ':' + identifier)
      resource = self.category_resource(category, 'font')
      header = self.documents[resource]['header']
      bit_x = int(row['bit_x'])
      if not 0 <= bit_x <= header['row_stride'] * 8:
        raise ValueError('Font bit position is outside its bitplane')
      item = dict(bit_x=bit_x)
      if identifier.isdecimal() and identifier == str(int(identifier)):
        codepoint = int(identifier)
        target = glyphs.setdefault(resource, {})
        if codepoint in target:
          raise ValueError('Duplicate font codepoint: ' + str(codepoint))
        target[codepoint] = item
        item['codepoint'] = codepoint
        if header['spacing_mode'] != 2:
          if row['advance'] == '' or not 0 <= int(row['advance']) <= 65535:
            raise ValueError('Font glyph lacks a valid advance')
          item['advance'] = int(row['advance'])
        elif row['advance']:
          raise ValueError('Fixed-spacing font has an unused advance')
      elif re.fullmatch(r'unused-\d+', identifier):
        if row['advance']:
          raise ValueError('Unused font pixels have a glyph advance')
        spare.setdefault(resource, []).append(item)
      else:
        raise ValueError('Invalid font sprite ID: ' + identifier)
      if row['alias']:
        if any(row[field] for field in ('x', 'y', 'width', 'height')):
          raise ValueError('Font alias duplicates a pixel rectangle')
        target = sheets.sprite_key(dict(sprite=row['alias']))
        if not isinstance(target, tuple):
          raise ValueError('Font alias requires sheet context')
        aliases[key] = target, row
        item['image'] = dict(sprite=category + ':' + identifier)
      elif row['width'] == '0':
        if 'codepoint' not in item or row['x'] or row['y'] or int(row['height']) != header['height']:
          raise ValueError('Invalid blank font glyph')
        item.update(image=None, ink_width=0)
      else:
        item['image'] = dict(sprite=category + ':' + identifier)
        pixels.append(row)
    self.read_merged_sprites(pixels)
    active = set()

    def resolve(key):
      if key in self.index['sprites']:
        return self.index['sprites'][key]
      if key in active:
        raise ValueError('Cyclic font sprite alias')
      if key not in aliases:
        raise ValueError('Unknown sprite reference: ' + str(key))
      active.add(key)
      target, row = aliases[key]
      self.index['sprites'][key] = dict(resolve(target), group=row['name'], label=row['name'],
          description=row['description'] or row['name'])
      active.remove(key)
      return self.index['sprites'][key]
    for key in aliases:
      resolve(key)
    for resource, document in self.documents.items():
      if not isinstance(document, dict) or document.get('codec') != 'font':
        continue
      header = document['header']
      members = glyphs.get(resource, {})
      expected = list(range(header['first_char'], header['last_char'] + 2))
      if sorted(members) != expected:
        raise ValueError('Missing or out-of-range font codepoints: ' + resource)
      document['glyphs'] = [members[number] for number in expected]
      document['spare'] = sorted(spare.get(resource, []), key=lambda item: item['bit_x'])

  def read_merged_sprites(self, rows=None):
    frames, seen, groups = {}, set(), {}
    categorized, annotated = self.version >= 9, self.version >= 6
    if rows is None:
      fields = ANNOTATED_SPRITE_FIELDS if annotated else MERGED_SPRITE_FIELDS
      rows = read_rows(safe_path(self.root, 'graphics/sprites.tsv'), fields)
    for row in rows:
      name = row['sprite']
      scoped = categorized or self.catalog['layout'] == 'flat-tsv-v8' and '#' not in name
      if scoped and not row['sheet']:
        raise ValueError('Local sprite ID requires a sheet: ' + name)
      key = (row['sheet'], name) if scoped else name
      if not name or key in seen:
        raise ValueError('Empty or duplicate sprite ID: ' + name)
      seen.add(key)
      resource, separator, number = name.rpartition('#')
      document = self.documents.get(resource)
      is_frame = isinstance(document, dict) and document.get('codec') == 'bob'
      if categorized:
        if '#' in name or ':' in name or '/' in name:
          raise ValueError('Expected a local sprite ID: ' + name)
        is_frame = bool(row['flags'] or row['unused_pointer'])
        if is_frame:
          resource, number = self.category_resource(row['sheet'], 'bob'), name
      if is_frame:
        if not number.isdecimal() or number != (str(int(number)) if categorized else f'{int(number):04d}'):
          raise ValueError('Invalid BOB frame ID: ' + name)
        if not row['flags'] or not row['unused_pointer']:
          raise ValueError('BOB frame lacks header fields: ' + name)
        frame = dict(flags=int(row['flags']), unused_pointer=int(row['unused_pointer']))
        if not 0 <= frame['flags'] <= 255 or not 0 <= frame['unused_pointer'] <= 0xffffffff:
          raise ValueError('BOB frame header is outside its original range: ' + name)
        if int(number) in frames.setdefault(resource, {}):
          raise ValueError('Duplicate BOB frame index: ' + number)
        frames[resource][int(number)] = frame
      elif row['flags'] or row['unused_pointer']:
        raise ValueError('Frame header fields on a non-BOB sprite: ' + name)
      width, height = int(row['width']), int(row['height'])
      if is_frame and (not 0 <= width <= 255 or not 0 <= height <= 255):
        raise ValueError('BOB frame dimensions are outside their original byte range: ' + name)
      if categorized and is_frame and not width * height:
        if row['x'] or row['y']:
          raise ValueError('Empty BOB frame has sheet coordinates: ' + name)
        frame.update(image=None, width=width, height=height)
        continue
      if row['sheet']:
        if row['sheet'] not in self.index['sheets'] or width <= 0 or height <= 0:
          raise ValueError('Invalid sprite sheet or dimensions: ' + name)
        if row['x'] == '' or row['y'] == '':
          raise ValueError('Missing sprite rectangle location: ' + name)
        sprite = dict(sheet=row['sheet'], x=int(row['x']), y=int(row['y']), width=width, height=height)
        self.index['sprites'][key] = sprite
        if annotated:
          sprite.update(group=row['name'], label=row['name'], description=row['description'] or row['name'])
          if row['name']:
            groups.setdefault((row['sheet'] if scoped else resource, row['name']), []).append(sprite)
        if is_frame:
          frame['image'] = dict(sprite=row['sheet'] + ':' + name) if categorized else dict(sprite=name)
      else:
        if not is_frame or row['x'] or row['y'] or width * height or not 0 <= width <= 255 or not 0 <= height <= 255:
          raise ValueError('Only empty BOB frames may omit a sprite sheet: ' + name)
        frame.update(image=None, width=width, height=height)
    for resource, members in frames.items():
      if sorted(members) != list(range(len(members))):
        raise ValueError('Missing or noncontiguous BOB frames: ' + resource)
      self.documents[resource]['frames'] = [members[number] for number in range(len(members))]
    for members in groups.values():
      positions = sorted({item['y'] for item in members})
      for y in positions:
        ordered = sorted((item for item in members if item['y'] == y), key=lambda item: item['x'])
        for column, sprite in enumerate(ordered):
          sprite.update(row=positions.index(y), column=column)

  def read_legacy_sprites(self):
    for row in read_rows(safe_path(self.root, 'graphics/frames.tsv'), FRAME_FIELDS):
      resource = self.documents[row['resource']]
      if resource.get('codec') != 'bob' or int(row['frame']) != len(resource['frames']):
        raise ValueError('Unexpected or out-of-order sprite frame: ' + row['resource'])
      frame = dict(flags=int(row['flags']), unused_pointer=int(row['unused_pointer']))
      if row['sprite']:
        if row['empty_width'] or row['empty_height']:
          raise ValueError('Nonempty frame duplicates sprite dimensions')
        frame['image'] = dict(sprite=row['sprite'])
      else:
        frame.update(image=None, width=int(row['empty_width']), height=int(row['empty_height']))
        if frame['width'] * frame['height']:
          raise ValueError('Nonempty frame lacks a sprite ID')
      resource['frames'].append(frame)
    for row in read_rows(safe_path(self.root, 'graphics/sprites.tsv'), LEGACY_SPRITE_FIELDS):
      name = row.pop('sprite')
      if name in self.index['sprites']:
        raise ValueError('Duplicate sprite ID: ' + name)
      row.update((key, int(row[key])) for key in ('x', 'y', 'width', 'height'))
      self.index['sprites'][name] = row

  def document(self, path):
    relative = path.relative_to(self.root)
    if relative.as_posix() == 'catalog.json':
      return self.catalog
    if relative.as_posix() == 'sheets/sprites.json':
      return self.index
    return self.documents[document_name(relative)]

  def related(self, name):
    if name.startswith('audio/') and name.endswith('.tsv'):
      if name not in self.events:
        self.events[name] = read_documents(safe_path(self.root, name))['events'] \
            if self.catalog['layout'] == 'flat-tsv-v1' else compact.read_events(safe_path(self.root, name))
      return self.events[name]
    return self.documents[name]


@contextmanager
def package_sources(root):
  package = FlatPackage(root) if (root / 'resources.tsv').is_file() else None
  token = _ACTIVE.set(package)
  try:
    yield package
  finally:
    _ACTIVE.reset(token)


def write_export(source, target):
  """Translate a legacy export once; final packages contain no JSON or directory-per-resource tree."""
  if not (source / 'sheets/sprites.json').is_file():
    raise ValueError('Flat exports require sprite sheets; consolidate individual PNGs first')
  catalog = read_json(source / 'catalog.json')
  index = read_json(source / 'sheets/sprites.json')
  identifiers = {key: sprite_name(key) for key in index['sprites']}
  if len(set(identifiers.values())) != len(identifiers):
    raise ValueError('Sprite ID collision')
  references, used_ids = {}, set()
  for key, info in index['sprites'].items():
    reference = sprite_reference(identifiers[key], info['sheet'])
    base, ordinal = reference['sprite'], 2
    while sheets.sprite_key(reference) in used_ids:
      reference = dict(sprite=base + '-' + str(ordinal))
      ordinal += 1
    used_ids.add(sheets.sprite_key(reference))
    references[key] = reference
  legacy_references = {identifiers[key]: reference for key, reference in references.items()}
  media, documents, events, names, shared_events = {}, {}, {}, set(), {}
  paths = sorted((source / 'files').rglob('*'))
  for path in paths:
    if not path.is_file():
      continue
    relative = path.relative_to(source)
    if path.suffix == '.json':
      identifier = document_name(relative)
      if identifier in documents or identifier.startswith('@'):
        raise ValueError('Resource document ID collision: ' + identifier)
      documents[identifier] = (relative.parent, read_json(path))
      continue
    area = 'graphics' if path.suffix == '.png' else 'audio' if path.suffix in ('.wav', '.jsonl') else 'data'
    if path.name in ('preamble.hex', 'instrument-gap.hex'):
      area = 'audio'
    name = quote(relative.as_posix().removeprefix('files/').replace('/', '__'), safe='._-')
    if path.suffix == '.png':
      match = re.fullmatch(r'(map|bez)(\d+)\.dat', path.parent.name.lower())
      if match:
        name = ('map-' if match[1] == 'map' else 'district-') + match[2] + '.png'
      elif path.parent.name.upper() == 'EUROMAP.VGA':
        name = 'europe-map.png'
      elif path.parent.name.upper() == 'PERMAP.VGA':
        name = 'person-panel.png'
    if path.suffix == '.jsonl':
      name = str(Path(name).with_suffix('.tsv'))
    destination = area + '/' + name
    if path.suffix == '.jsonl':
      decoded = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines() if line.strip()]
      key = json.dumps(decoded, sort_keys=True, separators=(',', ':'))
      if key in shared_events:
        media[relative.as_posix()] = shared_events[key]
        continue
      shared_events[key] = destination
      events[destination] = decoded
    if destination.casefold() in names:
      raise ValueError('Flat media filename collision: ' + destination)
    names.add(destination.casefold())
    media[relative.as_posix()] = destination
  for name, info in index['sheets'].items():
    destination = 'graphics/' + name + '.png'
    if destination.casefold() in names:
      raise ValueError('Sheet filename collision')
    names.add(destination.casefold())
    media[info['file']] = destination

  def convert(value, directory, key=''):
    if isinstance(value, dict):
      if 'sprite' in value:
        return dict(references[value['sprite']])
      result = {name: convert(item, directory, name) for name, item in value.items()}
      if value.get('codec') == 'bob':
        for frame in result['frames']:
          if frame['image']:
            del frame['width'], frame['height']
      if value.get('codec') == 'font' and 'spare' in value:
        for glyph in result['glyphs']:
          if glyph['image']:
            del glyph['ink_width']
      return result
    if isinstance(value, list):
      return [convert(item, directory) for item in value]
    if isinstance(value, str) and key in FILE_KEYS:
      relative = directory / value
      if relative.as_posix() in media:
        return media[relative.as_posix()]
      if relative.suffix == '.json' and document_name(relative) in documents:
        return document_name(relative)
    return value

  metadata = {name: convert(value, directory) for name, (directory, value) in documents.items()}
  sprites = {sheets.sprite_key(references[name]):
      dict(info, flags='', unused_pointer='', name='', description='')
      for name, info in index['sprites'].items()}
  with sheets.image_sources(source):
    for name, value in metadata.items():
      if isinstance(value, dict) and value.get('codec') == 'vga':
        if value.get('encoding') in ('raw', 'native-rle'):
          continue
        original = documents[name][1]
        pixels = png_read(source / 'files' / name, original['image'])
        controls = value.pop('packets')
        if controls is None:
          value['encoding'] = 'raw'
        elif images.rle_original(pixels, controls, False) == images.rle_encode(pixels):
          value['encoding'] = 'native-rle'
        else:
          raise ValueError(name + ': non-native packet choices cannot be inferred from pixels; '
              'use --legacy-layout to preserve an unsupported source encoding')
  for name, value in metadata.items():
    if isinstance(value, dict) and value.get('codec') == 'bob':
      categories = [sheets.sprite_key(frame['image'])[0] for frame in value['frames'] if frame['image']]
      category = categories[0] if categories else sheets.family_name(name)
      for number, frame in enumerate(value.pop('frames')):
        key = sheets.sprite_key(frame['image']) if frame['image'] else (category, str(number))
        info = dict(sprites[key]) if frame['image'] else \
            dict(sheet=category, x='', y='', width=frame['width'], height=frame['height'], name='', description='')
        sprites[key] = dict(info, flags=frame['flags'], unused_pointer=frame['unused_pointer'])
  for row in labels.legend_rows({identifiers[name]: info for name, info in index['sprites'].items()}):
    for number in labels.parse_frames(row['frames']):
      key = sheets.sprite_key(legacy_references[row['resource'] + f'#{number:04d}'])
      sprites[key].update(name=row['name'], description=row['description'])
  metadata['@catalog'] = {key: value for key, value in catalog.items() if key not in ('files', 'codecs')}
  merge_font_sprites(metadata, sprites)
  sheet_index = {name: dict(info, file=media[info['file']], data_mode=info.get('data_mode', info['mode']))
      for name, info in index['sheets'].items()}
  merge_font_headers(metadata, sheet_index)
  merge_graphic_resources(metadata, sprites, sheet_index)
  metadata['@catalog']['layout'] = 'flat-tsv-v13'
  metadata['@graphics'] = {key: value for key, value in index.items() if key not in ('sheets', 'sprites')}
  write_rows(target / 'resources.tsv', RESOURCE_FIELDS, catalog['files'])
  compact.write_package(target, metadata)
  write_sprite_rows(target / 'graphics/sprites.tsv', sheet_index, sprites)
  for original, destination in media.items():
    output = safe_path(target, destination)
    output.parent.mkdir(parents=True, exist_ok=True)
    if destination in events:
      compact.write_events(output, events[destination])
    else:
      shutil.copy2(safe_path(source, original), output)
  print(f'Wrote flat TSV package: {len(catalog["files"])} resources; {len(index["sprites"])} sprite rectangles')
