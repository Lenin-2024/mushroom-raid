#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "player.h"

const int playerTileSize = 14;

const int maxFrameRun = 6;
const int maxFrameIdle = 4;
const float gravity = 0.05f;
float jumpHeight = 1.5f;

Texture2D textureRun;
Texture2D textureIdle;
Texture2D textureFall;
Rectangle frameRect;

void initializePlayer(float x, float y, Player *player) {
    player->position = (Vector2){ x, y };
    player->onGround = 1;
    player->currentFrame = 0;
    player->flip = 0;
    player->health = 3;

    textureRun = LoadTexture("resource/herochar sprites(new)/herochar_run_anim_strip_6.png");
    if (textureRun.id == 0) {
        puts("INFO: текстуры анимации бега игрока не загрузились");
        exit(1);
    }

    textureIdle = LoadTexture("resource/herochar sprites(new)/herochar_idle_anim_strip_4.png");
    if (textureIdle.id == 0) {
        puts("INFO: текстуры анимации ожидания игрока не загрузились");
        exit(1);
    }

    frameRect = (Rectangle) {
        0.0f, 0.0f, 
        (float)textureIdle.width / maxFrameIdle, (float)textureIdle.height 
    };
}

void updatePlayer(Player *player, float speed, int **map, int tileSize) {
    static float frameCounter = 0.0f;
    const float frameSpeedRun = 0.05f;
    const float frameSpeedIdle = 0.08f;

    player->velocity.x = 0;

    if (IsKeyDown(KEY_LEFT)) {
        player->velocity.x -= speed;
        player->flip = 1;
    }

    if (IsKeyDown(KEY_RIGHT)) {
        player->velocity.x += speed;
        player->flip = 0;
    }

    if (!player->onGround) {
        player->velocity.y += gravity;
    }

    if (player->onGround && (IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_UP))) {
        player->velocity.y -= jumpHeight;
        player->onGround = 0;
    }

    player->position.x += player->velocity.x;
    collision(player, map, 0, tileSize);
    
    player->position.y += player->velocity.y;
    collision(player, map, 1, tileSize);

    // анимации
    if (player->velocity.x != 0 && player->onGround == 1) {
        frameCounter += GetFrameTime();
        if (frameCounter >= frameSpeedRun) {
            player->currentFrame++;
            if (player->currentFrame >= maxFrameRun) {
                player->currentFrame = 0;
            }
            frameCounter = 0;
        }
    }

    else if (player->velocity.x == 0 && player->onGround == 1) {
        frameCounter += GetFrameTime();
        if (frameCounter >= frameSpeedIdle) {
            player->currentFrame++;
            if (player->currentFrame >= maxFrameIdle) {
                player->currentFrame = 0;
            }
            frameCounter = 0;
        }
    }
}

void collision(Player *player, int **map, int dir, int tileSize) {
    int startX = (int)(player->position.x / tileSize);
    int endX = (int)((player->position.x + playerTileSize) / tileSize);
    int startY = (int)(player->position.y / tileSize);
    int endY = (int)((player->position.y + playerTileSize) / tileSize);
    
    player->onGround = 0;

    for (int y = startY; y <= endY; y++) {
        for (int x = startX; x <= endX; x++) {
            if (map[y][x] > 0) {
                // x
                if (player->velocity.x > 0 && dir == 0) {
                    player->position.x = x * tileSize - playerTileSize - 0.1f;
                }
                if (player->velocity.x < 0 && dir == 0) {
                    player->position.x = x * tileSize + tileSize + 0.01f;
                }
                // y
                if (player->velocity.y > 0 && dir == 1) {
                    player->position.y = y * tileSize - playerTileSize - 0.01f;
                    player->velocity.y = 0;
                    player->onGround = 1;
                }
                if (player->velocity.y < 0 && dir == 1) {
                    player->position.y = y * tileSize + tileSize + 0.01f;
                    player->velocity.y = 0;
                }
            }
        }
    }
}

void drawPlayer(Player *player) {
    //DrawRectangle(player->position.x, player->position.y, playerTileSize, playerTileSize, RED);
    frameRect.width = player->flip ? -fabs(frameRect.width) : fabs(frameRect.width);
    if (player->velocity.x != 0) {
        frameRect.x = (float)player->currentFrame * (float)textureRun.width / maxFrameRun;
        DrawTextureRec(textureRun, frameRect, (Vector2){player->position.x, player->position.y - (16 - playerTileSize)}, WHITE);
    }
    else if (player->velocity.x == 0) {
        frameRect.x = (float)player->currentFrame * (float)textureIdle.width / maxFrameIdle;
        DrawTextureRec(textureIdle, frameRect, (Vector2){player->position.x, player->position.y - (16 - playerTileSize)}, WHITE);
    }
}


void unloadPlayerTexture() {
    UnloadTexture(textureIdle);
    UnloadTexture(textureRun);
}