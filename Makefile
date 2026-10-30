CC ?= gcc
CPPFLAGS += -Iinclude
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2

SOURCES = src/main.c src/card.c src/game.c src/ui.c src/save.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = blackjack

# Détection et configuration de Raylib selon l'OS
ifeq ($(OS),Windows_NT)
    # Configuration Windows (ajustez les chemins si besoin si vous utilisez un autre mode d'installation)
    RAYLIB_LIBS ?= -lraylib -lopengl32 -lgdi32 -lwinmm
    RM = del /Q /F
    FIX_PATH = $(subst /,\,$1)
    RUN_TARGET = .\$(TARGET).exe
else
    # Configuration Linux / macOS
    RAYLIB_CFLAGS ?= $(shell pkg-config --cflags raylib 2>/dev/null)
    RAYLIB_LIBS ?= $(shell pkg-config --libs raylib 2>/dev/null)
    ifeq ($(strip $(RAYLIB_LIBS)),)
        RAYLIB_LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    endif
    RM = rm -f
    FIX_PATH = $1
    RUN_TARGET = ./$(TARGET)
endif

.PHONY: all clean clean-objs run

all: $(TARGET)
	@$(MAKE) clean-objs
	@$(RUN_TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(RAYLIB_LIBS) $(LDLIBS)

src/%.o: src/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(RAYLIB_CFLAGS) -c $< -o $@

clean-objs:
	$(RM) $(call FIX_PATH,$(OBJECTS))

clean:
	$(RM) $(call FIX_PATH,$(OBJECTS) $(TARGET) $(TARGET).exe)