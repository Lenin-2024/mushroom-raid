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
extern Texture2D textureAttack;

extern Sound soundRun;
extern Sound soundAttack;
extern Sound soundJump;

extern const int maxFrameRun;
extern const int maxFrameIdle;
extern const int maxFrameJump;
extern const int maxFrameFall;
extern const int maxFrameDeath;
extern const int maxFrameBeforeAfterJump;
extern const int maxFrameAttack;

extern const float gravity;
extern int fallCheck;

extern Vector2 dustPosition;


typedef enum {
    STATE_IDLE,
    STATE_RUNNING,
    STATE_JUMPING,
    STATE_FALLING,
    STATE_ATTACKING,
    STATE_DEAD
 } PlayerState;

typedef enum {
    STATE_NO_DUST,
    STATE_BEFORE_JUMP_DUST,
    STATE_AFTER_JUMP_DUST
} StateDust;


typedef struct Player {
    Vector2 position;
    Vector2 velocity;
    float jumpHeight;
    float attackWidth;
    int tileSize;
    int health;
    int currentFrame;
    int currentAttackFrame;
    int dustFrame;
    int flip;
    int onGround;
    int jumpFrame;
    int fallFrame;
    int stopDeathAnim;
    int dustBeforeAnimationActive;
    int dustAfterAnimationActive;
    int isAttack;
    PlayerState state;
    StateDust stateDust;
} Player;

void initializePlayer(float x, float y, Player *player);
void updatePlayer(Player *player, float speed, int **map, int tileSize);
void drawPlayer(Player *player);
void collision(Player *player, int **map, int dir, int tileSize);
void unloadPlayer();

#endif