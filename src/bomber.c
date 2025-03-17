#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "raylib.h"
#include "bomber.h"
#include "player.h"

const int bomberTileSize = 16;
const int maxFrameBomberIdle = 4;
const int maxFrameBomberDeath = 6;
const int maxFrameBomberAttack = 6;

Texture2D textureBomberIdle;
Texture2D textureBomberDeath;
Texture2D textureBomberAttack;

Rectangle frameRectBomberIdle;
Rectangle frameRectBomberDeath;
Rectangle frameRectBomberAttack;

void initializeBomber(float x, float y, Bomber *bomber) {
    bomber->position = (Vector2){x, y - 0.1f};
    bomber->isAlive = 1;
    bomber->isActivatedDeath = 0;
    bomber->frameCounter = 0;
    bomber->flip = 1;
    bomber->isAttack = 0; // 1
    bomber->startAttckAnim = 0; // 1
    bomber->lastAttackTime = 0.0f;

    if (textureBomberIdle.id == 0) {
        textureBomberIdle = LoadTexture("resource/enemies sprites/bomber goblin/bomber_goblin_idle_anim_strip_4.png");
        if (textureBomberIdle.id == 0) {
            puts("Ошибка: не удалось загрузить текстуру idle bomber");
            exit(1);
        }
    }

    if (textureBomberDeath.id == 0) {
        textureBomberDeath = LoadTexture("resource/enemies sprites/bomber goblin/bomber_goblin_death_anim_strip_6.png");
        if (textureBomberDeath.id == 0) {
            puts("Ошибка: не удалось загрузить текстуру смерти bomber");
            exit(1);
        }
    }

    if (textureBomberAttack.id == 0) {
        textureBomberAttack = LoadTexture("resource/enemies sprites/bomber goblin/bomber_goblin_attack_anim_strip_6.png");
        if (textureBomberAttack.id == 0) {
            puts("Ошибка: не удалось загрузить текстуру атаки bomber");
            exit(1);
        }
    }

    frameRectBomberIdle = (Rectangle){
        0.0f, 0.0f,
        (float)textureBomberIdle.width / maxFrameBomberIdle, (float)textureBomberIdle.height
    };

    frameRectBomberDeath = (Rectangle){
        0.0f, 0.0f,
        (float)textureBomberDeath.width / maxFrameBomberDeath, (float)textureBomberDeath.height
    };

    frameRectBomberAttack = (Rectangle){
        0.0f, 0.0f,
        (float)textureBomberAttack.width / maxFrameBomberAttack, (float)textureBomberAttack.height
    };
}

void updateBomber(Bomber *bomber, Player *player, int **map) {
    const float frameSpeedIdle = 0.09f;
    const float frameSpeedDeath = 0.125f;
    const float frameSpeedAttack = 0.09f;
    const float attackRange = 50.0f;
    const float attackCooldown = 2.0f;

    bomber->velocity.x = 0;
       
    if (player->position.x > bomber->position.x) {
        bomber->flip = 0;
    } else {
        bomber->flip = 1;
    }

    if (fabs(player->position.x - bomber->position.x) < attackRange && 
        bomber->isAlive && 
        !bomber->isActivatedDeath && 
        (GetTime() - bomber->lastAttackTime >= attackCooldown)) {
        bomber->startAttckAnim = 1;
        bomber->isAttack = 1;
        bomber->lastAttackTime = GetTime();
    } else {
        bomber->isAttack = 0;
    }

    float attackEndX = player->position.x + (player->attackWidth > 0 ? player->attackWidth : 0);
    float attackStartX = player->position.x + (player->attackWidth < 0 ? player->attackWidth + player->tileSize : 0);
    if ((attackStartX < (bomber->position.x + bomberTileSize)) && 
        (attackEndX > bomber->position.x) && 
        (player->position.y < (bomber->position.y + bomberTileSize)) && 
        ((player->position.y + player->tileSize) > bomber->position.y) && 
        (bomber->isAlive == 1) && (bomber->isActivatedDeath == 0) && (player->isAttack == 1)) {
        bomber->isActivatedDeath = 1;
        bomber->currentFrame = 0;
        bomber->frameCounter = 0;
        bomber->velocity.x = 0;
    }

    if (bomber->isAlive && !bomber->isActivatedDeath) {
        if (bomber->startAttckAnim == 1) {
            bomber->frameCounter += GetFrameTime();
            if (bomber->frameCounter >= frameSpeedAttack) {
                bomber->currentFrame++;
                if (bomber->currentFrame >= maxFrameBomberAttack) {
                    bomber->startAttckAnim = 0;
                    bomber->isAttack = 0;
                    bomber->currentFrame = 0;
                }
                bomber->frameCounter = 0;
            }
        } else if (bomber->velocity.x == 0) {
            bomber->frameCounter += GetFrameTime();
            if (bomber->frameCounter >= frameSpeedIdle) {
                bomber->currentFrame++;
                    if (bomber->currentFrame >= maxFrameBomberIdle) {
                        bomber->currentFrame = 0;
                    }
                bomber->frameCounter = 0;
            }
        }
    } else if (bomber->isActivatedDeath == 1) {
        bomber->frameCounter += GetFrameTime();
        if (bomber->frameCounter >= frameSpeedDeath) {
            bomber->currentFrame++;
            if (bomber->currentFrame == maxFrameBomberDeath) {
                bomber->isAlive = 0;
                bomber->currentFrame = 0;
            }
            bomber->frameCounter = 0;
        }
    }
}

void drawBomber(Bomber *bomber) {
    if (bomber->isActivatedDeath == 0) {
        if (bomber->startAttckAnim == 1) {
            frameRectBomberAttack.width = bomber->flip ? -fabs(frameRectBomberAttack.width) : fabs(frameRectBomberAttack.width);
            frameRectBomberAttack.x = (float)bomber->currentFrame * textureBomberAttack.width / maxFrameBomberAttack;
            DrawTextureRec(textureBomberAttack, frameRectBomberAttack, (Vector2){bomber->position.x, bomber->position.y}, WHITE);
        } else if (bomber->velocity.x == 0) {
            frameRectBomberIdle.width = bomber->flip ? -fabs(frameRectBomberIdle.width) : fabs(frameRectBomberIdle.width);
            frameRectBomberIdle.x = (float)bomber->currentFrame * (float)textureBomberIdle.width / maxFrameBomberIdle;
            DrawTextureRec(textureBomberIdle, frameRectBomberIdle, (Vector2){bomber->position.x, bomber->position.y - (16 - bomberTileSize)}, WHITE);
        }
    } else if ((bomber->isActivatedDeath == 1) && (bomber->isAlive == 1)) {
        frameRectBomberDeath.width = bomber->flip ? -fabs(frameRectBomberDeath.width) : fabs(frameRectBomberDeath.width);
        frameRectBomberDeath.x = (float)bomber->currentFrame * textureBomberDeath.width / maxFrameBomberDeath;
        DrawTextureRec(textureBomberDeath, frameRectBomberDeath, (Vector2){bomber->position.x, bomber->position.y - (bomberTileSize / 2)}, WHITE);
    }
}

void unloadBomber() {
    if (textureBomberIdle.id != 0) {
        UnloadTexture(textureBomberIdle);
        textureBomberIdle.id = 0;
    }

    if (textureBomberDeath.id != 0) {
        UnloadTexture(textureBomberDeath);
        textureBomberDeath.id = 0;
    }
    
    if (textureBomberAttack.id != 0) {
        UnloadTexture(textureBomberAttack);
        textureBomberAttack.id = 0;
    }
}