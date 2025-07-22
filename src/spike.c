#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "player.h"
#include "spike.h"

const int spikeTileSize = 16;
Texture2D textureSpikesTrap;

void initializeSpike(Spike *spike, float x, float y) {
    spike->position = (Vector2){x, y};

    if (textureSpikesTrap.id == 0) {
        textureSpikesTrap = LoadTexture("resource/miscellaneous sprites/spikes.png");
        if (textureSpikesTrap.id == 0) {
            exit(1);
        }
    }
    textureSpikesTrap.height = -textureSpikesTrap.height;
}

int CheckCollisionPlayerSpike(Player *player, Spike *spike) {
    Rectangle playerRect = {
        player->position.x,
        player->position.y,
        player->tileSize,
        player->tileSize
    };
    
    Rectangle spikeRect = {
        spike->position.x,
        spike->position.y,
        spikeTileSize,
        spikeTileSize
    };
    
    if (!CheckCollisionRecs(playerRect, spikeRect)) {
        return 0;
    }
    
    float playerBottom = player->position.y + player->tileSize;
    float spikeTop = spike->position.y;
    float collisionMargin = 16.0f;
    
    return (playerBottom <= spikeTop + collisionMargin);
}

void updateSpike(Spike *spike, Player* player) {
    if (CheckCollisionPlayerSpike(player, spike)) {
        if (player->onGround || player->velocity.y > 0) {
            player->velocity.y = -player->jumpHeight * 1.3f;
            player->onGround = 0;
            player->state = STATE_JUMPING;
            
             player->health--;

            if (player->position.x < spike->position.x) {
                player->velocity.x = -player->jumpHeight * 0.3f;
            } else {
                player->velocity.x = player->jumpHeight * 0.3f;
            }
        }
    }
}

void drawSpike(Spike *spike) {
    DrawTextureRec(textureSpikesTrap, (Rectangle){0, 0, 16, -16}, spike->position, WHITE);
}

void unloadSpike() {
    if (textureSpikesTrap.id != 0) {
        UnloadTexture(textureSpikesTrap);
    }
}