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
Sound soundLose;

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
    player->dustFrame = 0;
    player->fallFrame = 0;
    player->jumpFrame = 0;
    player->isAttack = 0;
    player->attackWidth = 24;
    player->state = STATE_IDLE;
    player->stateDust = STATE_NO_DUST;

    // Загрузка текстур
    if (textureRun.id == 0) {
        textureRun = LoadTexture("resource/herochar sprites(new)/herochar_run_anim_strip_6.png");
        if (textureRun.id == 0) {
            puts("Error: Failed to load run texture");
            exit(1);
        }
    }

    if (textureIdle.id == 0) {
        textureIdle = LoadTexture("resource/herochar sprites(new)/herochar_idle_anim_strip_4.png");
        if (textureIdle.id == 0) {
            puts("Error: Failed to load idle texture");
            exit(1);
        }
    }

    if (textureJump.id == 0) {
        textureJump = LoadTexture("resource/herochar sprites(new)/herochar_jump_up_anim_strip_3.png");
        if (textureJump.id == 0) {
            puts("Error: Failed to load jump texture");
            exit(1);
        }
    }

    if (textureFall.id == 0) {
        textureFall = LoadTexture("resource/herochar sprites(new)/herochar_jump_down_anim_strip_3.png");
        if (textureFall.id == 0) {
            puts("Error: Failed to load fall texture");
            exit(1);
        }
    }

    if (textureDeath.id == 0) {
        textureDeath = LoadTexture("resource/herochar sprites(new)/herochar_death_anim_strip_8.png");
        if (textureDeath.id == 0) {
            puts("Error: Failed to load death texture");
            exit(1);
        }
    }

    if (textureAfterJump.id == 0) {
        textureAfterJump = LoadTexture("resource/herochar sprites(new)/herochar_after_jump_dust_anim_strip_4.png");
        if (textureAfterJump.id == 0) {
            puts("Error: Failed to load after jump dust texture");
            exit(1);
        }
    }

    if (textureBeforeJump.id == 0) {
        textureBeforeJump = LoadTexture("resource/herochar sprites(new)/herochar_before_jump_dust_anim_strip_4.png");
        if (textureBeforeJump.id == 0) {
            puts("Error: Failed to load before jump dust texture");
            exit(1);
        }
    }

    if (textureAttack.id == 0) {
        textureAttack = LoadTexture("resource/herochar sprites(new)/herochar_sword_attack_anim_strip_4.png");
        if (textureAttack.id == 0) {
            puts("Error: Failed to load attack texture");
            exit(1);
        }
    }

    // Загрузка звуков
    if (soundLose.stream.buffer == NULL) {
        soundLose = LoadSound("resource/Sound/you_lose.mp3");
        if (soundLose.stream.buffer == NULL) {
            exit(1);
        }
    }

    if (soundRun.stream.buffer == NULL) {
        soundRun = LoadSound("resource/Sound/barrelstart.mp3");
        if (soundRun.stream.buffer == NULL) {
            puts("Error: Failed to load run sound");
            exit(1);
        }
    }

    if (soundAttack.stream.buffer == NULL) {
        soundAttack = LoadSound("resource/Sound/attack.mp3");
        if (soundAttack.stream.buffer == NULL) {
            puts("Error: Failed to load attack sound");
            exit(1);
        }
    }

    if (soundJump.stream.buffer == NULL) {
        soundJump = LoadSound("resource/Sound/playerjump.mp3");
        if (soundJump.stream.buffer == NULL) {
            puts("Error: Failed to load jump sound");
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
    
    // Сброс горизонтальной скорости
    player->velocity.x = 0;
    player->attackWidth = player->flip ? -fabs(player->attackWidth) : fabs(player->attackWidth);

    // Обработка ввода только если игрок жив
    if (player->health > 0) {
        // Движение влево/вправо
        if (IsKeyDown(KEY_LEFT)) {
            player->velocity.x -= speed;
            player->flip = 1;
            if (!IsSoundPlaying(soundRun) && player->onGround) {
                PlaySound(soundRun);
            }
        } else if (IsKeyDown(KEY_RIGHT)) {
            player->velocity.x += speed;
            player->flip = 0;
            if (!IsSoundPlaying(soundRun) && player->onGround) {
                PlaySound(soundRun);
            }
        } else {
            StopSound(soundRun);
        }

        // Прыжок
        if (player->onGround && IsKeyPressed(KEY_UP)) {
            player->velocity.y = -player->jumpHeight;
            player->jumpFrame = 0;
            player->stateDust = STATE_BEFORE_JUMP_DUST;
            dustPosition = (Vector2){player->position.x - player->tileSize / 4, 
                                    player->position.y - player->tileSize / 4};
            PlaySound(soundJump);
            justLanded = 1;
        }

        // Атака
        if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_DOWN)) && !player->isAttack) {
            player->state = STATE_ATTACKING;
            player->isAttack = 1;
            player->currentAttackFrame = 0;
            PlaySound(soundAttack);
        }
    }

    // Гравитация
    if (!player->onGround && player->health > 0) {
        player->velocity.y += gravity;
    }

    // Обновление позиции и коллизии
    player->position.x += player->velocity.x;
    collision(player, map, 0, tileSize);
    
    player->position.y += player->velocity.y;
    collision(player, map, 1, tileSize);

    // Определение состояния игрока
    if (player->health <= 0) {
        player->state = STATE_DEAD;
        PlaySound(soundLose);
    } else if (player->isAttack) {
        player->state = STATE_ATTACKING;
    } else if (!player->onGround) {
        if (player->velocity.y < 0) {
            player->state = STATE_JUMPING;
        } else {
            player->state = STATE_FALLING;
        }
    } else {
        // При приземлении
        if (player->onGround && justLanded) {
            player->stateDust = STATE_AFTER_JUMP_DUST;
            dustPosition = (Vector2){
                player->position.x - player->tileSize / 4, 
                player->position.y - player->tileSize / 4};
            justLanded = 0;
        }
        if (fabs(player->velocity.x) > 0.1f) {
            player->state = STATE_RUNNING;
        } else {
            player->state = STATE_IDLE;
        }
    }

    // Обновление анимаций
    frameCounter += GetFrameTime();
    frameDustCounter += GetFrameTime();
    
    switch (player->state) {
        case STATE_ATTACKING:
            if (frameCounter >= frameSpeedAttack) {
                player->currentAttackFrame++;
                if (player->currentAttackFrame >= maxFrameAttack) {
                    player->currentAttackFrame = 0;
                    player->isAttack = 0;
                    player->state = STATE_IDLE;
                }
                frameCounter = 0;
            }
            break;
        case STATE_RUNNING:
            if (frameCounter >= frameSpeedRun) {
                player->currentFrame = (player->currentFrame + 1) % maxFrameRun;
                frameCounter = 0;
            }
            break;
        case STATE_IDLE:
            if (frameCounter >= frameSpeedIdle) {
                player->currentFrame = (player->currentFrame + 1) % maxFrameIdle;
                frameCounter = 0;
            }
            break;
        case STATE_JUMPING:
            if (frameCounter >= frameSpeedJump) {
                player->jumpFrame = (player->jumpFrame + 1) % maxFrameJump;
                frameCounter = 0;
            }
            break;
        case STATE_FALLING:
            if (frameCounter >= frameSpeedFall) {
                player->fallFrame = (player->fallFrame + 1) % maxFrameFall;
                frameCounter = 0;
            }
            break;
        case STATE_DEAD:
            if (frameCounter >= frameSpeedDeath && !player->stopDeathAnim) {
                player->currentFrame++;
                if (player->currentFrame >= maxFrameDeath) {
                    player->currentFrame = maxFrameDeath - 1;
                    player->stopDeathAnim = 1;
                }
                frameCounter = 0;
            }
            break;
    }
    
    switch(player->stateDust) {
        case STATE_BEFORE_JUMP_DUST:
            if (frameDustCounter >= frameSpeedBeforeJump) {
                player->dustFrame++;
                if (player->dustFrame >= maxFrameBeforeAfterJump) {
                    player->dustFrame = 0;
                    player->state = STATE_JUMPING;
                    player->stateDust = STATE_NO_DUST;
                }
                frameDustCounter = 0;
            }
            break;
        case STATE_AFTER_JUMP_DUST:
            if (frameDustCounter >= frameSpeedAfterJump) {
                player->dustFrame++;
                if (player->dustFrame >= maxFrameBeforeAfterJump) {
                    player->dustFrame = 0;
                    player->state = STATE_IDLE;
                    player->stateDust = STATE_NO_DUST;
                }
                frameDustCounter = 0;
            }
            break;
        case STATE_NO_DUST:
            break;
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
            if (map[y][x] > 0 && map[y][x] <= 20) {
                // коллизия по X
                if (player->velocity.x > 0 && dir == 0) {
                    player->position.x = x * tileSize - player->tileSize - 0.1f;
                }
                if (player->velocity.x < 0 && dir == 0) {
                    player->position.x = x * tileSize + tileSize + 0.1f;
                }
                
                // коллизия по Y
                if (player->velocity.y > 0 && dir == 1) {
                    player->position.y = y * tileSize - player->tileSize - 0.1f;
                    player->onGround = 1;
                    player->velocity.y = 0.1f;
                }
                if (player->velocity.y < 0 && dir == 1) {
                    player->position.y = y * tileSize + tileSize + 0.01f;
                    player->velocity.y = 0;
                }
            } else if (map[y][x] == 28) {  
                if (player->velocity.x > 0 && dir == 0) {
                    player->position.x = x * tileSize - player->tileSize - 0.1f;
                }
                if (player->velocity.x < 0 && dir == 0) {
                    player->position.x = x * tileSize + tileSize + 0.1f;
                    player->velocity.x += 16.0f;
                }

                if (dir == 1) {
                    player->position.y = y * tileSize - player->tileSize - 0.1f;
                    player->velocity.y -= 2.5f;
                    player->health -= 1;
                }
            }
        }
    }
}

void drawPlayer(Player *player) {
    // Отрисовка эффектов пыли
    if (player->stateDust == STATE_BEFORE_JUMP_DUST || player->stateDust == STATE_AFTER_JUMP_DUST) {
        frameRectDust.x = (float)player->dustFrame * (float)textureBeforeJump.width / maxFrameBeforeAfterJump;
        Texture2D dustTexture = player->stateDust == STATE_BEFORE_JUMP_DUST ? textureBeforeJump : textureAfterJump;
        DrawTextureRec(dustTexture, frameRectDust, dustPosition, WHITE);
    }

    // Отрисовка игрока
    frameRect.width = player->flip ? -fabs(frameRect.width) : fabs(frameRect.width);
    switch (player->state) {
        case STATE_ATTACKING:
            frameRectAttack.width = player->flip ? -fabs(frameRectAttack.width) : fabs(frameRectAttack.width);
            frameRectAttack.x = player->currentAttackFrame * (textureAttack.width / maxFrameAttack);
            DrawTextureRec(textureAttack, frameRectAttack, 
                         (Vector2){ player->flip ? player->position.x - (textureAttack.width / maxFrameAttack - player->tileSize) : 
                                                  player->position.x - (textureAttack.width / maxFrameAttack - player->tileSize) / player->tileSize, 
                                  player->position.y - (textureAttack.height - player->tileSize)}, WHITE);
            break;    
        case STATE_RUNNING:
            frameRect.x = player->currentFrame * (textureRun.width / maxFrameRun);
            DrawTextureRec(textureRun, frameRect, 
                          (Vector2){player->position.x - (textureIdle.width / maxFrameIdle - player->tileSize) / 2, 
                                   player->position.y - (textureIdle.height - player->tileSize)}, WHITE);
            break;
        case STATE_IDLE:
            frameRect.x = player->currentFrame * (textureIdle.width / maxFrameIdle);
            DrawTextureRec(textureIdle, frameRect, 
                         (Vector2){player->position.x - (textureIdle.width / maxFrameIdle - player->tileSize) / 2, 
                                  player->position.y - (textureIdle.height - player->tileSize)}, WHITE);
            break;
        case STATE_JUMPING:
            frameRect.x = player->jumpFrame * (textureJump.width / maxFrameJump);
            DrawTextureRec(textureJump, frameRect, 
                          (Vector2){player->position.x - (textureJump.width / maxFrameJump - player->tileSize) / 2, 
                                   player->position.y - (textureJump.height - player->tileSize)}, WHITE);
            break;
        case STATE_FALLING:
            frameRect.x = (float)player->fallFrame * (float)textureFall.width / maxFrameFall;
            DrawTextureRec(textureFall, frameRect, 
                         (Vector2){player->position.x - (textureFall.width / maxFrameIdle - player->tileSize) / 2, 
                                  player->position.y - (textureFall.height - player->tileSize)}, WHITE);
            break;
        case STATE_DEAD:
            if (!player->stopDeathAnim) {
                frameRect.x = (float)player->currentFrame * (float)textureDeath.width / maxFrameDeath;
                DrawTextureRec(textureDeath, frameRect, 
                             (Vector2){player->position.x - (textureDeath.width / maxFrameDeath - player->tileSize) / 2, 
                                      player->position.y - (textureDeath.height - player->tileSize)}, WHITE);
            }
            break;
    }
}

void unloadPlayer() {
    if (textureIdle.id != 0) {
        UnloadTexture(textureIdle);
    }
    if (textureRun.id != 0) {
        UnloadTexture(textureRun);
    }
    if (textureFall.id != 0 ) {
        UnloadTexture(textureFall);
    }
    if (textureJump.id != 0) {
        UnloadTexture(textureJump);
    }
    if (textureAfterJump.id != 0) {
        UnloadTexture(textureAfterJump);
    }
    if (textureBeforeJump.id != 0) {
        UnloadTexture(textureBeforeJump);
    }
    if (textureAttack.id != 0) {
        UnloadTexture(textureAttack);
    }
    UnloadSound(soundRun);
    UnloadSound(soundAttack);
    UnloadSound(soundJump);
    UnloadSound(soundLose);
}