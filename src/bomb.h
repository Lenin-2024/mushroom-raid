#ifndef BOMB_H
#define BOMB_H

#include "raylib.h"
#include "player.h"

extern const int bombTileSize;
extern const float bombGravity;

typedef enum {
    BOMB_STATE_FLYING,
    BOMB_STATE_ON_GROUND,
    BOMB_STATE_EXPLODING
} BombState;

typedef struct Bomb {
    Vector2 position;
    Vector2 velocity;
    float bombRadius;
    float frameCounter;
    int currentFrame;
    int isAlive;
    int isActivated;
    int flip;
    BombState state;
    int damageDealt;
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

void initializeBomb(float x, float y, Bomb *bomb, float plX, float plY);
void updateBomb(Bomb *bomb, Player* player, int **map);
void collisionbWithMap(Bomb *bomb, int **map, int dir, int tileSize);
void drawBomb(Bomb *bomb);
void unloadBomb();

#endif