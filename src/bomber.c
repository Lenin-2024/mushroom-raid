#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "raylib.h"
#include "bomber.h"
#include "player.h"

const int bomberTileSize = 16;
const int maxFrameBomberIdle = 4;

Texture2D textureBomberIdle;

Rectangle frameRectBomberIdle;

void initializeBomber(float x, float y, Bomber *bomber) {
    bomber->position = (Vector2){x, y - 0.1f};
    bomber->isAlive = 1;
    bomber->isActivatedDeath = 0;
    bomber->frameCounter = 0;

    textureBomberIdle = LoadTexture("resource/enemies sprites/bomber goblin/bomber_goblin_idle_anim_strip_4.png");
    if (textureBomberIdle.id == 0) {
        puts("INFO: текстуры анимации ожидания слизня не загрузились");
        exit(1);
    }

    frameRectBomberIdle = (Rectangle) {
        0.0f, 0.0f,
        (float)textureBomberIdle.width / maxFrameBomberIdle, (float)textureBomberIdle.height
    };
}

void updateBomber(Bomber *bomber, Player *player, int **map) {
    const float frameSpeedIdle = 0.09f;
    
    bomber->velocity.x = 0;
    
    // проверка ударил ли игрок слайма
    float attackEndX = player->position.x + (player->attackWidth > 0 ? player->attackWidth : 0);
    float attackStartX = player->position.x + (player->attackWidth < 0 ? player->attackWidth + player->tileSize : 0);
    if ((attackStartX < (bomber->position.x + bomberTileSize)) && (attackEndX > bomber->position.x) && 
        (player->position.y < (bomber->position.y + bomberTileSize)) && 
        ((player->position.y + player->tileSize) > bomber->position.y) && 
        (bomber->isAlive == 1) && (bomber->isActivatedDeath == 0) && (player->isAttack == 1)) {
        bomber->isActivatedDeath = 1;
        bomber->isAlive = 0;
        bomber->currentFrame = 0;
        bomber->frameCounter = 0;
        bomber->velocity.x = 0;
    }

    if (bomber->velocity.x == 0) {
        bomber->frameCounter += GetFrameTime();
        if (bomber->frameCounter >= frameSpeedIdle) {
            bomber->currentFrame++;
                if (bomber->currentFrame >= maxFrameBomberIdle) {
                    bomber->currentFrame = 0;
                }
                bomber->frameCounter = 0;
            }
        }
}

void drawBomber(Bomber *bomber) {
    //DrawRectangle(slime->position.x + (slimeTileSize / 4), slime->position.y, slimeTileSize / 2, slimeTileSize / 2, RED);
    if (bomber->isActivatedDeath == 0) {
        if (bomber->velocity.x == 0) {
            frameRectBomberIdle.width = bomber->flip ? -fabs(frameRectBomberIdle.width) : fabs(frameRectBomberIdle.width);
            frameRectBomberIdle.x = (float)bomber->currentFrame * (float)textureBomberIdle.width / maxFrameBomberIdle;
            DrawTextureRec(textureBomberIdle, frameRectBomberIdle, (Vector2){bomber->position.x, bomber->position.y - (16 - bomberTileSize)}, WHITE);
        }
    }
}

void unloadBomber() {
    UnloadTexture(textureBomberIdle);
}