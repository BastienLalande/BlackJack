CC ?= gcc
CPPFLAGS += -Iinclude
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2

SOURCES = src/main.c src/card.c src/game.c src/ui.c src/save.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = blackjack

RAYLIB_CFLAGS := $(shell pkg-config --cflags raylib 2>/dev/null)
RAYLIB_LIBS := $(shell pkg-config --libs raylib 2>/dev/null)

ifeq ($(strip $(RAYLIB_LIBS)),)
ifeq ($(OS),Windows_NT)
RAYLIB_LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
else
RAYLIB_LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif
endif

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(RAYLIB_LIBS) $(LDLIBS)

src/%.o: src/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(RAYLIB_CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJECTS) $(TARGET) $(TARGET).exe
