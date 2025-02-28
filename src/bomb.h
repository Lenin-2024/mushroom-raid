#ifndef BOMB_H
#define BOMB_H

#include "raylib.h"
#include "player.h"

extern const int bombTileSize;
extern const float bombGravity;

typedef struct Bomb {
    Vector2 position;
    Vector2 velocity;
    float bombrRadius;
    float frameCounter;
    int currentFrame;
    int isAlive;
    int isActivated;
    int flip;
} Bomb;

extern const int maxFrameBombFly;
extern const int maxFrameBombActive;
extern const int maxFrameBombBooM;

extern Texture2D textureBombFly;
extern Texture2D textureBombActive;
extern Texture2D textureBombBooM;

extern Rectangle frameRectBombFly;
extern Rectangle frameRectBombActive;
extern Rectangle frameRectBombBooM;

void initializeBomb(float x, float y, Bomb *bomb);
void updateBomb(Bomb *bomb, Player* player, int **map);
void collisionbWithMap(Bomb *bomb, int **map, int dir, int tileSize);
void drawBomb(Bomb *bomb);
void unloadBomb();

#endif