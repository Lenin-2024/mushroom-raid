CC := gcc

CFLAGS := -Wall -g -I"./raylib/include"
LDFLAGS := -L"./raylib/src" -lraylib -lm -lpthread -ldl -lrt -lX11

TARGET := game

SRCS := src/game.c src/player.c src/map.c src/money.c src/slime.c src/bomber.c

OBJS := $(SRCS:.c=.o)

all: $(TARGET)

debug: CFLAGS += -DDEBUG -g
debug: $(TARGET)

$(TARGET): $(OBJS)
	make -C ./raylib/src
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	make clean -C ./raylib/src
	rm -f $(TARGET) $(OBJS)
