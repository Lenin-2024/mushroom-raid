#include "player.h"
#include "stone.h"
#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>

Texture2D textureStone;
Vector2 oldStonePos;

void initializeStone(float x, float y, Stone *stone) {
    stone->position = (Vector2) {
        x, y
    };
    
    stone->onGround = 0;
    stone->tileSize = 15.9f;

    if (textureStone.id == 0) {
        textureStone = LoadTexture("resource/miscellaneous sprites/stone.png");
        if (textureStone.id == 0) {
            exit(1);
        }
    }
}

void updateStone(Player *player, int **map, Stone *stone) {
    if (!stone->onGround) {
        stone->velocity.y += 0.2f;
    }
    
    oldStonePos = stone->position;

    if (player->velocity.x > 0 && 
        (player->position.x + player->tileSize >= stone->position.x) &&
        (player->position.x + player->tileSize <= stone->position.x + stone->tileSize) &&
        (player->position.y < stone->position.y + stone->tileSize) &&
        (player->position.y + player->tileSize > stone->position.y)) {
        stone->velocity.x = player->velocity.x;
        
        stone->position.x += stone->velocity.x;
        collisionStone(stone, map, 0, 16);
        
        if (oldStonePos.x == stone->position.x) {
            player->position.x -= player->velocity.x;
        }
    } else if (player->velocity.x < 0 && 
            (player->position.x <= stone->position.x + stone->tileSize) &&
            (player->position.x >= stone->position.x) &&
            (player->position.y < stone->position.y + stone->tileSize) &&
            (player->position.y + player->tileSize > stone->position.y)) {
        stone->velocity.x = player->velocity.x;
        
        stone->position.x += stone->velocity.x;
        collisionStone(stone, map, 0, 16);

        if (oldStonePos.x == stone->position.x) {
            player->position.x -= player->velocity.x;
        }
    } else {
        stone->velocity.x = 0;
    }

    Rectangle playerFeet = {
        player->position.x + 3, player->position.y + player->tileSize - 2,
        player->tileSize - 6, 4
    };
    
    Rectangle stoneTop = {
        stone->position.x + 2, stone->position.y,
        stone->tileSize - 4, 6
    };
    
    if (CheckCollisionRecs(playerFeet, stoneTop) && player->velocity.y >= 0) {
        player->state = STATE_IDLE;
        player->velocity.y = 0;
        player->onGround = 1;
        player->position.y = stone->position.y - player->tileSize - 0.1f;
    }

    stone->position.y += stone->velocity.y;
    collisionStone(stone, map, 1, 16);
}

void collisionStone(Stone *stone, int **map, int dir, int tileSize) {
    int startX = (int)(stone->position.x / tileSize);
    int endX = (int)((stone->position.x + stone->tileSize) / tileSize);
    int startY = (int)(stone->position.y / tileSize);
    int endY = (int)((stone->position.y + stone->tileSize) / tileSize);
    
    stone->onGround = 0;

    for (int y = startY; y <= endY; y++) {
        for (int x = startX; x <= endX; x++) {
            if (map[y][x] > 0 && map[y][x] <= 20) {
                // коллизия по X
                if (stone->velocity.x > 0 && dir == 0) {
                    stone->position.x = x * tileSize - stone->tileSize - 0.1f;
                }
                if (stone->velocity.x < 0 && dir == 0) {
                    stone->position.x = x * tileSize + tileSize + 0.1f;
                }

                // коллизия по Y
                if (stone->velocity.y > 0 && dir == 1) {
                    stone->position.y = y * tileSize - stone->tileSize - 0.1f;
                    stone->onGround = 1;
                }
                if (stone->velocity.y < 0 && dir == 1) {
                    stone->position.y = y * tileSize + tileSize + 0.01f;
                    stone->velocity.y = 0;
                }
            }
        }
    }
}

void drawStone(Stone *stone) {
    DrawTexture(textureStone, stone->position.x, stone->position.y, RAYWHITE); 
}

void unloadStone() {
    if (textureStone.id != 0) {
        UnloadTexture(textureStone);
    }
}