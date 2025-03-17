#ifndef BOMBER_H
#define BOMBER_H

#include "raylib.h"
#include "player.h"
#include "bomb.h"

extern const int bomberTileSize;

typedef struct Bomber {
    Vector2 position;
    Vector2 velocity;
    int isAlive;
    int isActivatedDeath;
    int currentFrame;
    float frameCounter;
    int flip;
    int isAttack;
    int startAttckAnim;
    float lastAttackTime;
} Bomber;

extern const int maxFrameBomberIdle;
extern const int maxFrameBomberDeath;

extern Texture2D textureBomberIdle;
extern Texture2D textureBomberDeath;
extern Texture2D textureBomberAttack;

extern Rectangle frameRectBomberIdle;
extern Rectangle frameRectBomberAttack;
extern Rectangle frameRectBomberDeath;

void initializeBomber(float x, float y, Bomber *bomber);
void updateBomber(Bomber *bomber, Player* player, int **map);
void drawBomber(Bomber *bomber);
void unloadBomber();

#endif