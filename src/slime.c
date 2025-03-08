#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "raylib.h"
#include "slime.h"
#include "player.h"

const int slimeTileSize = 16;
const int maxFrameSlimeIdle = 5;
const int maxFrameSlimeRun = 15;
const int maxFrameSlimeDeath = 6;
const float slimeSpeed = 0.3f;
float playerDamage = 0.3f;

Texture2D textureSlimeIdle;
Texture2D textureSlimeRun;
Texture2D textureSlimeDeath;

Rectangle frameRectSlimeRun;
Rectangle frameRectSlimeIdle;
Rectangle frameRectSlimeDeath;

Sound deathSound;

void initializeSlime(float x, float y, Slime *slime) {
    slime->position = (Vector2){x, y - 0.1f};
    slime->isAlive = 1;
    slime->isActivatedDeath = 0;
    slime->frameCounter = 0;
    slime->dir = 0;

    if (textureSlimeIdle.id == 0) {
        textureSlimeIdle = LoadTexture("resource/enemies sprites/slime/slime_idle_anim_strip_5.png");
        if (textureSlimeIdle.id == 0) {
            puts("Ошибка: не удалось загрузить текстуру анимации ожидания слизня");
            exit(1);
        }
    }

    if (textureSlimeRun.id == 0) {
        textureSlimeRun = LoadTexture("resource/enemies sprites/slime/slime_walk_anim_strip_15.png");
        if (textureSlimeRun.id == 0) {
            puts("Ошибка: не удалось загрузить текстуру анимации ходьбы слизня");
            exit(1);
        }
    }

    if (textureSlimeDeath.id == 0) {
        textureSlimeDeath = LoadTexture("resource/enemies sprites/slime/slime_death_anim_strip_6.png");
        if (textureSlimeDeath.id == 0) {
            puts("Ошибка: не удалось загрузить текстуру анимации смерти слизня");
            exit(1);
        }
    }

    frameRectSlimeRun = (Rectangle) {
        0.0f, 0.0f,
        (float)textureSlimeRun.width / maxFrameSlimeRun, (float)textureSlimeRun.height
    };

    frameRectSlimeIdle = (Rectangle) {
        0.0f, 0.0f,
        (float)textureSlimeIdle.width / maxFrameSlimeIdle, (float)textureSlimeIdle.height
    };

    frameRectSlimeDeath = (Rectangle) {
        0.0f, 0.0f,
        (float)textureSlimeDeath.width / maxFrameSlimeDeath, (float)textureSlimeDeath.height
    };

    if (deathSound.stream.buffer == NULL) {
        deathSound = LoadSound("resource/Sound/slimedeath.mp3");
        if (deathSound.stream.buffer == NULL) {
            puts("Ошибка: не удалось загрузить звук смерти слизня");
            exit(1);
        }
    }
}

void updateSlime(Slime *slime, Player *player, int **map) {
    const float frameSpeedRun = 0.05f;
    const float frameSpeedIdle = 0.09f;
    const float frameSpeedDeath = 0.125;
    const float damageInterval = 0.5f;

    slime->velocity.x = 0;
    
    // проверка коллизий
    playerDamage += GetFrameTime();

    if (((player->position.x + player->tileSize) > (slime->position.x + 6)) &&
        (player->position.x < (slime->position.x + slimeTileSize - 4)) &&
        ((player->position.y + player->tileSize) > (slime->position.y + 4)) &&
        ((player->position.y < slime->position.y + slimeTileSize)) &&
        slime->isAlive == 1 && slime->isActivatedDeath == 0) {
        if (player->position.y + player->tileSize <= slime->position.y + (slimeTileSize / 2)) {
            player->velocity.y = -player->jumpHeight;
            slime->isActivatedDeath = 1;
            slime->currentFrame = 0;
            slime->frameCounter = 0;
            slime->velocity.x = 0;
            PlaySound(deathSound);
        } else {
            if (playerDamage >= damageInterval && player->health > 0) {
                player->health--;
                playerDamage = 0;
            }            
        }
    }

    // проверка ударил ли игрок слайма
    float attackEndX = player->position.x + (player->attackWidth > 0 ? player->attackWidth : 0);
    float attackStartX = player->position.x + (player->attackWidth < 0 ? player->attackWidth + player->tileSize : 0);
    if ((attackStartX < (slime->position.x + slimeTileSize)) && (attackEndX > slime->position.x) && 
        (player->position.y < (slime->position.y + slimeTileSize)) && 
        ((player->position.y + player->tileSize) > slime->position.y) && 
        (slime->isAlive == 1) && (slime->isActivatedDeath == 0) && (player->isAttack == 1)) {
        slime->isActivatedDeath = 1;
        slime->currentFrame = 0;
        slime->frameCounter = 0;
        slime->velocity.x = 0;
        PlaySound(deathSound);
    }

    if (slime->dir == 0 && slime->isActivatedDeath != 1) {
        slime->velocity.x -= slimeSpeed;
    } else if(slime->dir == 1 && slime->isActivatedDeath != 1) {
        slime->velocity.x += slimeSpeed;
    }

    slime->flip = slime->dir;
    slime->position.x += slime->velocity.x;

    collisionWithMap(slime, map, 16);

    if (slime->isActivatedDeath == 0) {
        if (slime->velocity.x != 0) {
            slime->frameCounter += GetFrameTime();
            if (slime->frameCounter >= frameSpeedRun) {
                slime->currentFrame++;
                if (slime->currentFrame >= maxFrameSlimeRun) {
                    slime->currentFrame = 0;
                }
                slime->frameCounter = 0;
            }
        } else if (slime->velocity.x == 0) {
            slime->frameCounter += GetFrameTime();
            if (slime->frameCounter >= frameSpeedIdle) {
                slime->currentFrame++;
                if (slime->currentFrame >= maxFrameSlimeIdle) {
                    slime->currentFrame = 0;
                }
                slime->frameCounter = 0;
            }
        }
    } else if (slime->isActivatedDeath == 1) {
        slime->frameCounter += GetFrameTime();
        if (slime->frameCounter >= frameSpeedDeath) {
            slime->currentFrame++;
            if (slime->currentFrame == maxFrameSlimeDeath) {
                slime->isAlive = 0;
                slime->currentFrame = 0;
            }
            slime->frameCounter = 0;
        }
    }
}

void collisionWithMap(Slime *slime, int **map, int tileSize) {
    int startX = (int)(slime->position.x / tileSize);
    int endX = (int)((slime->position.x + slimeTileSize) / tileSize);
    int startY = (int)(slime->position.y / tileSize);
    int endY = (int)((slime->position.y + slimeTileSize) / tileSize);
    
    for (int y = startY; y <= endY; y++) {
        for (int x = startX; x <= endX; x++) {
            if (map[y][x] > 0) {
                if (slime->velocity.x > 0) {
                    slime->dir = 0;
                    slime->velocity.x = 0;
                } else if (slime->velocity.x < 0) {
                    slime->dir = 1;
                    slime->velocity.x = 0;
                }
            }
        }
    }
}

void drawSlime(Slime *slime) {
    //DrawRectangle(slime->position.x + (slimeTileSize / 4), slime->position.y, slimeTileSize / 2, slimeTileSize / 2, RED);
    if (slime->isActivatedDeath == 0) {
        if (slime->velocity.x != 0) {
            frameRectSlimeRun.width = slime->flip ? -fabs(frameRectSlimeRun.width) : fabs(frameRectSlimeRun.width);
            frameRectSlimeRun.x = (float)slime->currentFrame * (float)textureSlimeRun.width / maxFrameSlimeRun;
            DrawTextureRec(textureSlimeRun, frameRectSlimeRun, (Vector2){slime->position.x, slime->position.y - (slimeTileSize / 2)}, WHITE);
        }
        else if (slime->velocity.x == 0) {
            frameRectSlimeIdle.width = slime->flip ? -fabs(frameRectSlimeIdle.width) : fabs(frameRectSlimeIdle.width);
            frameRectSlimeIdle.x = (float)slime->currentFrame * (float)textureSlimeIdle.width / maxFrameSlimeIdle;
            DrawTextureRec(textureSlimeIdle, frameRectSlimeIdle, (Vector2){slime->position.x, slime->position.y - (16 - slimeTileSize)}, WHITE);
        }
    } else if (slime->isActivatedDeath == 1 && slime->isAlive == 1) {
        frameRectSlimeDeath.x = (float)slime->currentFrame * (float)textureSlimeDeath.width / maxFrameSlimeDeath;
        DrawTextureRec(textureSlimeDeath, frameRectSlimeDeath, (Vector2){slime->position.x, slime->position.y - (slimeTileSize / 2)}, WHITE);
    }
}

void unloadSlime() {
    UnloadTexture(textureSlimeDeath);
    UnloadTexture(textureSlimeIdle);
    UnloadTexture(textureSlimeRun);
    UnloadSound(deathSound);
}