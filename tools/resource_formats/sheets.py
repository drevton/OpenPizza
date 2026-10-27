"""Labeled sprite sheets with authoritative rectangles and lossless palette indices."""
from contextlib import contextmanager
from contextvars import ContextVar
from pathlib import Path, PurePosixPath
import re
from PIL import Image, ImageDraw
from .common import MAX_PIXELS, read_json, write_json, safe_path
from . import labels, sheet_layout

_ACTIVE = ContextVar('resource_sheet_reader', default=None)


def sprite_key(reference):
  if 'sheet' in reference:
    if not reference['sheet'] or not reference['sprite']:
      raise ValueError('Scoped sprite reference requires a sheet and sprite ID')
    return reference['sheet'], reference['sprite']
  if ':' in reference['sprite']:
    sheet, separator, identifier = reference['sprite'].partition(':')
    if not sheet or not identifier or ':' in identifier:
      raise ValueError('Invalid category:sprite reference')
    return sheet, identifier
  return reference['sprite']


def family_name(family):
  path = PurePosixPath(family)
  if path.parent.name.lower() == 'bob' and path.suffix.lower() == '.bob':
    family = path.stem
  elif path.parent.name.lower() == 'fonts' and path.suffix.lower() == '.fnt':
    family = ('meny' if path.stem.lower() == 'menu' else path.stem) + '-font'
  return re.sub(r'[^a-zA-Z0-9-]+', '-', family).strip('-').lower()


class SheetReader:
  def __init__(self, root):
    self.root = root
    self.index = None
    self.images = {}

  def read(self, reference):
    if self.index is None:
      self.index = read_json(safe_path(self.root, 'sheets/sprites.json'))
      if self.index.get('format') != 1:
        raise ValueError('Unsupported sprite index version')
    key = sprite_key(reference)
    if key not in self.index['sprites']:
      if isinstance(key, str) and '#' not in key:
        raise ValueError('Local sprite reference requires sheet context: ' + key)
      raise ValueError('Unknown sprite reference: ' + str(key))
    sprite = self.index['sprites'][key]
    info = self.index['sheets'][sprite['sheet']]
    name = info['file']
    if name not in self.images:
      with Image.open(safe_path(self.root, name)) as image:
        if image.format != 'PNG' or image.mode != info['mode'] or image.size != (info['width'], info['height']):
          raise ValueError('Sheet dimensions/mode changed: ' + name)
        if image.width * image.height > MAX_PIXELS:
          raise ValueError('Sprite sheet is too large')
        self.images[name] = image.copy()
    image = self.images[name]
    x, y, width, height = (sprite[key] for key in ('x', 'y', 'width', 'height'))
    if any(not isinstance(value, int) for value in (x, y, width, height)) or \
        x < 0 or y < 0 or width <= 0 or height <= 0 or x + width > image.width or y + height > image.height:
      raise ValueError('Sprite rectangle is outside its sheet: ' + reference['sprite'])
    data_mode = info.get('data_mode', image.mode)
    if any(reference.get(key, value) != value for key, value in
        (('width', width), ('height', height), ('mode', data_mode))):
      raise ValueError('Sprite rectangle dimensions/mode changed: ' + reference['sprite'])
    cropped = image.crop((x, y, x + width, y + height))
    if image.mode == 'P' and set(cropped.tobytes()) & set(info.get('border_indices', [])):
      raise ValueError('Decoration-only palette index inside sprite: ' + reference['sprite'])
    return Image.frombytes('L', cropped.size, cropped.tobytes()) if data_mode == 'L' and image.mode == 'P' else cropped


@contextmanager
def image_sources(root):
  reader = SheetReader(root)
  token = _ACTIVE.set(reader)
  try:
    yield
  finally:
    _ACTIVE.reset(token)
    for image in reader.images.values():
      image.close()


def read_sprite(reference):
  reader = _ACTIVE.get()
  if reader is None:
    raise ValueError('Shared sprite requires the package-level pack/verify command')
  return reader.read(reference)


def image_references(value):
  if isinstance(value, dict):
    if isinstance(value.get('file'), str) and value['file'].lower().endswith('.png'):
      yield value
    else:
      for item in value.values():
        yield from image_references(item)
  elif isinstance(value, list):
    for item in value:
      yield from image_references(item)


def decoration(mode, palette, used):
  display_mode = 'P' if mode == 'L' else mode
  if display_mode == 'P':
    palette = bytearray(palette or bytes(value for index in range(256) for value in (index, index, index)))
    actual_colors = {tuple(palette[index * 3:index * 3 + 3]) for index in used}
  else:
    actual_colors = {tuple(color[:3]) for color in used}
  chosen = []
  for position, candidate in enumerate(((192, 0, 255), (0, 190, 220), (255, 255, 255), (0, 0, 0))):
    while position < 2 and (candidate in actual_colors or candidate in chosen):
      candidate = ((candidate[0] + 1) % 256, candidate[1], candidate[2])
    chosen.append(candidate)
  reserved = []
  if display_mode == 'P':
    reserved = [index for index in range(255, -1, -1) if index not in used][:4]
    if len(reserved) != 4:
      raise ValueError('No unused indices for distinct sheet decoration')
    for index, color in zip(reserved, chosen):
      palette[index * 3:index * 3 + 3] = bytes(color)
    colors = reserved
  else:
    colors = [color + (255,) if display_mode == 'RGBA' else color for color in chosen]
  return display_mode, palette, reserved, colors


def label_for(path):
  return sprite_name(path)


def sprite_name(path):
  path = PurePosixPath(path)
  source = path.parent.as_posix().removeprefix('files/')
  return source + '#' + path.stem.removeprefix('frame-').removeprefix('palette-')


def label_font():
  path = Path(__file__).resolve().parents[2] / 'Gfx/3x5_font.png'
  if not path.is_file():
    from .tiny_font import label_font as builtin_font
    return builtin_font()
  with Image.open(path) as image:
    if image.size != (128, 24) or image.mode != 'L':
      raise ValueError('Expected the 128x24 ASCII 3x5 font atlas: ' + str(path))
    return {chr(code): image.crop((code % 32 * 4, code // 32 * 6,
        code % 32 * 4 + 3, code // 32 * 6 + 5)).point(lambda value: 255 - value)
        for code in range(32, 127)}


def draw_label(image, x, y, text, font, ink):
  for character in text:
    image.paste(ink, (x, y, x + 3, y + 5), font.get(character, font['?']))
    x += 4


def consolidate(root, size=1024, catalog=None):
  if not 64 <= size <= 4096:
    raise ValueError('Sheet size must be between 64 and 4096')
  if (root / 'sheets').exists():
    raise ValueError('Export already has sprite sheets')
  font = label_font()
  catalog = catalog or {}
  groups, documents, originals, known = {}, [], set(), {}
  for path in sorted((root / 'files').rglob('resource.json')):
    meta = read_json(path)
    if isinstance(meta.get('preview'), str):
      meta['preview'] = dict(file=meta['preview'])
    documents.append((path, meta))
    source = path.parent.relative_to(root / 'files').as_posix()
    # Maps remain complete standalone images, with their embedded viewing palette.
    if meta['codec'] == 'cell-map' or source.upper() in ('GFX/EUROMAP.VGA', 'GFX/PERMAP.VGA'):
      continue
    if meta['codec'] in ('bob', 'font'):
      family = source
    elif meta['codec'] == 'character':
      family = 'portraits'
    elif meta['codec'] == 'vga':
      family = str(PurePosixPath(source).parent) + '/interface'
    else:
      family = meta['codec']
    for reference in image_references(meta):
      original = safe_path(path.parent, reference['file'])
      key = original.relative_to(root).as_posix()
      if key in known:
        reference.clear()
        reference.update(known[key])
        continue
      with Image.open(original) as image:
        if image.format != 'PNG' or image.width * image.height > MAX_PIXELS:
          raise ValueError('Invalid PNG artifact: ' + key)
        mode = image.mode
        if mode not in ('P', 'L', 'RGB', 'RGBA'):
          raise ValueError('Unsupported sheet image mode: ' + mode)
        width, height = image.size
        palette = bytes(image.getpalette() or []) if mode == 'P' else b''
        if mode in ('P', 'L'):
          used = set(image.tobytes())
        else:
          used = set(image.get_flattened_data() if hasattr(image, 'get_flattened_data') else image.getdata())
        if image.info.get('transparency') is not None:
          raise ValueError('PNG transparency metadata needs explicit sheet support: ' + key)
      if any(reference.get(name, expected) != expected for name, expected in
          (('width', width), ('height', height), ('mode', mode))):
        raise ValueError('PNG differs from its resource metadata: ' + key)
      reference.clear()
      reference.update(sprite=key, width=width, height=height, mode=mode)
      known[key] = dict(reference)
      annotation = labels.describe(source, original.name, meta['codec'], catalog)
      item = dict(key=key, original=original, width=width, height=height, used=used,
          is_font=meta['codec'] == 'font', **annotation)
      groups.setdefault((family, mode, palette), []).append(item)
      originals.add(original)
  output = root / 'sheets'
  output.mkdir()
  index = dict(format=1, note='Named related sets share a purple outline and single cyan separators on white.',
      labels=dict(font='Gfx/3x5_font.png', glyph_width=3, glyph_height=5, advance=4, border_pixels=1),
      sheets={}, sprites={})
  family_versions = {}
  for (family, mode, palette), items in groups.items():
    # Choose palette batches by whole logical sets, never by individual rotations or poses.
    logical_sets = {}
    for item in items:
      logical_sets.setdefault(item['group'], []).append(item)
    batches, batch, used = [], [], set()
    for members in logical_sets.values():
      colors_used = set().union(*(item['used'] for item in members))
      if mode in ('P', 'L') and batch and len(used | colors_used) > 252:
        batches.append((batch, used))
        batch, used = [], set()
      batch.extend(members)
      used |= colors_used
    if batch:
      batches.append((batch, used))
    family_slug = family_name(family)
    version = family_versions.get(family_slug, 0)
    family_versions[family_slug] = version + 1
    if version:
      family_slug += f'-palette-{version}'
    batch_pages = [(used, sheet_layout.pack(sheet_layout.panels(batch), size)) for batch, used in batches]
    single_sheet = len(batch_pages) == 1 and len(batch_pages[0][1]) == 1
    for batch_number, (used, pages) in enumerate(batch_pages):
      display_mode, display_palette, reserved, (purple, cyan, background, ink) = decoration(mode, palette, used)
      for page_number, page in enumerate(pages):
        name = family_slug if single_sheet else f'{family_slug}-{batch_number:02d}-{page_number:02d}'
        if name in index['sheets']:
          raise ValueError('Sheet filename collision: ' + name)
        width, height = page['width'], page['height']
        if width * height > MAX_PIXELS:
          raise ValueError('Sprite sheet exceeds the pixel limit')
        sheet = Image.new(display_mode, (width, height), background)
        if display_mode == 'P':
          sheet.putpalette(display_palette)
        draw = ImageDraw.Draw(sheet)
        for panel, x, y in page['panels']:
          draw_label(sheet, x + 1, y + 1, panel['label'], font, ink)
          draw.rectangle((x, y + 7, x + panel['width'] - 1, y + panel['height'] - 1), outline=purple)
          for line_x, top, bottom in panel['verticals']:
            draw.line((x + line_x, y + top, x + line_x, y + bottom), fill=cyan)
          for line_y in panel['horizontals']:
            draw.line((x + 1, y + line_y, x + panel['width'] - 2, y + line_y), fill=cyan)
          for item, local_x, local_y in panel['placements']:
            px, py = x + local_x, y + local_y
            with Image.open(item['original']) as image:
              if mode == 'L':
                image = Image.frombytes('P', image.size, image.tobytes())
              sheet.paste(image, (px, py))
            if panel['font']:
              draw_label(sheet, px, py - 6, item['label'], font, ink)
            index['sprites'][item['key']] = dict(sheet=name, x=px, y=py, width=item['width'], height=item['height'],
                group=item['group'], label=item['label'], description=item['description'],
                row=item['row'], column=item['column'])
        filename = 'sheets/' + name + '.png'
        sheet.save(root / filename, format='PNG', optimize=False)
        index['sheets'][name] = dict(file=filename, width=width, height=height, mode=display_mode,
            data_mode=mode, border_indices=reserved)
        sheet.close()
  write_json(output / 'sprites.json', index)
  for path, meta in documents:
    write_json(path, meta)
  for path in originals:
    path.unlink()
  print(f'Packed {len(originals)} images into {len(index["sheets"])} grouped, labeled sprite sheets')
  return dict(sprites=len(originals), sheets=len(index['sheets']))
