# OpenPizza 0.1

A native C/SDL2 reconstruction of Pizza Tycoon for Windows x64 and macOS.

The game loads graphics, fonts, sound, music and initial tables directly from
**your own Pizza Tycoon English DOS V2.0 installation**. Original game assets
and executables are not distributed. No startup bundle or emulator is required.

Windows: extract the release ZIP, copy the original game directory to `PIZZA`
beside `PizzaTycoon.exe`, and run the launcher. `PT_GAME_DATA_DIR` may point to
another game directory. If the files are missing or incompatible, the game offers
an optional download from the [Internet Archive](https://archive.org/details/msdos_Pizza_Tycoon_1994).
Choose **Download** to install the verified files in your user data folder, or
**Cancel** to leave without downloading. No game data is bundled with OpenPizza.
Saves and logs are stored under `%LOCALAPPDATA%\Pizza Tycoon`.

The public 0.1 checks cover startup, New Game through the city map, and native
save/load. Complete campaign playthroughs and broader regression comparisons
have not yet been repeated on this public build.
Windows builds are smoke-tested with Wine; testing on Windows hardware is pending.

On macOS install SDL2 and run:

```sh
make native
./build/pizza-native --data /path/to/PIZZA
```

All 495 fixed resource files are checked by SHA-256 before play.
The downloader also checks the exact ZIP and executable fingerprints, installs
only the required resources, and never runs the downloaded DOS executable.
Downloaded data is reused on later launches. Existing files are never overwritten.
On Windows it is stored below `%LOCALAPPDATA%\Pizza Tycoon\game-data`; on macOS,
below `~/Library/Application Support/Pizza Tycoon/game-data`.
If an existing managed folder is damaged, a new numbered folder is used.
You can still supply your own installation without downloading anything.

For an explicit download or offline installation of the same verified archive:

```sh
./build/pizza-native --download-data /new/path/PIZZA
./build/pizza-native --install-data-archive Pizza_Tycoon_1994.zip /new/path/PIZZA
```

All downloaded input is validated before the installation becomes available. The unmodified
English V2.0 `PT.EXE` (or `PT_Original.EXE`) must be 470411 bytes with SHA-256
`9cce82355b466fc0e47c51ae1de4ba850a666b5f12c3831ee19ba5c4c4ef6add`.
The executable is read as data to extract initial tables; it is never executed.
Other languages and game revisions are not supported by this release.

`tools/resources.py` converts graphics to labeled PNG sprite sheets, sound to
WAV, and music to editable event/instrument tables. It reconstructs the selected
original media files exactly; see `tools/resource_formats/README.md`.

The source is organized into subsystem files. Include `pizza.h` for the main
application API or the subsystem header listed in `SOURCE_LAYOUT.md`.

Program source: GPL v3 only, copyright 2026 Drevton. The OPL chip model retains
its LGPL license and attribution. The bundled puff inflater retains its zlib license. Original game data retains its owners' rights.
