"""Original overview palette composition, verified against the English DOS executable."""
from .common import sha256, bounded

EXE_HASH = '9cce82355b466fc0e47c51ae1de4ba850a666b5f12c3831ee19ba5c4c4ef6add'
TABLES = {'owners': (448572, 18, 'a135f3fd3832b979a1a17a2a2c8c98f42f1ccae4f10b8ebcab8096e740302db0'),
    'terrain': (448652, 256, '8a5fedeaee29ac3ce653befdf2bf18aaac4453b263398e6b18a0f396c7ba314d')}


def map_palettes(installation):
  palette_file = next((p for p in installation.rglob('*') if p.is_file() and
      p.relative_to(installation).as_posix().lower() == 'gfx/palette.e'), None)
  if palette_file is None:
    return {}
  data = palette_file.read_bytes()
  if len(data) < 63 * 768:
    return {}
  executable = next((p for p in installation.iterdir() if p.is_file() and
      p.name.lower() in ('pt.exe', 'pt_original.exe') and sha256(p.read_bytes()) == EXE_HASH), None)
  if executable is None:
    raise ValueError('Correct map viewing palettes require the verified English DOS V2.0 executable')
  original = executable.read_bytes()
  tables = {}
  for name, (offset, size, checksum) in TABLES.items():
    raw = bounded(original, offset, size)
    if sha256(raw) != checksum:
      raise ValueError('Original map color lookup differs: ' + name)
    tables[name] = raw
  bank = bounded(data, 62 * 768, 768)
  if any(value > 63 for value in bank):
    raise ValueError('Invalid original map DAC palette')
  rgb = [bytes(component * 4 for component in bank[index * 3:index * 3 + 3]) for index in range(256)]
  # terrain.c DrawMapAnyZoom(1) transposes the tile nibbles, then selects bank 62.
  terrain = b''.join(rgb[tables['terrain'][16 * (tile & 15) + (tile >> 4)]] for tile in range(256))
  # map.c GetBezColor(0) renders ownership from the low nibble.
  district = b''.join(rgb[tables['owners'][tile & 15]] for tile in range(256))
  country = bounded(data, 2 * 768, 768)
  return {'terrain': terrain, 'district': district, 'bank62': b''.join(rgb),
      'country': bytes(component * 4 for component in country)}
