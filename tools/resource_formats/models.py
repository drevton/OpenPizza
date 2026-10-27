"""Keep wire-layout scaffolding in the tools, rather than repeating it in asset tables."""
from . import records


class InlineArray(list):
  """A fixed wire array belongs in its owning record's columns."""


WIRE_FIELDS = {
    'rectangles': {'rectangles': ('SpriteRect', True, None)},
    'floor-patterns': {'patterns': ('FloorPattern', True, None)},
    'high-scores': {'scores': ('HighScoreRecord', True, None)},
    'missions': {'columns': ('PTMissionGoals', False, None)},
    'character': {'character': ('Character', False, None)},
    'recipe': {'header': ('PTSavedRecipe', False, 292), 'entries': ('RecipeEntry', True, None)},
    'city': {'town': ('PTSavedTown', False, None), 'zones': ('PTSavedZone', True, None),
      'suppliers': ('PTSavedSupplier', True, None)},
    'restaurant-template': {'floor': ('RestaurantFloorState', False, None),
      'furniture': ('Furniture', True, None), 'ovens': ('Furniture', True, None)},
    'preferences': {'preferences': ('PTPreferencesFileImage', False, None)},
    'sound-config': {'configuration': ('PTSoundConfiguration', False, None)}}

EMPTY_TAIL_CODECS = {'bob', 'font', 'cell-map', 'character', 'recipe', 'city', 'restaurant-template',
    'floor-patterns', 'missions', 'preferences', 'sound-config', 'staff-market', 'sound-bank', 'driver-catalog', 'voc'}


def defaults(codec):
  if codec == 'vga':
    return dict(compressed_tail_hex='', file_tail_hex='')
  return dict(tail_hex='') if codec in EMPTY_TAIL_CODECS else {}


def simplify_wire(ctype, value):
  schema = records.resolve(ctype)
  if schema['kind'] == 'record':
    if value['type'] != ctype:
      raise ValueError('Unexpected wire record type: ' + value['type'])
    result = {field['name']: simplify_wire(field['type'], value['fields'][field['name']])
        for field in schema['fields'] if field['name'] in value['fields']}
    padding = [item for item in value['padding'] if any(bytes.fromhex(item['hex']))]
    if padding:
      result['$padding'] = padding
    return result
  if schema['kind'] == 'array':
    if isinstance(value, dict) and 'cp437' in value:
      return value['cp437']
    return InlineArray(simplify_wire(schema['element'], item) for item in value)
  return value


def expand_wire(ctype, value, extent=None):
  schema = records.resolve(ctype)
  if schema['kind'] == 'record':
    size = schema['size'] if extent is None else extent
    result = records.decode(ctype, bytes(size), extent=size)
    for field in schema['fields']:
      if field['name'] in result['fields']:
        result['fields'][field['name']] = expand_wire(field['type'], value[field['name']])
    if '$padding' in value:
      for item in value['$padding']:
        original = next((pad for pad in result['padding'] if pad['offset'] == item['offset']), None)
        if original is None or len(original['hex']) != len(item['hex']):
          raise ValueError('Unexpected wire padding extent')
        original['hex'] = item['hex']
    return result
  if schema['kind'] == 'array':
    if isinstance(value, str):
      return dict(cp437=value)
    if len(value) != schema['count']:
      raise ValueError('Fixed wire array length changed: ' + ctype)
    return [expand_wire(schema['element'], item) for item in value]
  return value


def simplify_document(value):
  if not isinstance(value, dict) or 'codec' not in value:
    return value
  result = {key: item for key, item in value.items()
      if key not in ('format', 'note', 'palette_note', 'semantic_layout', 'reason')}
  if value.get('format', 1) != 1:
    raise ValueError('Unsupported resource document format')
  for field, (ctype, multiple, unused_extent) in WIRE_FIELDS.get(value['codec'], {}).items():
    if field in value:
      result[field] = [simplify_wire(ctype, item) for item in value[field]] if multiple \
          else simplify_wire(ctype, value[field])
  for field, default in defaults(value['codec']).items():
    if result.get(field) == default:
      result.pop(field, None)
  if value['codec'] == 'font' and 'spare' in result:
    result['glyphs'] = [{key: item for key, item in glyph.items() if key != 'image' or item is not None}
        for glyph in result['glyphs']] + result.pop('spare')
  return result


def expand_document(value):
  if not isinstance(value, dict) or 'codec' not in value:
    return value
  # Legacy tables already contain their wire models.
  if 'format' in value:
    return value
  result = dict(defaults(value['codec']), **value, format=1)
  if value['codec'] == 'font' and 'glyphs' in value and 'spare' not in value:
    result['glyphs'] = [dict({'image': None}, **item) for item in value['glyphs'] if item.get('codepoint') is not None]
    result['spare'] = [{key: item for key, item in glyph.items() if key != 'codepoint'}
        for glyph in value['glyphs'] if glyph.get('codepoint') is None]
  for field, (ctype, multiple, extent) in WIRE_FIELDS.get(value['codec'], {}).items():
    if field in value:
      result[field] = [expand_wire(ctype, item, extent) for item in value[field]] if multiple \
          else expand_wire(ctype, value[field], extent)
  return result
