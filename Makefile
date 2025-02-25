CC := gcc

CFLAGS := -Wall -g -I"./raylib/include"
LDFLAGS := -L"./raylib/src" -lraylib -lm -lpthread -ldl -lrt -lX11

TARGET := game

SRCS := src/game.c src/player.c src/map.c src/money.c src/slime.c

OBJS := $(SRCS:.c=.o)

all: $(TARGET)

debug: CFLAGS += -DDEBUG -g
debug: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
