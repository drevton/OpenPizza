.DEFAULT_GOAL := all
CC := clang
PYTHON ?= python3
GAME_DATA_DIR ?= PIZZA
CFLAGS := -std=c11 -O2 -g0 -ffp-contract=off "-ffile-prefix-map=$(CURDIR)=." "-fmacro-prefix-map=$(CURDIR)=." -Wall -Wextra -Werror -Isrc -I.
CFLAGS += "-ffile-prefix-map=$(CURDIR)=." "-fmacro-prefix-map=$(CURDIR)=."
CORE := src/game.c
CORE += src/graphics.c
CORE += src/text.c
CORE += src/input.c
CORE += src/sound.c
CORE += src/calendar.c
CORE += src/files.c
CORE += src/savegame.c
CORE += src/world.c
CORE += src/player.c
CORE += src/restaurant.c
CORE += src/staff.c
CORE += src/trade.c
CORE += src/recipe.c
CORE += src/menu.c
CORE += src/character.c
CORE += src/reports.c
CORE += src/finance.c
CORE += src/missions.c
CORE += src/messages.c
CORE += src/frame.c
CORE += src/game_data.c
CORE += src/initial_data.c
CORE += src/command.c
CORE += src/arithmetic.c
CORE += src/savegame_initial.c
CORE += src/runtime_state.c
CORE += src/ai_day.c
CORE += src/host_io.c
NATIVE_PLATFORM_LIBS := -lcurl
NATIVE_HEADERS := $(wildcard src/*.h src/*.def) vendor/pt_opl_adapter.h vendor/puff/puff.c vendor/puff/puff.h
NATIVE_HEADERS += vendor/nuked-opl3/opl3.h vendor/nuked-opl3/opl3.c

# Native gameplay is compiled directly.
NATIVE_GAME_SOURCES := $(CORE)

.PHONY: all native windows clean
all: build/pizza-native
native: build/pizza-native

WINDOWS_CC ?= x86_64-w64-mingw32-gcc
WINDOWS_SDL ?= build/windows-deps/SDL2-2.32.10/x86_64-w64-mingw32
WINDOWS_CFLAGS := -std=c11 -O2 -g0 -Wall -Wextra -Werror -Isrc -I. -ffp-contract=off
WINDOWS_CFLAGS += -Wno-unknown-pragmas -Wno-unused-but-set-variable
WINDOWS_CFLAGS += -Wno-int-in-bool-context -Wno-implicit-fallthrough
WINDOWS_CFLAGS += "-ffile-prefix-map=$(CURDIR)=." "-fmacro-prefix-map=$(CURDIR)=."
WINDOWS_LDFLAGS := -static-libgcc -Wl,--no-insert-timestamp,--stack,8388608 -s
WINDOWS_LIBS := -Wl,-Bstatic -lwinpthread -Wl,-Bdynamic -lSDL2 -lm
windows: build/windows/pizza-native.exe build/windows/PizzaTycoon.exe

build/windows:
	mkdir -p $@

build/windows/pizza-native.exe: $(NATIVE_GAME_SOURCES) src/native.c src/display.c $(NATIVE_HEADERS) | build/windows
	$(WINDOWS_CC) $(WINDOWS_CFLAGS) -I"$(WINDOWS_SDL)/include/SDL2" $(NATIVE_GAME_SOURCES) src/native.c src/display.c -L"$(WINDOWS_SDL)/lib" $(WINDOWS_LDFLAGS) -municode $(WINDOWS_LIBS) -lwinhttp -o $@

build/windows/PizzaTycoon.exe: tools/windows_launcher.c src/types.h | build/windows
	$(WINDOWS_CC) $(WINDOWS_CFLAGS) $(WINDOWS_LDFLAGS) -municode -mwindows $< -lshell32 -o $@

build:
	mkdir -p build


build/pizza-native: $(NATIVE_GAME_SOURCES) src/native.c src/display.c $(NATIVE_HEADERS) | build
	$(CC) $(CFLAGS) $$(sdl2-config --cflags) $(NATIVE_GAME_SOURCES) src/native.c src/display.c $$(sdl2-config --libs) $(NATIVE_PLATFORM_LIBS) -Wl,-map,build/pizza-native-link.map -o $@

clean:
	rm -rf build
