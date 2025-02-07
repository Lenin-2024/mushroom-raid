#ifndef MONEY_H
#define MONEY_H

#include "raylib.h"

extern const int moneyTileSize;

typedef struct Money {
    Vector2 position;
    int currentFrame;
    int isAlive;
    float frameCounter;
    int isActivated;
} Money;

extern const int maxFrameMoneyIdle;
extern Texture2D textureMoneyIdle;
extern Texture2D textureMoneyPickUp;
extern Rectangle frameRectMoney;

void initializeMoney(float x, float y, Money *money);
void updateMoney(Money *money, float playerX, float playerY, int playerTileSize, int id);
void drawMoney(Money *money);
void unloadMoneyTexture();

#endif