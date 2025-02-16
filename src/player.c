#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "player.h"

const int maxFrameRun = 6;
const int maxFrameIdle = 4;
const int maxFrameJump = 3;
const int maxFrameFall = 3;
const int maxFrameDeath = 8;
const int maxFrameBeforeAfterJump = 4;
const float gravity = 0.05f;
int fallCheck = 0;

Texture2D textureRun;
Texture2D textureIdle;
Texture2D textureFall;
Texture2D textureJump;
Texture2D textureDeath;
Texture2D textureAfterJump;
Texture2D textureBeforeJump;

Rectangle frameRect;
Rectangle frameRectDust;

Vector2 dustPosition = {0};

void initializePlayer(float x, float y, Player *player) {
    player->position = (Vector2){ x, y };
    player->velocity = (Vector2){0, 0};
    player->onGround = 1;
    player->currentFrame = 0;
    player->flip = 0;
    player->health = 3;
    player->jumpHeight = 1.5f;
    player->tileSize = 12;
    player->stopDeathAnim = 0;
    player->dustAfterAnimationActive = 0;
    player->dustBeforeAnimationActive = 0;

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
    textureJump = LoadTexture("resource/herochar sprites(new)/herochar_jump_up_anim_strip_3.png");
    if (textureJump.id == 0) {
       puts("INFO: текстуры анимации прыжка игрока не загрузились");
       exit(1);
    }
    textureFall = LoadTexture("resource/herochar sprites(new)/herochar_jump_down_anim_strip_3.png");
    if (textureJump.id == 0) {
       puts("INFO: текстуры анимации прыжка игрока не загрузились");
       exit(1);
    }
    textureDeath = LoadTexture("resource/herochar sprites(new)/herochar_death_anim_strip_8.png");
    if (textureDeath.id == 0) {
       puts("INFO: текстуры анимации смерти игрока не загрузились");
       exit(1);
    }
    textureAfterJump = LoadTexture("resource/herochar sprites(new)/herochar_after_jump_dust_anim_strip_4.png");
    if (textureAfterJump.id == 0) {
        exit(1);
    }
    textureBeforeJump = LoadTexture("resource/herochar sprites(new)/herochar_before_jump_dust_anim_strip_4.png");
    if (textureAfterJump.id == 0) {
        exit(1);
    }

    frameRect = (Rectangle) {
        0.0f, 0.0f, 
        (float)textureIdle.width / maxFrameIdle, (float)textureIdle.height 
    };

    frameRectDust = (Rectangle) {
        0.0f, 0.0f,
        (float)textureBeforeJump.width / maxFrameBeforeAfterJump, (float)textureBeforeJump.height
    };
}

void updatePlayer(Player *player, float speed, int **map, int tileSize) {
    static float frameCounter = 0.0f;
    static float frameDustCounter = 0.0f;
    const float frameSpeedRun = 0.1f;
    const float frameSpeedIdle = 0.15f;
    const float frameSpeedJump = 0.1f;
    const float frameSpeedFall = 0.1f;
    const float frameSpeedDeath = 0.1f;
    const float frameSpeedBeforeJump = 0.085f;
    const float frameSpeedAfterJump = 0.075f;
    
    static int justLanded = 0;
    player->velocity.x = 0;

    if (IsKeyDown(KEY_LEFT) && player->health > 0) {
        player->velocity.x -= speed;
        player->flip = 1;
    }
    if (IsKeyDown(KEY_RIGHT) && player->health > 0) {
        player->velocity.x += speed;
        player->flip = 0;
    }
   
    if (player->onGround) {
        fallCheck = 1;
        if (justLanded == 1) {
            player->dustAfterAnimationActive = 1;
            justLanded = 0;
            dustPosition = (Vector2){player->position.x - player->tileSize / 4, player->position.y - player->tileSize / 4};
        }

        if ((IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_UP)) && (player->health > 0)) {
            player->velocity.y = -player->jumpHeight;
            player->jumpFrame = 0;
            player->dustBeforeAnimationActive = 1;
            player->dustAfterAnimationActive = 0;
            justLanded = 1;
            dustPosition = (Vector2){player->position.x - player->tileSize / 4, player->position.y - player->tileSize / 4};
        }
    } else {
        if (fallCheck == 1 && player->velocity.y > 0) {
            player->velocity.y = 0;
        }
        fallCheck = 0;
        player->velocity.y += gravity;
    }
    
    player->position.x += player->velocity.x;
    collision(player, map, 0, tileSize);
    
    player->position.y += player->velocity.y;
    collision(player, map, 1, tileSize);
    
    // анимация пыли по игроком
    frameDustCounter += GetFrameTime();
    if (player->dustBeforeAnimationActive == 1) {
        if (frameDustCounter >= frameSpeedBeforeJump) {
            player->dustFrame++;
            if (player->dustFrame >= maxFrameBeforeAfterJump) {
                player->dustFrame = 0;
                player->dustBeforeAnimationActive = 0;
                player->dustAfterAnimationActive = 0;
            }
            frameDustCounter = 0;
        }
    }
    
    if (player->dustAfterAnimationActive == 1) {
        if (frameDustCounter >= frameSpeedAfterJump) {
            player->dustFrame++;
            if (player->dustFrame >= maxFrameBeforeAfterJump) {
                player->dustFrame = 0;
                player->dustAfterAnimationActive = 0;
                player->dustBeforeAnimationActive = 0;
            }
            frameDustCounter = 0;
        }
    }

    //анимация игрока
    frameCounter += GetFrameTime();
    if (player->onGround == 1 && player->health > 0) {
        if (player->velocity.x != 0) {
            if (frameCounter >= frameSpeedRun) {
                player->currentFrame++;
                if (player->currentFrame >= maxFrameRun) {
                    player->currentFrame = 0;
                }
                frameCounter = 0;
            }
        } else if (player->velocity.x == 0) {
            if (frameCounter >= frameSpeedIdle) {
                player->currentFrame++;
                if (player->currentFrame >= maxFrameIdle) {
                    player->currentFrame = 0;
                }
                frameCounter = 0;
            }
        }
    } else if (player->onGround == 0 && player->health > 0) {
        if (player->velocity.y > 0) {
            if (frameCounter >= frameSpeedFall) {
                player->fallFrame++;
                if (player->fallFrame >= maxFrameFall) {
                    player->fallFrame = 0;
                }
                frameCounter = 0;
            }
        } else {
            if (frameCounter >= frameSpeedJump) {
                player->jumpFrame++;
                if (player->jumpFrame >= maxFrameJump) {
                    player->jumpFrame = 0;
                }
                frameCounter = 0;
            }
        }
    } else {
        if (frameCounter >= frameSpeedDeath) {
            player->currentFrame++;
            if (player->currentFrame >= maxFrameDeath) {
                player->currentFrame = 0;
                player->stopDeathAnim = 1;
            }
            frameCounter = 0;
        }
    }
}

void collision(Player *player, int **map, int dir, int tileSize) {
    int startX = (int)(player->position.x / tileSize);
    int endX = (int)((player->position.x + player->tileSize) / tileSize);
    int startY = (int)(player->position.y / tileSize);
    int endY = (int)((player->position.y + player->tileSize) / tileSize);
    
    player->onGround = 0;

    for (int y = startY; y <= endY; y++) {
        for (int x = startX; x <= endX; x++) {
            if (map[y][x] > 0) {
                // x
                if (player->velocity.x > 0 && dir == 0) {
                    player->position.x = x * tileSize - player->tileSize - 0.1f;
                }
                if (player->velocity.x < 0 && dir == 0) {
                    player->position.x = x * tileSize + tileSize + 0.1f;
                }
                // y
                if (player->velocity.y > 0 && dir == 1) {
                    player->position.y = y * tileSize - player->tileSize - 0.1f;
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
    //DrawRectangle(player->position.x, player->position.y, player->tileSize, player->tileSize, RED);
    // отрисовка пыли под игроком
    if (player->dustBeforeAnimationActive == 1) {
        frameRectDust.x = (float)player->dustFrame * (float)textureBeforeJump.width / maxFrameBeforeAfterJump;
        DrawTextureRec(textureBeforeJump, frameRectDust, dustPosition, WHITE);
    } 

    if (player->dustAfterAnimationActive == 1) {
        frameRectDust.x = (float)player->dustFrame * (float)textureAfterJump.width / maxFrameBeforeAfterJump;
        DrawTextureRec(textureAfterJump, frameRectDust, dustPosition, WHITE);
    }

    // отрисовка игрока
    frameRect.width = player->flip ? -fabs(frameRect.width) : fabs(frameRect.width);
    if (player->onGround == 1 && player->health > 0) {
        if (player->velocity.x != 0) {
            frameRect.x = player->currentFrame * (textureRun.width / maxFrameRun);
            DrawTextureRec(textureRun, frameRect, (Vector2){player->position.x - (textureIdle.width / maxFrameIdle - player->tileSize) / 2, 
                                                            player->position.y - (textureIdle.height - player->tileSize)}, WHITE);
        } else {
            frameRect.x = player->currentFrame * (textureIdle.width / maxFrameIdle);
            DrawTextureRec(textureIdle, frameRect, (Vector2){player->position.x - (textureIdle.width / maxFrameIdle - player->tileSize) / 2, 
                                                             player->position.y - (textureIdle.height - player->tileSize)}, WHITE);
        }
    } else if (player->onGround == 0 && player->health > 0) {
        if (player->velocity.y < 0) {
            frameRect.x = player->jumpFrame * (textureJump.width / maxFrameJump);
            DrawTextureRec(textureJump, frameRect, (Vector2){player->position.x - (textureJump.width / maxFrameJump - player->tileSize) / 2, 
                                                             player->position.y - (textureJump.height - player->tileSize)}, WHITE);
        } else {
            frameRect.x = (float)player->fallFrame * (float)textureFall.width / maxFrameFall;
            DrawTextureRec(textureFall, frameRect, (Vector2){player->position.x - (textureFall.width / maxFrameIdle - player->tileSize) / 2, 
                                                             player->position.y - (textureFall.height - player->tileSize)}, WHITE);
        }
    } else if (player->stopDeathAnim == 0) {
        frameRect.x = (float)player->currentFrame * (float)textureDeath.width / maxFrameDeath;
        DrawTextureRec(textureDeath, frameRect, (Vector2){player->position.x - (textureDeath.width / maxFrameDeath - player->tileSize) / 2, 
                                                          player->position.y - (textureDeath.height - player->tileSize)}, WHITE);
    }
}

void unloadPlayerTexture() {
    UnloadTexture(textureIdle);
    UnloadTexture(textureRun);
    UnloadTexture(textureFall);
    UnloadTexture(textureJump);
    UnloadTexture(textureAfterJump);
    UnloadTexture(textureBeforeJump);
}