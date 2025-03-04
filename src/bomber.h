#ifndef BOMBER_H
#define BOMBER_H

#include "raylib.h"
#include "player.h"
#include "bomb.h"

extern const int bomberTileSize;

typedef struct Bomber {
    Vector2 position;
    Vector2 velocity;
    float frameCounter;
    int currentFrame;
    int isAlive;
    int isActivatedDeath;
    int flip;
    int isAttack;
} Bomber;

extern const int maxFrameBomberIdle;
extern const int maxFrameBomberDeath;

extern Texture2D textureBomberIdle;
extern Texture2D textureBomberDeath;

extern Rectangle frameRectBomberIdle;
extern Rectangle frameRectBomberDeath;

void initializeBomber(float x, float y, Bomber *bomber);
void updateBomber(Bomber *bomber, Player* player, int **map);
void drawBomber(Bomber *bomber);
void unloadBomber();

#endif