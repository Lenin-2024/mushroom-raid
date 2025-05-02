#ifndef VASE_H
#define VASE_H

#include "player.h"
#include <raylib.h>

typedef enum  {
    STATE_VASE_DEAD,
    STATE_ALIVE,
    STATE_VASE_DESTROYED
} VaseState;

extern Texture2D textureVase;
extern Texture2D textureVaseDeath;
extern int maxFrameVaseDeath;

typedef struct Vase {
    Vector2 position;
    VaseState state;
    float frameCounter;
    int currentFrame;
} Vase;

void initializeVase(float x, float y, Vase *vase);
void updateVase(Player *player, Vase *vase);
void drawVase(Vase *vase);
void unloadVase();

#endif