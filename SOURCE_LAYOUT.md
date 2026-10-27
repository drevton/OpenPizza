# Source layout

Include `pizza.h` for the application entry point, command API and frame output.
Game implementation code includes the header for the subsystem it calls.
Game headers live in `src/` beside their corresponding C files.
Headers are self-contained; callers do not need to assemble an include order.

The native game now has 31 C files and 43 headers, reduced from 155 C files and
165 headers before the Windows port. Related implementations and declarations
are physically merged; there are no forwarding headers or included C files.
Original reconstructed function boundaries and exported names are retained.
File-private helpers with colliding names have subsystem-qualified names.

| Header | Responsibility |
| --- | --- |
| `pizza.h` | Application-facing entry, commands and frame output |
| `native.h`, `display.h`, `host_io.h` | Native startup, SDL frontend and host file/process services |
| `game.h`, `frame.h`, `command.h` | Initialization, cooperative execution and recorded commands |
| `graphics.h`, `render_types.h`, `text.h` | Drawing, bitmaps, fonts and text |
| `input.h`, `menu.h` | Pointer/keyboard input and menu navigation |
| `sound.h`, `sound_types.h` | PCM/FM audio and device records |
| `world.h`, `player.h`, `calendar.h`, `ai_day.h` | Towns, player turns, clock and AI |
| `restaurant.h`, `staff.h`, `trade.h` | Restaurant operations, staff, suppliers and stock |
| `recipe.h`, `character.h` | Recipe editing/lists and character interaction |
| `finance.h`, `missions.h`, `reports.h`, `messages.h` | Money, missions, reports and notices |
| `files.h`, `savegame.h`, `game_data.h` | File lifecycle, save/load and checksum admission |
| `types.h`, `layout.h`, `state_types.h` | Integer aliases, fixed wire records and native objects |
| `globals.h`, `native_globals.h`, `runtime_state.h` | Shared state and runtime ownership |
| `objects.h`, `memory.h` | Typed object and buffer access |
| `arithmetic.h`, `initial_data.h` | Arithmetic semantics and runtime initialization |
| `savegame_format.h`, `saved_globals.h`, `sound_layout.h` | Serialization boundaries |

Generated record/global headers and large initialization data remain separate
so their generators can update them without rewriting unrelated declarations.
