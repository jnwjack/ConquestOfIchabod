SHELL = /bin/bash

CFLAG = -Wall -g
CC = gcc
WEB_CC = emcc
ENTRY = src/main.c
WEB_FLAGS = -s USE_SDL=2 --emrun --use-preload-plugins -s ALLOW_MEMORY_GROWTH=1
WEB_FLAGS_2 = --emrun --use-preload-plugins -s NO_EXIT_RUNTIME=1 -s OFFSCREEN_FRAMEBUFFER=1 -s USE_WEBGL2=1 -s FULL_ES3=1 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS=["png"] -s EXPORTED_RUNTIME_METHODS="['cwrap', 'ccall']" -s USE_SDL=2 -s USE_SDL_MIXER=2 -s USE_SDL_TTF=2 -s ALLOW_MEMORY_GROWTH=1
WEB_FLAGS_LIBS = -sUSE_SDL=2 -sUSE_SDL_IMAGE=2 -sUSE_SDL_TTF=2 -sSDL2_IMAGE_FORMATS=["png"]
WEB_FLAGS_LIBS2 = --use-port=sdl2_image:formats=png --use-port=sdl2_ttf
WEB_FILES = --preload-file=assets --preload-file=test_data --preload-file=src/engine/etc --preload-file=src/threadtown/spritemap.dat --preload-file=src/armory/spritemap.dat  --preload-file=src/battle/spritemap.dat
SRCS = $(shell find src -name "*.c")
OUTPUT = coi
WEB_OUTPUT = coi.html
LIBS = -lSDL2 -lSDL2main -lSDL2_image -lSDL2_ttf -lSDL2_mixer
DEBUGFLAGS = -g3 -D__COI_DEBUG__=1
NATIVE_FLAGS = -D__NATIVE__=1
# WINDOWS_CC = gcc
# WINDOWS_INCLUDE_BASE = C:\Users\jnw90\Projects\mingw-dev\include
# WINDOWS_LIB_BASE = C:\Users\jnw90\Projects\mingw-dev\lib
WINDOWS_CC = C:\mingw32\bin\gcc.exe
WINDOWS_INCLUDE_BASE = C:\Users\jnw90\Projects\testSDL\include
WINDOWS_LIB_BASE = C:\Users\jnw90\Projects\testSDL\lib
WINDOWS_FLAGS = -m32 -D__WINDOWS__=1
WINDOWS_SRCS = src\main.c src\actor.c src\inputloops.c src\inventory.c src\items.c src\player.c src\special.c src\tech.c src\tests.c src\TextBox.c src\TimeState.c src\armory\Armory.c src\battle\Battle.c src\battle\BattleBehavior.c src\battle\BattleSplash.c src\battle\Status.c src\engine\COIAssetLoader.c src\engine\COIBoard.c src\engine\COIMenu.c src\engine\COISprite.c src\engine\COIString.c src\engine\COITextType.c src\engine\COITransition.c src\engine\COIWindow.c src\engine\util.c src\gameover\GameOver.c src\renthouse\RentHouse.c src\threadtown\PauseOverlay.c src\threadtown\Town.c src\title\Title.c src\engine\COISound.c src\engine\COIPreferences.c src\title\CharacterCreation.c

# TOOLS
SRCS_TOOLS = tools/cbb.c src/engine/util.c
CFLAG_TOOLS = -g

windows_debug:
	$(WINDOWS_CC) $(WINDOWS_SRCS) $(DEBUGFLAGS) -I$(WINDOWS_INCLUDE_BASE) -L$(WINDOWS_LIB_BASE) -w -Wl,-subsystem,windows -lmingw32 $(LIBS) $(NATIVE_FLAGS) $(WINDOWS_FLAGS) -o $(OUTPUT)
windows:
	$(WINDOWS_CC) $(WINDOWS_SRCS) -I$(WINDOWS_INCLUDE_BASE) -L$(WINDOWS_LIB_BASE) -w -Wl,-subsystem,windows -lmingw32 $(LIBS) $(NATIVE_FLAGS) $(WINDOWS_FLAGS) -o $(OUTPUT)
# the turtle command works on mac with emscripten 3.1.33
turtle:
	$(WEB_CC) $(SRCS) -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' -s USE_SDL_TTF=2 --preload-file assets --preload-file test_data --preload-file src/engine/etc --preload-file src/threadtown/spritemap.dat --preload-file src/armory/spritemap.dat --preload-file src/battle/spritemap.dat -o coi.html

web:
	$(WEB_CC) $(SRCS) $(WEB_FILES) $(WEB_FLAGS) $(WEB_FLAGS_LIBS) -o $(WEB_OUTPUT)

build:
	$(CC) $(SRCS) $(CFLAGS) $(NATIVE_FLAGS) $(LIBS) -o $(OUTPUT)

debug:
	$(CC) $(SRCS) $(DEBUGFLAGS) $(CFLAGS) $(NATIVE_FLAGS) $(LIBS) -o $(OUTPUT)

clean:
	rm -f *.o $(OUTPUT) *~ src/*~ src/engine/*~ src/.#* src/engine/.#*

tool:
	$(CC) $(SRCS_TOOLS) `pkg-config gtk4 --cflags pkg-config gtk4 --libs` -o cbb 
