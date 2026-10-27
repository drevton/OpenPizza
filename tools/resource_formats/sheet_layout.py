"""Atomic related-frame panels and compact, portrait-biased rectangle packing."""
from collections import defaultdict
import math


def panels(items):
  groups = defaultdict(list)
  if items and items[0]['is_font']:
    for index, item in enumerate(sorted(items, key=lambda item: item['key'])):
      item['group'], item['row'], item['column'] = 'font glyphs', index // 16, index % 16
  for item in items:
    groups[item['group']].append(item)
  result = []
  for label, members in groups.items():
    rows = defaultdict(list)
    for item in members:
      rows[item['row']].append(item)
    font = members[0]['is_font']
    slot = max(max(item['width'], len(item['label']) * 4 - 1) for item in members) if font else 0
    placements, verticals, horizontals, y, width = [], [], [], 0, 0
    for row_number in sorted(rows):
      row = sorted(rows[row_number], key=lambda item: (item['column'], item['key']))
      height = max(item['height'] for item in row) + (6 if font else 0)
      x = 0
      for number, item in enumerate(row):
        placements.append((item, x + 1, y + 8 + (6 if font else 0)))
        x += slot if font else item['width']
        if number + 1 < len(row):
          verticals.append((x + 1, y + 8, y + 8 + height - 1))
          x += 1
      width = max(width, x)
      y += height
      if row_number != max(rows):
        horizontals.append(y + 8)
        y += 1
    width = max(width, len(label) * 4 - 1)
    result.append(dict(label=label, width=width + 2, height=y + 9, placements=placements,
        verticals=verticals, horizontals=horizontals, rank=min(item['rank'] for item in members), font=font))
  return result


def contains(outer, inner):
  x, y, width, height = outer
  a, b, w, h = inner
  return x <= a and y <= b and a + w <= x + width and b + h <= y + height


def one_page(items, width, height):
  free, placed, remaining = [(0, 0, width, height)], [], []
  for item in sorted(items, key=lambda item: (-item['height'], -item['width'], item['rank'], item['label'])):
    w, h = item['width'] + 2, item['height'] + 2
    options = [rect for rect in free if w <= rect[2] and h <= rect[3]]
    if not options:
      remaining.append(item)
      continue
    x, y, unused_w, unused_h = min(options, key=lambda rect: (rect[1] + h, rect[0], rect[2] * rect[3]))
    placed.append((item, x, y))
    revised = []
    for a, b, rw, rh in free:
      if a >= x + w or b >= y + h or a + rw <= x or b + rh <= y:
        revised.append((a, b, rw, rh))
        continue
      if a < x:
        revised.append((a, b, x - a, rh))
      if x + w < a + rw:
        revised.append((x + w, b, a + rw - x - w, rh))
      if b < y:
        revised.append((a, b, rw, y - b))
      if y + h < b + rh:
        revised.append((a, y + h, rw, b + rh - y - h))
    revised = list(dict.fromkeys(revised))
    free = [rect for index, rect in enumerate(revised)
        if not any(index != other and contains(candidate, rect) for other, candidate in enumerate(revised))]
  return placed, remaining


def pack(items, maximum_width=1024):
  if not items:
    return []
  area = sum((item['width'] + 2) * (item['height'] + 2) for item in items)
  minimum = max(item['width'] + 2 for item in items)
  limit = max(minimum, maximum_width)
  ideal = math.sqrt(area * 0.7)
  candidates = {minimum, limit}
  for factor in (0.65, 0.8, 1.0, 1.2):
    candidates.add(max(minimum, min(limit, math.ceil(ideal * factor))))
  best = None
  for width in sorted(candidates):
    remaining, pages = list(items), []
    height = max(1536, max(item['height'] + 2 for item in items))
    while remaining:
      placed, remaining = one_page(remaining, width, height)
      if not placed:
        raise ValueError('Related sprite panel cannot fit on a sheet')
      w = max(x + item['width'] for item, x, y in placed)
      h = max(y + item['height'] for item, x, y in placed)
      pages.append(dict(width=w, height=h, panels=placed))
    score = sum(page['width'] * page['height'] *
        (1 + 0.12 * abs(math.log(page['width'] / page['height'])) +
          (0.25 if page['width'] > page['height'] * 1.2 else 0)) for page in pages)
    score *= 1 + 0.12 * (len(pages) - 1)
    if best is None or score < best[0]:
      best = (score, pages)
  return best[1]
