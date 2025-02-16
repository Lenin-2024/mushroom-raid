#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

#define MAP_WIDTH 20
#define MAP_HEIGHT 15

extern Texture2D textureRun;
extern Texture2D textureIdle;
extern Texture2D textureFall;
extern Texture2D textureJump;
extern Texture2D textureDeath;
extern Texture2D textureAfterJump;
extern Texture2D textureBeforeJump;

extern const int maxFrameRun;
extern const int maxFrameIdle;
extern const int maxFrameJump;
extern const int maxFrameFall;
extern const int maxFrameDeath;
extern const int maxFrameBeforeAfterJump;

extern const float gravity;

extern int fallCheck;

extern Vector2 dustPosition;

typedef struct Player {
    Vector2 position;
    Vector2 velocity;
    float jumpHeight;
    int tileSize;
    int health;
    int currentFrame;
    int dustFrame;
    int flip;
    int onGround;
    int jumpFrame;
    int fallFrame;
    int stopDeathAnim;
    int dustAnimationActive;
} Player;

void initializePlayer(float x, float y, Player *player);
void updatePlayer(Player *player, float speed, int **map, int tileSize);
void drawPlayer(Player *player);
void collision(Player *player, int **map, int dir, int tileSize);
void unloadPlayerTexture();

#endif