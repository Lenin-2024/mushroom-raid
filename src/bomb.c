#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "raylib.h"
#include "bomb.h"
#include "player.h"

const int bombTileSize = 8;
const float bombGravity = 0.02f;

const int maxFrameBombFly = 3;
const int maxFrameBombActive = 8;
const int maxFrameBombBooM = 8;

Texture2D textureBombFly;
Texture2D textureBombActive;
Texture2D textureBombBooM;

Rectangle frameRectBombFly;
Rectangle frameRectBombActive;
Rectangle frameRectBombBooM;

void initializeBomb(float x, float y, Bomb *bomb) {
    bomb->position = (Vector2){x + 4, y};
    bomb->isAlive = 1;
    bomb->isActivated = 1;
    bomb->frameCounter = 0;
    
    textureBombFly = LoadTexture("resource/miscellaneous sprites/bomb_thrown_anim_strip_3.png");
    if (textureBombFly.id == 0) {
        exit(1);
    }
    textureBombActive = LoadTexture("resource/miscellaneous sprites/bomb_on_ground_anim_strip_8.png");
    if (textureBombActive.id == 0) {
        exit(1);
    }
    
    textureBombBooM = LoadTexture("resource/miscellaneous sprites/bomb_thrown_anim_strip_3.png");
    if (textureBombBooM.id == 0) {
        exit(1);
    }

    frameRectBombFly = (Rectangle) {
        0.0f, 0.0f,
        textureBombFly.width / maxFrameBombFly, textureBombFly.height
    };

    frameRectBombActive = (Rectangle) {
        0.0f, 0.0f,
        textureBombActive.width / maxFrameBombActive, textureBombActive.height
    };

    frameRectBombBooM = (Rectangle) {
        0.0f, 0.0f,
        textureBombBooM.width / maxFrameBombBooM, textureBombBooM.height
    };
}

void updateBomb(Bomb *bomb, Player *player, int **map) {
    const float frameSpeedFly = 0.06f;
    bomb->velocity.x = 0;

    if (bomb->velocity.y != 0) {
        if (player->position.x > bomb->position.x) {
            bomb->velocity.x += 1.5f;
        } else {
            bomb->velocity.x -= 1.5f;
        }
    }

    bomb->velocity.y += gravity;


    bomb->position.x += bomb->velocity.x;
    collisionbWithMap(bomb, map, 0, 16);
    bomb->position.y += bomb->velocity.y;
    collisionbWithMap(bomb, map, 1, 16);

    if (bomb->isActivated == 1) {
        bomb->frameCounter += GetFrameTime();
        if (bomb->frameCounter >= frameSpeedFly) {
            bomb->currentFrame++;
            if (bomb->currentFrame >= maxFrameBombFly) {
                bomb->currentFrame = 0;
                //bomb->isActivated = 0;
            }
            bomb->frameCounter = 0;
        }
    }
}

void collisionbWithMap(Bomb *bomb, int **map, int dir, int tileSize) {
    int startX = (int)(bomb->position.x / tileSize);
    int endX = (int)((bomb->position.x + bombTileSize) / tileSize);
    int startY = (int)(bomb->position.y / tileSize);
    int endY = (int)((bomb->position.y + bombTileSize) / tileSize);
    
    for (int y = startY; y <= endY; y++) {
        for (int x = startX; x <= endX; x++) {
            if (map[y][x] > 0) {
                /*
                if (bomb->velocity.x > 0 && dir == 0) {
                    bomb->position.x = x * tileSize - bombTileSize - 0.1f;
                }
                if (bomb->velocity.x < 0 && dir == 0) {
                    bomb->position.x = x * tileSize + tileSize + 0.1f;
                }
                */

                // y
                /*
                if (bomb->velocity.y < 0 && dir == 1) {
                    bomb->position.y = y * tileSize + tileSize + 0.01f;
                    bomb->velocity.y = 0;
                }
                */

                if (bomb->velocity.y > 0 && dir == 1) {
                    bomb->position.y = y * tileSize;
                    bomb->velocity.y = 0;
                }

            }
        }
    }
}

void drawBomb(Bomb *bomb) {
    if (bomb->isActivated == 1) {
        frameRectBombFly.x = bomb->currentFrame * textureBombFly.width / maxFrameBombFly;
        DrawTextureRec(textureBombFly, frameRectBombFly, (Vector2){bomb->position.x, bomb->position.y - (16 - bombTileSize)}, WHITE);
    }
}

void unloadBomb() {
    UnloadTexture(textureBombFly);
    UnloadTexture(textureBombActive);
    UnloadTexture(textureBombBooM);
}