#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "player.h"
#include "vase.h"

Texture2D textureVase;
Texture2D textureVaseDeath;
Rectangle frameRectVaseDeath;

int maxFrameVaseDeath = 5;
const float frameSpeedDeath = 0.25f;

void initializeVase(float x, float y, Vase *vase) {
    vase->position = (Vector2) {
        x, y
    };
    vase->state = STATE_ALIVE;
    vase->frameCounter = 0.0f;
    vase->currentFrame = 0.0f;

    textureVase = LoadTexture("resource/miscellaneous sprites/vase.png");
    if (textureVase.id == 0) {
        exit(1);
    }

    textureVaseDeath = LoadTexture("resource/miscellaneous sprites/vase_breaking_anim_strip_5.png");
    if (textureVaseDeath.id == 0) {
        exit(1);
    }

    frameRectVaseDeath = (Rectangle) {
        0, 0,
        textureVaseDeath.width / 5, textureVase.height
    };

}

void updateVase(Player *player, Vase *vase) {
    float attackEndX = player->position.x + (player->attackWidth > 0 ? player->attackWidth : 0);
    float attackStartX = player->position.x + (player->attackWidth < 0 ? player->attackWidth + player->tileSize : 0);
    if ((attackStartX < (vase->position.x + textureVase.width)) && 
        (attackEndX > vase->position.x) && 
        (player->position.y < (vase->position.y + textureVase.height)) && 
        ((player->position.y + player->tileSize) > vase->position.y) && 
        (player->isAttack == 1) && (vase->state == STATE_ALIVE)) {
        vase->state = STATE_VASE_DEAD;
    }
    
    if (vase->state == STATE_VASE_DEAD) {
        vase->frameCounter += GetFrameTime();
        if (vase->frameCounter >= frameSpeedDeath) {
            vase->currentFrame++;
            vase->frameCounter = 0;
            if (vase->currentFrame >= maxFrameAttack) {
                vase->state = STATE_VASE_DESTROYED;
                vase->frameCounter = 0;
                vase->currentFrame = -1;
            }
        }
    }

    
}

void drawVase(Vase *vase) {
    switch (vase->state == STATE_ALIVE) {
        case STATE_ALIVE:
            DrawTexture(textureVase, vase->position.x, vase->position.y, RAYWHITE);
            break;
        case STATE_VASE_DEAD:
            frameRectVaseDeath.x = (float)vase->currentFrame * textureVaseDeath.width / maxFrameVaseDeath;
            DrawTextureRec(textureVaseDeath, frameRectVaseDeath, (Vector2){vase->position.x, vase->position.y}, WHITE);
            break;
        case STATE_VASE_DESTROYED:
            break;
    }
}

void unloadVase() {
    if (textureVase.id != 0) {
        UnloadTexture(textureVase);
        textureVase.id = 0;
    }

    if (textureVaseDeath.id != 0) {
        UnloadTexture(textureVaseDeath);
        textureVaseDeath.id = 0;
    }
}