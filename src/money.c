#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include "money.h"

const int moneyTileSize = 16;
const int maxFrameMoneyIdle = 6;
const int maxFrameMoneyPickUp = 6;

Texture2D textureMoneyIdle;
Texture2D textureMoneyPickUp;
Rectangle frameRectMoney;
Rectangle frameRectMoneyPickUp;

void initializeMoney(float x, float y, Money *money) {
    money->isAlive = 1;
    money->isActivated = 0;

    money->position = (Vector2){ x, y };
    money->currentFrame = 0;
    money->frameCounter = 0;

    textureMoneyIdle = LoadTexture("resource/miscellaneous sprites/coin_anim_strip_6.png");
    textureMoneyPickUp = LoadTexture("resource/miscellaneous sprites/coin_pickup_anim_strip_6.png");
    
    frameRectMoney = (Rectangle){
        0, 0,
        textureMoneyIdle.width / maxFrameMoneyIdle, textureMoneyIdle.height
    };

    frameRectMoneyPickUp = (Rectangle){
        0, 0,
        textureMoneyPickUp.width / maxFrameMoneyPickUp, textureMoneyPickUp.height
    };
}

void updateMoney(Money *money, float playerX, float playerY, int playerTileSize) {
    float speedAnimationIdle = 0.08f;
    float speedAnimationPickUp = 0.1f;

    if (playerX + playerTileSize > money->position.x && playerX < money->position.x + 8 &&
        playerY + playerTileSize > money->position.y && playerY < money->position.y + 8 && 
        money->isAlive == 1 && money->isActivated == 0) {
            money->isActivated = 1;
            money->frameCounter = 0;
            money->currentFrame = 0;
    }

    // idle анимация
    if (money->isActivated == 0) {
        money->frameCounter += GetFrameTime();
        if (money->frameCounter >= speedAnimationIdle) {
            money->currentFrame++;
            if (money->currentFrame >= maxFrameMoneyIdle) {
                money->currentFrame = 0;
            }
            money->frameCounter = 0;
        }
    }

    // pickUp анимация
    if (money->isActivated == 1) {
        money->frameCounter += GetFrameTime();
        if (money->frameCounter >= speedAnimationPickUp) {
            money->currentFrame++;
            if (money->currentFrame >= maxFrameMoneyPickUp) {
                money->isAlive = 0;
                money->currentFrame = 0;
            }
            money->frameCounter = 0;

        }
    }
    //printf("ID: %d, Alive: %d, Position: [%f, %f], Frame: %d\n", id, money->isAlive, money->position.x, money->position.y, money->currentFrame);
}

void drawMoney(Money *money) {
    if (money->isActivated == 0) {
        frameRectMoney.x = money->currentFrame * textureMoneyIdle.width / maxFrameMoneyIdle;
        DrawTextureRec(textureMoneyIdle, frameRectMoney, money->position, WHITE);
    }
    else {
        frameRectMoneyPickUp.x = money->currentFrame * textureMoneyPickUp.width / maxFrameMoneyPickUp;
        DrawTextureRec(textureMoneyPickUp, frameRectMoneyPickUp, (Vector2){money->position.x, money->position.y - 8 }, WHITE);
    }
}

void unloadMoneyTexture() {
    UnloadTexture(textureMoneyPickUp);
    UnloadTexture(textureMoneyIdle);
}