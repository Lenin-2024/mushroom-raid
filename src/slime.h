#ifndef SLIME_H
#define SLIME_H

#include "raylib.h"
#include "player.h"

extern const int slimeTileSize;

typedef struct Slime {
    Vector2 position;
    Vector2 velocity;
    int currentFrame;
    int isAlive;
    float frameCounter;
    int isActivatedDeath;
    int flip;
    int dir;
} Slime;

extern float playerDamage;
extern const float slimeSpeed;
extern const int maxFrameSlimeIdle;
extern const int maxFrameSlimeRun;
extern const int maxFrameSlimeDeath;

extern Texture2D textureSlimeIdle;
extern Texture2D textureSlimeRun;
extern Texture2D textureSlimeDeath;

extern Rectangle frameRectSlimeRun;
extern Rectangle frameRectSlimeIdle;

extern Sound deathSound;

void initializeSlime(float x, float y, Slime *slime);
void updateSlime(Slime *slime, Player* player, int **map);
void drawSlime(Slime *slime);
void unloadSlimeTexture();
void collisionWithMap(Slime *slime, int **map, int tileSize);

#endif