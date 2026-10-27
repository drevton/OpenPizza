"""Indexed images, sprite sets, bitplane fonts and palette banks."""
import struct
from .common import bounded, png_image, png_read, png_write, save_hex, load_hex, unpack_fields, pack_fields, MAX_PIXELS

FONT_HEADER = [('height', 'H'), ('field_2', 'B'), ('spacing_mode', 'B'), ('fixed_advance', 'H'),
    ('baseline', 'h'), ('first_char', 'B'), ('last_char', 'B'), ('unused_rows', '4B'),
    ('row_stride', 'H'), ('unused_buffers', '8B')]


def rle_decode(data, expected):
  pixels, packets, at = bytearray(), [], 0
  while True:
    control = bounded(data, at, 1)[0]
    at += 1
    if control == 128:
      break
    count = (control & 127) + 1
    if len(pixels) + count > expected:
      raise ValueError('Compressed bitmap exceeds its declared dimensions')
    packets.append(control)
    if control & 128:
      pixels.extend(bounded(data, at, 1) * count)
      at += 1
    else:
      pixels.extend(bounded(data, at, count))
      at += count
  if len(pixels) != expected:
    raise ValueError('Compressed bitmap does not fill its dimensions')
  return bytes(pixels), packets, data[at:]


def rle_encode(pixels):
  """Native CompressBackground packet rules: triples start runs, capped at 128 pixels.

  Lookahead is bounded to the logical input, avoiding the native routine's two-byte
  overread. Verified against every compressed image in the English DOS V2.0 set.
  """
  output, at = bytearray(), 0
  while at < len(pixels):
    run = 1
    while at + run < len(pixels) and run < 128 and pixels[at + run] == pixels[at]:
      run += 1
    if run >= 3:
      output.extend((128 | (run - 1), pixels[at]))
      at += run
      continue
    start = at
    at += 1
    while at < len(pixels) and at - start < 128:
      if at + 2 < len(pixels) and pixels[at] == pixels[at + 1] == pixels[at + 2]:
        break
      at += 1
    output.append(at - start - 1)
    output.extend(pixels[start:at])
  return bytes(output) + b'\x80'


# Compatibility for callers of the earlier utility API.
rle_canonical = rle_encode


def rle_original(pixels, packets, allow_edits):
  output, at = bytearray(), 0
  for control in packets:
    if not isinstance(control, int) or not 0 <= control <= 255 or control == 128:
      raise ValueError('Invalid preserved RLE packet')
    count = (control & 127) + 1
    part = bounded(pixels, at, count)
    if control & 128 and len(set(part)) != 1:
      if allow_edits:
        return rle_encode(pixels)
      raise ValueError('Edited PNG breaks an original repeated packet; use --allow-edits to recompress')
    output.append(control)
    output.extend(part[:1] if control & 128 else part)
    at += count
  if at != len(pixels):
    raise ValueError('Original RLE packet lengths do not cover the image')
  return bytes(output) + b'\x80'


def decode_vga(data, root, palette):
  width, height, compressed = struct.unpack('<HHI', bounded(data, 0, 8))
  if not width or not height or width * height > MAX_PIXELS:
    raise ValueError('Invalid VGA dimensions')
  body = bounded(data, 8, compressed or width * height)
  if compressed:
    pixels, packets, remainder = rle_decode(body, width * height)
  else:
    pixels, packets, remainder = body, None, b''
  encoding = 'raw' if not compressed else 'native-rle'
  if compressed and rle_encode(pixels) + remainder != body:
    encoding = 'legacy-packets'
  result = dict(codec='vga', image=png_write(root, 'image.png', width, height, pixels, palette),
      encoding=encoding, compressed_tail_hex=remainder.hex(),
      file_tail_hex=data[8 + len(body):].hex(),
      palette_note='Pixel indices are authoritative; PLTE is a viewing palette.')
  if encoding == 'legacy-packets':
    result['packets'] = packets
  return result


def encode_vga(meta, root, allow_edits):
  with png_image(root, meta['image']) as image:
    pixels, width, height = image.tobytes(), image.width, image.height
  encoding = meta.get('encoding')
  if encoding == 'native-rle':
    body, compressed = rle_encode(pixels), True
  elif encoding == 'raw':
    body, compressed = pixels, False
  elif encoding in (None, 'legacy-packets'):
    compressed = meta['packets'] is not None
    body = rle_original(pixels, meta['packets'], allow_edits) if compressed else pixels
  else:
    raise ValueError('Unknown VGA encoding: ' + str(encoding))
  if compressed:
    body += bytes.fromhex(meta['compressed_tail_hex'])
  return struct.pack('<HHI', width, height,
      len(body) if compressed else 0) + body + bytes.fromhex(meta['file_tail_hex'])


def decode_bob(data, root, palette):
  count, opaque = struct.unpack('<HI', bounded(data, 0, 6))
  if count > 10000:
    raise ValueError('Unreasonable sprite count')
  at, frames = 6 + 7 * count, []
  bounded(data, 0, at)
  for index in range(count):
    width, height, flags, unused_pointer = struct.unpack('<BBBI', data[6 + 7 * index:13 + 7 * index])
    size = width * height
    pixels = bounded(data, at, size)
    image = png_write(root, f'frame-{index:04d}.png', width, height, pixels, palette) if size else None
    frames.append(dict(width=width, height=height, flags=flags,
        unused_pointer=unused_pointer, image=image))
    at += size
  return dict(codec='bob', opaque=opaque, frames=frames, tail_hex=data[at:].hex())


def encode_bob(meta, root):
  headers, payloads = bytearray(), bytearray()
  for frame in meta['frames']:
    if frame['image']:
      with png_image(root, frame['image']) as image:
        pixels, width, height = image.tobytes(), image.width, image.height
      if frame.get('width', width) != width or frame.get('height', height) != height:
        raise ValueError('Sprite dimensions differ')
      payloads.extend(pixels)
    else:
      width, height = frame['width'], frame['height']
      if width * height:
        raise ValueError('Missing sprite PNG')
    headers.extend(struct.pack('<BBBI', width, height, frame['flags'], frame['unused_pointer']))
  return struct.pack('<HI', len(meta['frames']), meta['opaque']) + headers + payloads + bytes.fromhex(meta['tail_hex'])


def decode_font(data, root):
  header = unpack_fields(bounded(data, 0, 24), FONT_HEADER)
  height, stride = header['height'], header['row_stride']
  count = header['last_char'] - header['first_char'] + 2
  if count <= 0 or height * stride * 8 > MAX_PIXELS:
    raise ValueError('Invalid font dimensions')
  bits = bounded(data, 24, height * stride)
  pixels = bytes(255 if byte & (128 >> bit) else 0 for byte in bits for bit in range(8))
  at = 24 + len(bits)
  glyphs = []
  for index in range(count):
    bit_x, ink_width = struct.unpack('<HH', bounded(data, at, 4))
    glyphs.append(dict(codepoint=header['first_char'] + index, bit_x=bit_x, ink_width=ink_width))
    at += 4
  if header['spacing_mode'] != 2:
    for glyph in glyphs:
      glyph['advance'] = struct.unpack('<H', bounded(data, at, 2))[0]
      at += 2
  from PIL import Image
  atlas = Image.frombytes('L', (stride * 8, height), pixels)
  covered, shared = bytearray(stride * 8), {}
  for glyph in glyphs:
    x, width = glyph['bit_x'], glyph['ink_width']
    if x + width > atlas.width:
      raise ValueError('Font glyph exceeds its bitplane')
    glyph['image'] = None
    if width:
      key = (x, width)
      if key not in shared:
        shared[key] = png_write(root, f'glyph-{glyph["codepoint"]:04d}.png', width, height,
            atlas.crop((x, 0, x + width, height)).tobytes(), mode='L')
      glyph['image'] = dict(shared[key])
      covered[x:x + width] = b'\1' * width
  spare, x = [], 0
  while x < atlas.width:
    if covered[x]:
      x += 1
      continue
    end = x + 1
    while end < atlas.width and not covered[end]:
      end += 1
    spare.append(dict(bit_x=x, image=png_write(root, f'unused-{x:04d}.png', end - x, height,
        atlas.crop((x, 0, end, height)).tobytes(), mode='L')))
    x = end
  return dict(codec='font', header=header, glyphs=glyphs, spare=spare, tail_hex=data[at:].hex(),
      note='Glyphs wrap into a dedicated sheet; unused bitplane columns are explicit spare rectangles.')


def encode_font(meta, root):
  header = meta['header']
  widths = {}
  if 'atlas' in meta:
    with png_image(root, meta['atlas']) as image:
      if image.size != (header['row_stride'] * 8, header['height']):
        raise ValueError('Font atlas dimensions differ from header')
      pixels = image.tobytes()
  else:
    width, height = header['row_stride'] * 8, header['height']
    pixels, covered = bytearray(width * height), bytearray(width * height)
    for item in meta['glyphs'] + meta['spare']:
      if not item['image']:
        continue
      with png_image(root, item['image']) as image:
        if image.height != height or item['bit_x'] + image.width > width:
          raise ValueError('Font glyph dimensions exceed the original bitplane')
        data = image.tobytes()
        widths[id(item)] = image.width
        for y in range(height):
          for x in range(image.width):
            destination = y * width + item['bit_x'] + x
            value = data[y * image.width + x]
            if covered[destination] and pixels[destination] != value:
              raise ValueError('Overlapping font glyphs disagree')
            pixels[destination], covered[destination] = value, 1
    if not all(covered):
      raise ValueError('Font bitplane has missing columns')
  if any(pixel not in (0, 255) for pixel in pixels):
    raise ValueError('Font atlas must use black/white pixels without antialiasing')
  bits = bytes(sum((pixels[at + bit] != 0) << (7 - bit) for bit in range(8)) for at in range(0, len(pixels), 8))
  glyphs = meta['glyphs']
  if len(glyphs) != header['last_char'] - header['first_char'] + 2:
    raise ValueError('Font glyph count differs')
  rows = b''.join(struct.pack('<HH', glyph['bit_x'], glyph.get('ink_width', widths.get(id(glyph), 0)))
      for glyph in glyphs)
  advances = b'' if header['spacing_mode'] == 2 else b''.join(struct.pack('<H', row['advance']) for row in glyphs)
  return pack_fields(header, FONT_HEADER) + bits + rows + advances + bytes.fromhex(meta['tail_hex'])


def decode_palettes(data, root):
  if len(data) % 768:
    raise ValueError('Palette bank is not a multiple of 256 RGB triples')
  banks = []
  for index in range(len(data) // 768):
    raw = data[index * 768:(index + 1) * 768]
    if any(value > 63 for value in raw):
      raise ValueError('Unsupported non-DAC palette component')
    from PIL import Image
    image = Image.frombytes('RGB', (16, 16), bytes(value * 4 for value in raw))
    name = f'palette-{index:03d}.png'
    image.save(root / name)
    banks.append(dict(file=name, width=16, height=16, mode='RGB'))
  return dict(codec='palettes', banks=banks, rgb_scale=4,
      note='Each RGB component is an exact VGA DAC value times four; preserve multiples of four.')


def encode_palettes(meta, root):
  output = bytearray()
  for bank in meta['banks']:
    reference = {'width': 16, 'height': 16, 'mode': 'RGB', **bank}
    if (reference['width'], reference['height'], reference['mode']) != (16, 16, 'RGB'):
      raise ValueError('Palette PNG must remain 16x16 RGB')
    pixels = png_read(root, reference)
    if any(value % 4 for value in pixels):
      raise ValueError('Palette RGB components must be exact multiples of four')
    output.extend(value // 4 for value in pixels)
  return bytes(output)
