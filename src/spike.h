#ifndef SPIKE_H
#define SPIKE_H

#include "raylib.h"
#include "player.h"

extern const int spikeTileSize;
extern float playerDamage;

typedef struct Spike {
    Vector2 position;
} Spike;

extern Texture2D textureSpikesTrap;

void initializeSpike(Spike *spike, float x, float y);
void updateSpike(Spike *spike, Player* player);
void drawSpike(Spike *spike);
void unloadSpike();
int CheckCollisionPlayerSpike(Player *player, Spike *spike);

#endif