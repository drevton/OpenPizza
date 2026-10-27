# Windows x64

Build with Python 3.12+, GNU make and an x86_64-w64-mingw32 GCC toolchain:

```sh
python3 tools/build_windows.py --output build/OpenPizza-0.1-windows-x64
```

The helper downloads the official SDL2 2.32.10 MinGW SDK and verifies its SHA-256.
Use `--sdl-dir` with an already downloaded SDK. The ZIP contains binaries, licenses,
complete corresponding program source and media conversion scripts. No game assets
are included. Copy your verified English DOS V2.0 installation into `PIZZA` beside
the launcher, or set an absolute `PT_GAME_DATA_DIR`.

The launcher displays missing/invalid asset errors, keeps per-user saves and logs in
`%LOCALAPPDATA%\Pizza Tycoon`, and prevents two instances sharing a save directory.
`PT_SAVE_DIRECTORY` selects another absolute save directory.

Command line usage:

```sh
pizza-native.exe --data C:\Games\PIZZA --disk user.ptdisk
```
