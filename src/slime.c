#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "raylib.h"
#include "slime.h"

const int slimeTileSize = 16;
const int maxFrameSlimeIdle = 5;
const int maxFrameSlimeRun = 15;
const int maxFrameSlimeDeath = 6;

Texture2D textureSlimeIdle;
Texture2D textureSlimeRun;
Texture2D textureSlimeDeath;
Rectangle frameRectSlimeRun;
Rectangle frameRectSlimeIdle;

void initializeSlime(float x, float y, Slime *slime) {
    slime->position = (Vector2){x, y};
    slime->isAlive = 1;
    slime->isActivatedDeath = 0;
    slime->frameCounter = 0;
    
    textureSlimeIdle = LoadTexture("resource/enemies sprites/slime/slime_idle_anim_strip_5.png");
    if (textureSlimeIdle.id == 0) {
        puts("INFO: текстуры анимации ожидания слизня не загрузились");
        exit(1);
    }

    textureSlimeRun = LoadTexture("resource/enemies sprites/slime/slime_walk_anim_strip_15.png");
    if (textureSlimeIdle.id == 0) {
        puts("INFO: текстуры анимации ожидания слизня не загрузились");
        exit(1);
    }

    frameRectSlimeRun = (Rectangle) {
        0.0f, 0.0f,
        (float)textureSlimeRun.width / maxFrameSlimeRun, (float)textureSlimeRun.height
    };

    frameRectSlimeIdle = (Rectangle) {
        0.0f, 0.0f,
        (float)textureSlimeIdle.width / maxFrameSlimeIdle, (float)textureSlimeIdle.height
    };
}

void updateSlime(Slime *slime) {
    const float frameSpeedRun = 0.05f;
    const float frameSpeedIdle = 0.08f;

    slime->velocity.x += 0.002;
    if (slime->velocity.x > 0) {
        slime->flip = 1;
    }
    else if (slime->velocity.x < 0) {
        slime->flip = 0;
    }

    slime->position.x += slime->velocity.x;

    if (slime->velocity.x != 0) {
        slime->frameCounter += GetFrameTime();
        if (slime->frameCounter >= frameSpeedRun) {
            slime->currentFrame++;
            if (slime->currentFrame >= maxFrameSlimeRun) {
                slime->currentFrame = 0;
            }
            slime->frameCounter = 0;
        }
    }

    else if (slime->velocity.x == 0) {
        slime->frameCounter += GetFrameTime();
        if (slime->frameCounter >= frameSpeedIdle) {
            slime->currentFrame++;
            if (slime->currentFrame >= maxFrameSlimeIdle) {
                slime->currentFrame = 0;
            }
            slime->frameCounter = 0;
        }
    }
}

void drawSlime(Slime *slime) {
    DrawRectangle(slime->position.x, slime->position.y, slimeTileSize, slimeTileSize, RED);
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
}

void unloadSlimeTexture() {
    UnloadTexture(textureSlimeIdle);
}