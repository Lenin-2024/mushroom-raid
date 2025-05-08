CC := gcc

CFLAGS := -Wall -g -I"./raylib"
LDFLAGS := -L"./raylib" -lraylib -lm -lpthread -ldl -lrt -lX11

TARGET := game

SRCS := src/game.c src/player.c src/map.c src/money.c src/slime.c src/bomber.c src/bomb.c src/vase.c src/stone.c

OBJS := $(SRCS:.c=.o)

all: $(TARGET)

debug: CFLAGS += -DDEBUG -g
debug: $(TARGET)

$(TARGET): $(OBJS)
	make -C ./raylib
	gcc -Wall -I"./raylib" -L"./raylib" -o tmp tileMapEditor.c -lraylib -lm -lpthread -ldl -lrt -lX11
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	make clean -C ./raylib
	rm -f $(TARGET) $(OBJS)