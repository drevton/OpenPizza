# Reversible resource conversion

`tools/resources.py` exports installed Pizza Tycoon files as labeled PNG sheets,
WAV and music **TSV tables**. It rebuilds the selected original media files
without the original installation. Every export is immediately reconstructed and
compared byte for byte; strict packing checks every original size and SHA-256
before creating output.

The tested edition is **Pizza Tycoon English DOS V2.0**, the edition required by
the native checksum manifest. The media export contains 335 files: 51 BOB sprite
sets, 145 VGA images, six fonts, one palette file, 100 portrait containers,
20 map images and 12 sound/music banks. The native runtime separately verifies
495 fixed game files; a media export is not a complete playable installation.
This validates resource conversion; it does not establish complete gameplay
parity or compatibility with another edition.

## Current assets and layout

The private media export is at the repository root in `assets/`:

```text
assets/
  resources.tsv          Original filenames, codecs, sizes, hashes, modes and timestamps
  tables.tsv             Layout version and index of purpose-specific record tables
  graphics/
    moebel*.png          Furniture sprites; other BOB sets have their own sheets
    meny-font.png        Menu font; each font has its own glyph grid, wrapped at 16 columns
    map-0.png ...        One complete town map per PNG, with the game's overview palette
    district-0.png ...   One district map per PNG, with ownership colors
    europe-map.png      Country selection map, palette bank 2
    sprites.tsv          Sheet headers followed by sprite rectangles, BOB headers, names and descriptions
    portraits.tsv        Embedded fields required to rebuild portrait containers
    maps.tsv             Map PNG references and any trailing bytes
  audio/
    *.wav                PCM audio blocks
    *events.tsv          One row per music event, with named columns
    *.hex                Original CMF header gaps needed for exact reconstruction
```

There are no per-resource directories, `resource.json` files, JSON/JSONL data
files or individual sprite PNGs in this layout. Source names in TSV identify the
original files to recreate; they do not refer to separate images on disk.

In `graphics/sprites.tsv`, the first column is `id`: a complete identifier such
as `gaeste:177`. Rows and references in the other tables use the same
`category:id` form, with no separate category column, source filename or `#`
separator. Fonts use IDs such as `meny-font:143`, and
unused font regions use IDs such as `meny-font:unused-0996`. Glyph aliases retain
their shared reference. The same local ID may occur in different categories.

Core columns are `id`, `x`, `y`, `width`, `height`, `flags`,
`unused_pointer`, `name`, `description`, `mode`, `data_mode`, `border_indices`,
`bit_x`, `advance` and `alias` columns. Each BOB frame's rectangle, header data and labels occupy one row.
The frame part of a BOB ID is an ordinary decimal number starting at zero,
without padding, as in `gaeste:0`.
Their original output resources are resolved through the category and the
existing resource catalog, including groups split over several sheet pages.
Unknown or ambiguous categories, duplicate frames across pages, and gaps in a
frame sequence are rejected. No original resource filename is repeated in the
sprite ID. Empty frames retain their category and header dimensions, with blank
`x`/`y` cells; an entirely empty frame set needs no PNG.

A row whose `id` is just a category, such as `gaeste`, defines a sheet.
`width`/`height` are the complete sheet dimensions, and `mode`, `data_mode` and
`border_indices` describe its pixels and decoration. The PNG path is derived as
`graphics/<sheet>.png`; there is no redundant filename column. Shared settings
occur once per sheet, followed by its sprite rows. The reader resolves sheet
definitions before sprites, so reordered rows remain valid. Duplicate IDs within
one category, missing sheet definitions, conflicting settings on sprite rows,
and PNG dimensions or modes that disagree with a header are rejected.
Older exports with separate sprite/category columns, global IDs, separate sheet
fields in references, or separate frame/label/sheet tables remain readable.
BOB file headers use `bob_opaque` and `bob_tail_hex` on the primary sheet row.
An entirely empty BOB set has a resource-only header with blank canvas settings
and no PNG. VGA image rows carry `vga_encoding` (`raw` or `native-rle`),
`vga_compressed_tail_hex` and `vga_file_tail_hex`. The standalone Europe map and
person panel have full-image rectangles in this table and retain their existing
PNGs. Original output resources are resolved through the resource catalog and
category/ID rather than repeated source filenames.

Palette banks use numeric IDs such as `palettes:0`; their order comes from those
IDs, and the encoder restores the original six-bit DAC values from RGB swatches.
Missing or duplicate bank indices are rejected. The exporter and packer consume
all these graphics records directly in `sprites.tsv`; no forwarding graphics
metadata tables are needed.

Single-sheet groups use the group name directly, such as `moebel.png`.
BOB sheet filenames use the source basename without the `bob-`/`-bob` wrappers.
Font sheets use `<name>-font.png`, including `meny-font.png` for `MENU.FNT`.
Groups requiring several sheets retain `-BB-PP` batch/page counters; palette
variants retain their distinct `-palette-N` names.

Sheets group related artwork rather than mixing all assets together: each BOB
sprite set, font and portrait collection has its own group. Within a sheet,
related rotations, directions and animation phases form atomic labeled panels.
Frames touch with exactly one cyan separator pixel between neighbors; a panel
has one purple outline, with its short human label above. Image captions contain
no filenames or paths. Each sprite row carries its own name and description;
there is no `labels.tsv`, label-group forwarding record or frame-range list.
For example, `zutaten:299` has name `mozzarella` and a blank description.
Related frames repeat the shared name so each row can be understood on its own.
Descriptions are included only when they add information, such as a chair's
catalog model name. Unnamed sprites have blank label cells. Row/column values
are inferred from sheet coordinates rather than stored again. Font and palette labels share the sprite table.

Furniture labels such as `chair 0`, `table 0` and `oven 0` come from the original
44-byte `FurnitureDesc` records and their four image slots. Ingredient groups
come from the 57-byte `Ingredient` records' preparation/rotation image slots.
The catalogs are extracted only from the verified English DOS executable.
Guest groups use the native formulas in `restaurant.c` and `restaurant_scroll.c`:
four directions in each of seven body/pose bands stay with the same guest.
Weapon and attacker animations follow `interactive_attack.c`. Unidentified
sprites receive neutral numbered captions rather than invented identities.

Panels stay intact while the packer searches sheet widths for dense layouts,
favoring square or portrait pages over excessively wide strips. A large group
can span several pages. The six fonts have six separate sheets; each glyph has a
hexadecimal codepoint label and rows wrap at 16 slots. Original glyph aliases
share their original pixels, and small spare rectangles preserve unused bitplane
columns. All per-glyph metrics now live in `sprites.tsv`: `bit_x` records the
original bitplane position, and `advance` records variable character spacing.
The codepoint comes from the numeric ID, so no duplicate codepoint or record
index is needed. A shared glyph has an `alias` such as `meny-font:143`, with blank
rectangle columns. A blank glyph has width zero, the font height, its original
metrics and no rectangle. Unused bitplane regions keep their own `unused-N` rows
with bit positions and rectangles. Alias cycles, missing targets, missing glyphs
and conflicting rectangles are rejected. Neither `fonts-glyphs.tsv` nor
`fonts-spare.tsv` is exported. Font file headers also live on their sheet header
rows, in `font_*` columns: height, baseline, character range, spacing, bitplane
stride and the original unused byte arrays. They occur once per font, even when
its glyphs span several sheets. `font_tail_hex` preserves trailing bytes. There
is no `fonts.tsv`; all font file and glyph data is in `sprites.tsv`. The exporter
generates this layout automatically, and the packer reads it directly.

Packing loads the sheets once, crops rectangles in memory and writes original
resource bytes directly. It creates no temporary sprite PNGs, reconstructed JSON
files or per-resource working directories. Borders and labels are outside the
recorded rectangles and never enter game data. Sheets retain all original pixel
indices, including duplicate palette colors; sprites are not trimmed, rotated or
quantized. Labels use an independently drawn built-in 3x5 ASCII font.

The sheet canvas is white, captions are black, related frames share single cyan
separators, and panels have only a one-pixel purple outline. Purple and cyan are
absent from the group's original pixel colors. Indexed sheets reserve four
unused indices for purple, cyan, white and black, recoloring only unused entries.
Palette batches split at whole panel boundaries, preserving related sets.
Font sheets use indexed PNGs with grayscale glyph pixels and colored decoration;
`data_mode=L` restores the original grayscale samples. Painting a reserved
decoration index inside a sprite is rejected, including in edit mode.

`assets/` is ignored by Git because it contains original game content. It must
remain private. Rebuilding it does not need the original installation, the label
font, any files under `build/`, a compiler or the game binary.

## Commands

Use Python 3.10 or newer and Pillow:

```sh
python3 -m pip install -r tools/resource_formats/requirements.txt
python3 tools/resources.py pack assets --output build/restored-game-data
python3 tools/resources.py verify assets --report build/assets-report.json
```

To create and verify a fresh export:

```sh
python3 tools/resources.py roundtrip PIZZA --output build/resources-check
```

That batch produces `readable/`, `rebuilt/` and `report.json`. Separate steps are:

```sh
python3 tools/resources.py unpack PIZZA --output build/resources-readable
python3 tools/resources.py pack build/resources-readable --output build/resources-restored
python3 tools/resources.py verify build/resources-readable --original PIZZA
```

Outputs must be new directories, outside the input. Failed operations leave no
partial output; the installation and export are never overwritten.

Export selects graphics (`.BOB`, `.VGA`, `.FNT` and `PALETTE.E`), character
portrait containers (`CHR/*.PC`), map images (`DAT/MAP*.DAT`, `DAT/BEZ*.DAT`),
sound/music banks (`SOUND/M0`, etc.) and standalone `.VOC`/`.CMF` files. It skips
DOS archives, executables, installers, drivers, saves, recipes, town records,
missions, settings, text and analysis files. There is no `--all-files` mode.
`--game-resources-only` additionally requires all media paths from the runtime
checksum manifest; it does not export or require the other game files.

Portrait containers mix pixels with character fields. Their original header and
embedded fields remain in `graphics/portraits.tsv` solely to reconstruct those
image-bearing files exactly. Other metadata contains media headers, palettes,
font metrics, audio controls and music instruments needed for the same purpose.
The exporter does not create standalone gameplay catalogs.

Older exports remain readable by `pack` and `verify`. To convert an existing
JSON sheet export into this flat layout:

```sh
python3 tools/resources.py flatten /path/to/old-export --output build/flat-assets
```

For older individual-PNG exports, first use `resources sheets INPUT --output NEW`
to consolidate images, then `resources flatten`. `--legacy-layout` explicitly
requests the old JSON layout on export; `--individual-images` additionally
requests separate PNGs. Neither is needed for normal use.

## TSV editing

`resources.tsv` and `graphics/sprites.tsv` have ordinary
named columns. There is no monolithic metadata file. Glyph metrics share the
sprite table. Music also has one event
per row, with named columns such as delta, channel, note and velocity. Row order
defines music event order; there is no redundant event-number column. Identical
event streams share one TSV, referenced by each track's row in `audio/music.tsv`.
Editing a shared stream changes all tracks referencing it; copy the file and
change one track's `events` field to edit that track independently.

Other media tables identify their source with `resource`; repeated entries also
have an `index`. Field names and types occur once in the column headers, with one
record per row. `tables.tsv` lists the tables and their relationships. Original
portrait wire layouts are supplied by the tool's checked-in schema instead of
being repeated in assets. Blank cells mean absent fields; in typed columns,
`:` represents an empty value and a literal leading colon is doubled.

The current private media export has 17 TSV files. Earlier redundant files were
removed from the previous layout: `package.tsv` (the version is now a declaration
in `tables.tsv`), `fonts-spare.tsv` (merged into glyphs), and two identical music
event tables (shared by reference). `frames.tsv`, `labels.tsv` and `sheets.tsv`
are now merged into `sprites.tsv`, along with `fonts-glyphs.tsv`, `fonts.tsv`,
`sprite-sets.tsv`, `palettes.tsv` and `images.tsv`.
The remaining reconstruction tables contain
resource data, sheet locations, original checksums and relationships between
records. The `name` and `description` columns in `sprites.tsv` are optional
annotations: editing them does not change rebuilt game data or redraw existing
PNG captions. Borders and captions remain visible in the PNGs, while the TSV
retains editable names and descriptions.

Tabs, newlines, carriage returns, backslashes and control bytes inside values
use `\t`, `\n`, `\r`, `\\`, and `\xNN` escapes, respectively. This preserves
CP437 fixed-width strings, mixed line endings and unused text bytes while keeping
one physical line per row. Field-name path components escape `~` as `~0` and
`/` as `~1` in column names. Numeric types distinguish integers, floats and booleans; float
fields retain signed zero and nonfinite bit patterns. Duplicate fields, missing
array entries and inconsistent frame dimensions are rejected.

Strict `pack` rejects reconstructed files whose bytes differ from their recorded
hashes. To intentionally rebuild modified resources:

```sh
python3 tools/resources.py pack assets --output build/modified-game-data --allow-edits
```

This reports changed resources and makes no exact-original claim for them.
Editing pixels inside a sprite rectangle, PCM samples, frame flags or music
events changes the rebuilt files. Editing borders and labels does not. VGA images
are recompressed from their pixels on every rebuild; strict packing rejects
changed results unless edit mode was explicitly selected. Keep the whole export,
including preservation metadata: MIDI integer encodings, unused header words,
padding and trailing bytes are needed for exact reconstruction. There is no hidden original-binary cache.

VGA compression uses the deterministic native packet rules in `src/graphics.c`:
three equal pixels start a repeat packet, packets contain at most 128 pixels,
and `0x80` terminates the stream. The Python encoder bounds lookahead to the
logical image rather than reproducing the native routine's out-of-range read.
It reproduces all 145 original English DOS V2.0 compressed images byte for byte
from their pixels. No `rle.tsv`, packet recipes, original compressed images or
hidden PNG metadata are needed or exported. Unsupported non-native packet choices
are rejected by normal flat export; old exports remain readable for compatibility.

## Format details

VGA, BOB and character pictures use indexed (`P`) sheets. `--palette-bank N`
selects their viewing palette from `GFX/PALETTE.E` (default 0); the game switches
palettes dynamically. Viewing palettes do not change original palette data.
The RGB sheet's 16-by-16 swatch rectangles represent the original palette banks;
edit those rectangles to change palette data. Each RGB component must remain a
multiple of four because the original uses six-bit DAC values.

Maps remain complete standalone indexed PNGs. `map-N.png` retains each original
tile ID as its pixel index; its embedded PLTE composes the verified original
`terrain_tile_colors` lookup with palette bank 62, matching `terrain.c`'s overview.
`district-N.png` uses `owner_to_color` for the low-nibble ownership view and bank
62, matching `map.c`'s `GetBezColor(0)`. `europe-map.png` uses bank 2, as selected
by `CountrySelector`. Initial export verifies the English DOS executable before
extracting these lookup tables. Packing needs only the exported PNGs and tables.
These are native overview images, not a rendering of the entire high-zoom terrain.

Keep each sheet or map's recorded mode and dimensions. Do not convert indexed sheets to RGB.

VOC samples export unsigned 8-bit PCM WAV blocks. WAV rates are rounded from
preserved VOC time constants; no resampling occurs. Repeat, silence and control
blocks stay in TSV. Keep WAV rate, sample width and channel count. Sample lengths
may change; bank offsets are rebuilt. Installed samples use PCM, and unsupported
ADPCM is rejected explicitly.

CMF music contains FM synthesis instructions. Reversible TSV events and named
OPL instruments preserve it; rendering to WAV would lose information needed to
recover the original music. This utility does not render FM music to WAV.

DOS driver archives are outside the converter's scope. `driver-catalog.tsv`,
program hex dumps and unrelated game-data tables are not exported. The native
runtime implements its sound interfaces in C/SDL and no longer requires
`SOUND/DRIVERS.CAT` in its checksum manifest.

Packing restores file contents, relative filename spelling, modes and
modification times. Filesystem ownership, creation times, extended attributes
and directory metadata are not reproduced. Modified fixed resources will be
rejected by the native game's checksum gate; this utility does not bypass it.
