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


The fixed-file SHA-256 manifest is compiled into `src/game_data.c`. Missing,
truncated, modified or wrong-edition files stop startup before gameplay. The
original executable fingerprint is checked separately before reading defaults.
Extract the installed game files; DOS installation archives alone are insufficient.
Settings and files created during play are mutable and are not fixed asset inputs.

OpenPizza 0.1 uses its own in-memory save identities. Original DOS saved-game
compatibility is not promised. Retain backups when trying any prerelease build.
