#!/usr/bin/env python3
"""Round trip game graphics, sound and music without modifying the installation."""
import argparse
from collections import Counter
from contextlib import contextmanager
import os
from pathlib import Path
import re
import shutil
import stat
import struct
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
  sys.path.insert(0, str(ROOT))
try:
  from tools.resource_formats import codecs
except ModuleNotFoundError as error:
  if error.name == 'PIL':
    raise SystemExit('Install Pillow: python3 -m pip install -r tools/resource_formats/requirements.txt') from error
  raise
from tools.resource_formats.common import read_json, write_json, safe_path, sha256
from tools.resource_formats import sheets, flat, palettes, labels


def media_resource(name):
  """Select media containers and their palettes, including embedded portrait/map images."""
  path = Path(name.lower())
  return (path.suffix in ('.vga', '.bob', '.fnt', '.voc', '.cmf') or path.name == 'palette.e' or
      (path.parent.name == 'sound' and re.fullmatch(r'm\d+', path.name) is not None) or
      (path.parent.name == 'chr' and path.suffix == '.pc') or
      (path.parent.name == 'dat' and re.fullmatch(r'(?:map|bez)\d+\.dat', path.name) is not None))


def inventory(source, resources_only=False):
  if not source.is_dir():
    raise ValueError('Input installation must be a directory')
  paths, skipped = [], []
  pinned = None
  if resources_only:
    pinned = {name for name in re.findall(r'PT_GAME_DATA\("([^"]+)"',
        (ROOT / 'src/game_data_checksums.def').read_text()) if media_resource(name)}
  for path in sorted(source.rglob('*')):
    if path.is_symlink():
      raise ValueError('Symlink in input installation: ' + str(path))
    if not path.is_file():
      continue
    name = path.relative_to(source).as_posix()
    if not media_resource(name) or (pinned is not None and name.lower() not in pinned):
      skipped.append(name)
    else:
      paths.append((name, path))
  if pinned is not None:
    missing = pinned - {name.lower() for name, path in paths}
    if missing:
      raise ValueError('Missing required game resources: ' + ', '.join(sorted(missing)))
  if not paths:
    raise ValueError('No input files selected')
  return paths, skipped


def separate_input_output(source, target):
  if target.exists():
    raise ValueError('Output must be a new directory: ' + str(target))
  if source == target or source in target.parents or target in source.parents:
    raise ValueError('Input and output directories must be separate, without nesting')


@contextmanager
def fresh_output(target):
  if target.exists():
    raise ValueError('Output already exists: ' + str(target))
  target.parent.mkdir(parents=True, exist_ok=True)
  temporary = Path(tempfile.mkdtemp(prefix='.' + target.name + '-', dir=target.parent))
  try:
    yield temporary
    if target.exists():
      raise ValueError('Output appeared during conversion')
    temporary.rename(target)
  finally:
    if temporary.exists():
      shutil.rmtree(temporary)


def palette_from_installation(source, bank):
  palette = next((path for path in source.rglob('*') if path.is_file() and
      path.relative_to(source).as_posix().lower() == 'gfx/palette.e'), None)
  if palette is None:
    if bank != 0:
      raise ValueError('No game palette found for the requested palette bank')
    return None
  data = palette.read_bytes()
  if bank < 0 or (bank + 1) * 768 > len(data):
    raise ValueError('Palette bank does not exist')
  raw = data[bank * 768:(bank + 1) * 768]
  if any(value > 63 for value in raw):
    raise ValueError('Palette has non-DAC components')
  return bytes(value * 4 for value in raw)


def unpack(source, target, resources_only=False, palette_bank=0, individual_images=False,
    legacy_layout=False):
  separate_input_output(source, target)
  paths, skipped = inventory(source, resources_only)
  palette = palette_from_installation(source, palette_bank)
  map_colors = palettes.map_palettes(source)
  sprite_labels = labels.catalog_labels(source)
  entries, counts = [], Counter()
  with fresh_output(target) as temporary:
    export = temporary if legacy_layout or individual_images else temporary / '_work'
    export.mkdir(exist_ok=True)
    for name, path in paths:
      data = path.read_bytes()
      destination = safe_path(export, 'files/' + name)
      try:
        codec = codecs.decode(data, destination, name, palette)
        if codec == 'cell-map' and map_colors:
          from PIL import Image
          image_path = destination / 'cells.png'
          with Image.open(image_path) as image:
            image = Image.frombytes('P', image.size, image.tobytes()) if image.mode != 'P' else image.copy()
            image.putpalette(map_colors['terrain' if Path(name).stem.lower().startswith('map') else 'district'])
            image.save(image_path)
          meta = read_json(destination / 'resource.json')
          meta['image']['mode'] = 'P'
          meta['note'] = 'Original cell IDs with the composed game overview palette, bank 62.'
          meta['palette_bank'] = 62
          write_json(destination / 'resource.json', meta)
        if codec == 'vga' and name.lower() == 'gfx/euromap.vga' and map_colors:
          from PIL import Image
          image_path = destination / 'image.png'
          with Image.open(image_path) as image:
            image.putpalette(map_colors['country'])
            image.save(image_path)
        rebuilt = codecs.encode(destination)
        if rebuilt != data:
          index = next((i for i, pair in enumerate(zip(rebuilt, data)) if pair[0] != pair[1]),
              min(len(rebuilt), len(data)))
          raise ValueError(f'Immediate round trip differs at byte {index}')
      except (ValueError, KeyError, TypeError, struct.error, OSError) as error:
        raise ValueError(name + ': ' + str(error)) from error
      info = path.stat()
      entries.append(dict(path=name, codec=codec, size=len(data), sha256=sha256(data),
          mode=stat.S_IMODE(info.st_mode), mtime_ns=info.st_mtime_ns))
      counts[codec] += 1
    catalog = dict(format=1, files=entries, skipped_files=len(skipped), codecs=dict(sorted(counts.items())),
        palette_bank=palette_bank, pixel_mode='indexed PNGs preserve indices; external PLTE is a viewing context',
        identity='Every source byte is represented by editable media/text and explicit preservation metadata',
        warning='Original and converted resources remain proprietary; keep this directory out of public source history')
    write_json(export / 'catalog.json', catalog)
    if not individual_images:
      catalog['sprite_sheets'] = sheets.consolidate(export, catalog=sprite_labels)
      write_json(export / 'catalog.json', catalog)
      materialize(export)
    if export != temporary:
      flat.write_export(export, temporary)
      shutil.rmtree(export)
      materialize(temporary)
  print(f'Exported and immediately verified {len(entries)} files to {target}')
  print('Codecs: ' + ', '.join(f'{key}={value}' for key, value in sorted(counts.items())))
  return catalog


def package_catalog(source):
  catalog = read_json(safe_path(source, 'catalog.json'))
  if catalog.get('format') != 1:
    raise ValueError('Unsupported export catalog version')
  seen, folded = set(), set()
  for entry in catalog['files']:
    name = entry['path']
    safe_path(source, 'files/' + name)
    if name in seen or name.casefold() in folded:
      raise ValueError('Duplicate/case-colliding output path: ' + name)
    seen.add(name)
    folded.add(name.casefold())
  for name in seen:
    if any(str(parent) in seen for parent in Path(name).parents if str(parent) != '.'):
      raise ValueError('Output file also used as a directory: ' + name)
  return catalog


def materialize(source, allow_edits=False):
  with flat.package_sources(source), sheets.image_sources(source):
    return materialize_images(source, allow_edits)


def materialize_images(source, allow_edits):
  catalog = package_catalog(source)
  result, changed = [], []
  for entry in catalog['files']:
    name = entry['path']
    try:
      data = codecs.encode(safe_path(source, 'files/' + name), allow_edits)
    except (ValueError, KeyError, TypeError, struct.error, OSError) as error:
      raise ValueError(name + ': ' + str(error)) from error
    if len(data) != entry['size'] or sha256(data) != entry['sha256']:
      changed.append(name)
      if not allow_edits:
        raise ValueError('Resource was edited; refusing an EXACT reconstruction claim: ' + name +
            '. Use --allow-edits only when intentionally building modified files.')
    result.append((entry, data))
  return catalog, result, changed


def make_sheets(source, target):
  separate_input_output(source, target)
  package_catalog(source)
  # Validate the complete input before copying and modifying an export.
  verify(source)
  with fresh_output(target) as temporary:
    shutil.copytree(source, temporary, dirs_exist_ok=True)
    summary = sheets.consolidate(temporary)
    catalog = read_json(temporary / 'catalog.json')
    catalog['sprite_sheets'] = summary
    write_json(temporary / 'catalog.json', catalog)
    verify(temporary)
  print('Sprite sheet export: ' + str(target))


def flatten(source, target):
  separate_input_output(source, target)
  verify(source)
  with fresh_output(target) as temporary:
    flat.write_export(source, temporary)
    verify(temporary)
  print('Flat TSV export: ' + str(target))


def pack(source, target, allow_edits=False):
  separate_input_output(source, target)
  catalog, results, changed = materialize(source, allow_edits)
  with fresh_output(target) as temporary:
    for entry, data in results:
      output = safe_path(temporary, entry['path'])
      output.parent.mkdir(parents=True, exist_ok=True)
      output.write_bytes(data)
      output.chmod(entry['mode'] & 0o777)
      os.utime(output, ns=(entry['mtime_ns'], entry['mtime_ns']))
  print(f'Reconstructed {len(results)} files; {len(changed)} modified; output: {target}')
  for name in changed:
    print('Modified: ' + name)
  return dict(files=len(results), exact=len(results) - len(changed), changed=changed)


def verify(source, original=None):
  catalog, results, changed = materialize(source)
  if original:
    for entry, data in results:
      actual = safe_path(original, entry['path']).read_bytes()
      if actual != data:
        raise ValueError('Original directory differs from the export: ' + entry['path'])
  report = dict(status='passed exact byte-for-byte reconstruction', files=len(results),
      total_bytes=sum(len(data) for entry, data in results), codecs=catalog['codecs'],
      entries=[dict(path=entry['path'], size=len(data), sha256=sha256(data)) for entry, data in results])
  print(f'PASS: {len(results)} files reconstruct byte-for-byte with matching SHA-256 hashes')
  return report


def main():
  parser = argparse.ArgumentParser(description=__doc__)
  commands = parser.add_subparsers(dest='command', required=True)
  for name in ('unpack', 'roundtrip'):
    help_text = 'Export resources' if name == 'unpack' else 'Export, rebuild and verify'
    command = commands.add_parser(name, help=help_text)
    command.add_argument('source', type=Path)
    command.add_argument('--output', type=Path, required=True, help='New output directory, outside the input')
    command.add_argument('--game-resources-only', action='store_true',
        help='Require all media paths from the fixed runtime checksum manifest')
    command.add_argument('--palette-bank', type=int, default=0, help='Viewing palette for indexed images')
    command.add_argument('--individual-images', action='store_true', help='Use separate PNGs instead of sprite sheets')
    command.add_argument('--legacy-layout', action='store_true', help='Use the previous JSON directory layout')
  command = commands.add_parser('flatten', help='Convert a previous sheet export to flat TSV/media files')
  command.add_argument('source', type=Path)
  command.add_argument('--output', type=Path, required=True)
  command = commands.add_parser('sheets', help='Convert an existing individual-PNG export into labeled sprite sheets')
  command.add_argument('source', type=Path)
  command.add_argument('--output', type=Path, required=True)
  command = commands.add_parser('pack', help='Rebuild original game files from exported media/text')
  command.add_argument('source', type=Path)
  command.add_argument('--output', type=Path, required=True)
  command.add_argument('--allow-edits', action='store_true',
      help='Intentionally build modified, non-identical resources')
  command = commands.add_parser('verify', help='Verify exact reconstruction without writing original files')
  command.add_argument('source', type=Path)
  command.add_argument('--original', type=Path, help='Also compare against the installed source files')
  command.add_argument('--report', type=Path)
  args = parser.parse_args()
  source = args.source.resolve()
  if args.command == 'unpack':
    unpack(source, args.output.resolve(), args.game_resources_only, args.palette_bank,
        args.individual_images, args.legacy_layout)
  elif args.command == 'roundtrip':
    output = args.output.resolve()
    separate_input_output(source, output)
    with fresh_output(output) as temporary:
      unpack(source, temporary / 'readable', args.game_resources_only, args.palette_bank,
          args.individual_images, args.legacy_layout)
      pack(temporary / 'readable', temporary / 'rebuilt')
      report = verify(temporary / 'readable', source)
      write_json(temporary / 'report.json', report)
    print('Complete round-trip report: ' + str(output / 'report.json'))
  elif args.command == 'pack':
    pack(source, args.output.resolve(), args.allow_edits)
  elif args.command == 'sheets':
    make_sheets(source, args.output.resolve())
  elif args.command == 'flatten':
    flatten(source, args.output.resolve())
  else:
    report = verify(source, args.original.resolve() if args.original else None)
    if args.report:
      write_json(args.report, report)


if __name__ == '__main__':
  try:
    main()
  except (OSError, ValueError, KeyError, TypeError, struct.error) as error:
    raise SystemExit('resources: ' + str(error))
