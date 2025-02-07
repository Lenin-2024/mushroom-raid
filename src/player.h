#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

#define MAP_WIDTH 20
#define MAP_HEIGHT 15

extern Texture2D textureRun;
extern Texture2D textureIdle;
extern Texture2D textureFall;
extern Texture2D textureJump;

extern const int playerTileSize;
extern const int maxFrameRun;
extern const int maxFrameIdle;
extern const int maxFrameJump;
extern const int maxFrameFall;
extern const float gravity;

typedef struct Player {
    Vector2 position;
    Vector2 velocity;
    int health;
    int currentFrame;
    int flip;
    int onGround;
    int jumpFrame;
    int fallFrame;
} Player;

void initializePlayer(float x, float y, Player *player);
void updatePlayer(Player *player, float speed, int **map, int tileSize);
void drawPlayer(Player *player);
void collision(Player *player, int **map, int dir, int tileSize);
void unloadPlayerTexture();

#endif