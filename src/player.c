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
const int maxFrameAttack = 4;

const float gravity = 0.05f;
int fallCheck = 0;

Texture2D textureRun;
Texture2D textureIdle;
Texture2D textureFall;
Texture2D textureJump;
Texture2D textureDeath;
Texture2D textureAfterJump;
Texture2D textureBeforeJump;
Texture2D textureAttack;

Sound soundRun;
Sound soundAttack;
Sound soundJump;

Rectangle frameRect;
Rectangle frameRectDust;
Rectangle frameRectAttack;
Vector2 dustPosition = {0};

void initializePlayer(float x, float y, Player *player) {
    player->position = (Vector2){ x, y };
    player->velocity = (Vector2){0, 0};
    player->onGround = 1;
    player->currentFrame = 0;
    player->currentAttackFrame = 0;
    player->flip = 0;
    player->health = 3;
    player->jumpHeight = 1.5f;
    player->tileSize = 12;
    player->stopDeathAnim = 0;
    player->dustAfterAnimationActive = 0;
    player->dustBeforeAnimationActive = 0;
    player->isAttack = 0;
    player->attackWidth = 24;

    if (textureRun.id == 0) {
        textureRun = LoadTexture("resource/herochar sprites(new)/herochar_run_anim_strip_6.png");
        if (textureRun.id == 0) {
            puts("Ошибка: не удалось загрузить текстуру анимации бега игрока");
            exit(1);
        }
    }

    if (textureIdle.id == 0) {
        textureIdle = LoadTexture("resource/herochar sprites(new)/herochar_idle_anim_strip_4.png");
        if (textureIdle.id == 0) {
            puts("Ошибка: не удалось загрузить текстуру анимации ожидания игрока");
            exit(1);
        }
    }

    if (textureJump.id == 0) {
        textureJump = LoadTexture("resource/herochar sprites(new)/herochar_jump_up_anim_strip_3.png");
        if (textureJump.id == 0) {
            puts("Ошибка: не удалось загрузить текстуру анимации прыжка вверх игрока");
            exit(1);
        }
    }

    if (textureFall.id == 0) {
        textureFall = LoadTexture("resource/herochar sprites(new)/herochar_jump_down_anim_strip_3.png");
        if (textureFall.id == 0) {
            puts("Ошибка: не удалось загрузить текстуру анимации прыжка вниз игрока");
            exit(1);
        }
    }

    if (textureDeath.id == 0) {
        textureDeath = LoadTexture("resource/herochar sprites(new)/herochar_death_anim_strip_8.png");
        if (textureDeath.id == 0) {
            puts("Ошибка: не удалось загрузить текстуру анимации смерти игрока");
            exit(1);
        }
    }

    if (textureAfterJump.id == 0) {
        textureAfterJump = LoadTexture("resource/herochar sprites(new)/herochar_after_jump_dust_anim_strip_4.png");
        if (textureAfterJump.id == 0) {
            puts("Ошибка: не удалось загрузить текстуру анимации пыли после прыжка игрока");
            exit(1);
        }
    }

    if (textureBeforeJump.id == 0) {
        textureBeforeJump = LoadTexture("resource/herochar sprites(new)/herochar_before_jump_dust_anim_strip_4.png");
        if (textureBeforeJump.id == 0) {
            puts("Ошибка: не удалось загрузить текстуру анимации пыли перед прыжком игрока");
            exit(1);
        }
    }

    if (textureAttack.id == 0) {
        textureAttack = LoadTexture("resource/herochar sprites(new)/herochar_sword_attack_anim_strip_4.png");
        if (textureAttack.id == 0) {
            puts("Ошибка: не удалось загрузить текстуру анимации атаки игрока");
            exit(1);
        }
    }

    // Загрузка звуков только если они еще не загружены
    if (soundRun.stream.buffer == NULL) {
        soundRun = LoadSound("resource/Sound/barrelstart.mp3");
        if (soundRun.stream.buffer == NULL) {
            puts("Ошибка: не удалось загрузить звук бега игрока");
            exit(1);
        }
    }

    if (soundAttack.stream.buffer == NULL) {
        soundAttack = LoadSound("resource/Sound/attack.mp3");
        if (soundAttack.stream.buffer == NULL) {
            puts("Ошибка: не удалось загрузить звук атаки игрока");
            exit(1);
        }
    }

    if (soundJump.stream.buffer == NULL) {
        soundJump = LoadSound("resource/Sound/playerjump.mp3");
        if (soundJump.stream.buffer == NULL) {
            puts("Ошибка: не удалось загрузить звук прыжка игрока");
            exit(1);
        }
    }

    frameRect = (Rectangle){
        0.0f, 0.0f, 
        (float)textureIdle.width / maxFrameIdle, (float)textureIdle.height 
    };

    frameRectDust = (Rectangle){
        0.0f, 0.0f,
        (float)textureBeforeJump.width / maxFrameBeforeAfterJump, (float)textureBeforeJump.height
    };

    frameRectAttack = (Rectangle){
        0.0f, 0.0f,
        (float)textureAttack.width / maxFrameAttack, (float)textureAttack.height
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
    const float frameSpeedAttack = 0.1f;

    static int justLanded = 0;
    player->velocity.x = 0;
    player->attackWidth = player->flip ? -fabs(player->attackWidth) : fabs(player->attackWidth);

    if (IsKeyDown(KEY_LEFT) && (player->health > 0)) {
        player->velocity.x -= speed;
        player->flip = 1;
        if (!IsSoundPlaying(soundRun) && player->onGround) {
            PlaySound(soundRun);
        }
    } else if (IsKeyDown(KEY_RIGHT) && (player->health > 0)) {
        player->velocity.x += speed;
        player->flip = 0;
        if (!IsSoundPlaying(soundRun) && player->onGround) {
            PlaySound(soundRun);
        }
    } else {
        if (!IsSoundPlaying(soundRun)) {
            StopSound(soundRun);
        }        
    }
   
    if (player->onGround && player->health > 0) {
        fallCheck = 1;
        if (justLanded == 1) {
            player->dustAfterAnimationActive = 1;
            justLanded = 0;
            dustPosition = (Vector2){player->position.x - player->tileSize / 4, 
                                     player->position.y - player->tileSize / 4};
        }
        if (IsKeyDown(KEY_UP)) {
            player->velocity.y = -player->jumpHeight;
            player->jumpFrame = 0;
            player->dustBeforeAnimationActive = 1;
            player->dustAfterAnimationActive = 0;
            justLanded = 1;
            dustPosition = (Vector2){player->position.x - player->tileSize / 4, 
                                     player->position.y - player->tileSize / 4};
            PlaySound(soundJump);
        }
        if (IsKeyPressed(KEY_SPACE) && (player->isAttack == 0)) {
            player->isAttack = 1;
            PlaySound(soundAttack);
        }
    } else {
        if ((fallCheck == 1) && (player->velocity.y > 0)) {
            player->velocity.y = 0;
        }
        fallCheck = 0;
        player->velocity.y += gravity;
    }

    player->position.x += player->velocity.x;
    collision(player, map, 0, tileSize);
    
    player->position.y += player->velocity.y;
    collision(player, map, 1, tileSize);
    
    // анимация пыли под игроком
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
    if (player->isAttack == 1) {
        if (frameCounter >= frameSpeedAttack) {
                player->currentAttackFrame++;
                if (player->currentAttackFrame >= maxFrameAttack) {
                    player->currentAttackFrame = 0;
                    player->isAttack = 0;
                }
            frameCounter = 0;
        }
    } else if (player->onGround == 1 && player->health > 0) {
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
    if (player->isAttack == 1) {
        frameRectAttack.width = player->flip ? -fabs(frameRectAttack.width) : fabs(frameRectAttack.width);
        frameRectAttack.x = player->currentAttackFrame * (textureAttack.width / maxFrameAttack);
        DrawTextureRec(textureAttack, frameRectAttack, (Vector2){ player->flip ? player->position.x - (textureAttack.width / maxFrameAttack - player->tileSize) : 
                                                                                 player->position.x - (textureAttack.width / maxFrameAttack - player->tileSize) / player->tileSize, 
                                                                 player->position.y - (textureAttack.height - player->tileSize)}, WHITE);
    }
    else if (player->onGround == 1 && player->health > 0) {
        if (player->velocity.x != 0) {
            frameRect.x = player->currentFrame * (textureRun.width / maxFrameRun);
            DrawTextureRec(textureRun, frameRect, (Vector2){player->position.x - (textureIdle.width / maxFrameIdle - player->tileSize) / 2, 
                                                            player->position.y - (textureIdle.height - player->tileSize)}, WHITE);
        } else if (player->velocity.x == 0) {
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

void unloadPlayer() {
    UnloadTexture(textureIdle);
    UnloadTexture(textureRun);
    UnloadTexture(textureFall);
    UnloadTexture(textureJump);
    UnloadTexture(textureAfterJump);
    UnloadTexture(textureBeforeJump);
    UnloadSound(soundRun);
    UnloadSound(soundAttack);
    UnloadSound(soundJump);
}