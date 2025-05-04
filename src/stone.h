#ifndef STONE_H
#define STONE_H

#include "player.h"
#include <raylib.h>

extern Texture2D textureStone;

typedef struct Stone {
    Vector2 position;
    Vector2 velocity;
    int onGround;
    int tileSize;
} Stone;

void initializeStone(float x, float y, Stone *stone);
void updateStone(Player *player, int **map, Stone *stone);
void collisionStone(Stone *stone, int **map, int dir, int tileSize);
void drawStone(Stone *stone);
void unloadStone();

#endif