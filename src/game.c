#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"
#include "money.h"
#include "player.h"
#include "map.h"
#include "slime.h"

const int windowWidth = 640;
const int windowHeight = 480;
const int backGroundSize = 16;

Texture2D midground;
Texture2D foreground;

void initTexture();
void removeInactiveMoney();
void removeInactiveSlime();
void unloadTextureAndMemory(int **map, int yMax);
void initializeAll(int **map, Vector2 *playerStartPosition, Camera2D *camera, int yMax, int xMax);
void draw(Camera2D *camera, int **map, int yMax, int xMax);
void update(int **map, Camera2D *camera);

Texture2D textureGround;
Rectangle arrayTexture[21];

Player player = { 0 };

int gameOver = 0;
int gamePause = 0;

int countMoney = 0;
Money *arrayMoney = NULL;
int countSlime = 0;
Slime *arraySlime = NULL;

int main(void) {
    int yMax = 15;
    int xMax = 25;

    int **map = initializeMap(xMax, yMax);

    Vector2 playerStartPosition = { 0 };
    Camera2D camera = { 0 };

    initializeAll(map, &playerStartPosition, &camera, yMax, xMax);

    while(!WindowShouldClose()) {
        if (gameOver == 0) {
            if (IsKeyPressed(KEY_Q)) {
                gameOver = 1;
                player.health = 3;
                player.stopDeathAnim = 0;
            }
            update(map, &camera);
            draw(&camera, map, yMax, xMax);
        } else if (gameOver == 1) {
            player.position = playerStartPosition;
            gameOver = 0;
        }
    }
    unloadTextureAndMemory(map, yMax);
    return 0;
}


void update(int **map, Camera2D *camera) {
    camera->target = (Vector2){ player.position.x + 20.0f, player.position.y + 20.0f };

    updatePlayer(&player, 1.5f, map, backGroundSize);
    updateScrolling(player.position.x);
    
    for (int i = 0; i < countMoney; i++) {
        updateMoney(&arrayMoney[i], player.position.x, player.position.y, 14);
    }
    removeInactiveMoney();

    for (int i = 0; i < countSlime; i++) {
        updateSlime(&arraySlime[i], player.position.x, player.position.y, &player.velocity.y, player.jumpHeight, &player.health, 14, map);
    }
    removeInactiveSlime();
}

void draw(Camera2D *camera, int **map, int yMax, int xMax) {
    BeginDrawing();
        ClearBackground(RAYWHITE);
        drawBackGround(midground, foreground);                
            BeginMode2D(*camera);
                drawMap(map, xMax, yMax, backGroundSize, sizeof(arrayTexture)/sizeof(arrayTexture[0]), textureGround, arrayTexture);
                for (int i = 0; i < countMoney; i++) {
                    drawMoney(&arrayMoney[i]);
                }
                for (int i = 0; i < countSlime; i++) {
                    drawSlime(&arraySlime[i]);
                }
                drawPlayer(&player);
            EndMode2D();
        DrawText(TextFormat("player.position = [%f, %f]", player.position.x, player.position.y), 1, 15, 20, BLACK);
        DrawText(TextFormat("player.health = [%d]", player.health), 1, 33, 20, BLACK);
        DrawFPS(1, 1);
    EndDrawing();
}

void initializeAll(int **map, Vector2 *playerStartPosition, Camera2D *camera, int yMax, int xMax) {
    camera->target = (Vector2){ player.position.x + 20.0f, player.position.y + 20.0f };
    camera->offset = (Vector2){ windowWidth / 2.0f, windowHeight / 2.0f };
    camera->rotation = 0.0f;
    camera->zoom = 3.8f;

    arrayMoney = calloc(sizeof(Money), countMoney);
    arraySlime = calloc(sizeof(Slime), countSlime);

    // иницилизация окна
    InitWindow(windowWidth, windowHeight, "Мухоморный Рейд");
    SetTargetFPS(60);

    loadMap("save_map.txt", map, xMax, yMax);
    // иницилизация текстур
    midground = LoadTexture("resource/tiles and background_foreground (new)/bg_0.png");
    foreground = LoadTexture("resource/tiles and background_foreground (new)/bg_1.png");
    textureGround = LoadTexture("resource/tiles and background_foreground (new)/tileset.png");

    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 6; x++) {

            int index = y * 6 + x;
            arrayTexture[index] = (Rectangle) {
                x * backGroundSize, y * backGroundSize,
                backGroundSize, backGroundSize
            };
        }
    }

    // иницилизация карты
    for (int y = 0; y < yMax; y++) {
        for (int x = 0; x < xMax; x++) {
            if (map[y][x] == 19) {
                initializePlayer(x * backGroundSize, y * backGroundSize, &player);
                *playerStartPosition = (Vector2){ x * backGroundSize, y * backGroundSize };  
                map[y][x] = 0;
            }

            if (map[y][x] == 20) {
                countMoney++; 
                //printf("%d\n", countMoney);
                Money *money = malloc(sizeof(Money));
                if (money == NULL) {
                    free(money);
                }

                Money *temp = realloc(arrayMoney, sizeof(Money) * countMoney);
                if (temp == NULL) {
                    free(arrayMoney);
                    free(temp);
                } else {
                    arrayMoney = temp;
                }

                initializeMoney(x * backGroundSize + 4, y * backGroundSize + 4, money);
                if (countMoney != 0) {
                    arrayMoney[countMoney - 1] = *money;
                } else {
                    arrayMoney[0] = *money;
                }

                map[y][x] = 0;
                free(money);
            }
            
            if (map[y][x] == 21) {
                countSlime++;
                //printf("%d\n", countSlime);
                Slime *slime = malloc(sizeof(Slime));
                if (slime == NULL) {
                    free(slime);
                }

                Slime *temp = realloc(arraySlime, sizeof(Slime) * countSlime);
                if (temp == NULL) {
                    free(arraySlime);
                    free(temp);
                } else {
                    arraySlime = temp;
                }
                
                initializeSlime(x * backGroundSize, y * backGroundSize, slime);
                if (countSlime != 0) {
                    arraySlime[countSlime - 1] = *slime;
                } else {
                    arraySlime[0] = *slime;
                }

                map[y][x] = 0;
                free(slime);
            }

        }
    }
}


void unloadTextureAndMemory(int **map, int yMax) {
    UnloadTexture(textureRun);
    UnloadTexture(textureIdle);
    UnloadTexture(textureGround);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    unloadPlayerTexture();
    unloadMoneyTexture();
    unloadSlimeTexture();

    for (int i = 0; i < yMax; i++) {
        free(map[i]);
    }

    free(map);
    free(arrayMoney);

    CloseWindow();
}


void removeInactiveMoney() {
    int newCountMoney = 0;

    // создаем временный массив для активных монеток
    Money *newArrayMoney = malloc(sizeof(Money) * countMoney);
    if (newArrayMoney == NULL) {
        return;
    }

    // копируем активные монетки в новый массив
    for (int i = 0; i < countMoney; i++) {
        if (arrayMoney[i].isAlive == 1) {
            newArrayMoney[newCountMoney] = arrayMoney[i];
            newCountMoney++;
        }
    }

    free(arrayMoney);

    // обновляем указатель и количество монеток
    arrayMoney = newArrayMoney;
    countMoney = newCountMoney;
}

void removeInactiveSlime() {
    int newCountSlime = 0;
    Slime *newArraySlime = malloc(sizeof(Slime) * countSlime);
    if (newArraySlime == NULL) {
        return;
    }

    for (int i = 0; i < countSlime; i++) {
        if (arraySlime[i].isAlive == 1) {
            newArraySlime[newCountSlime] = arraySlime[i];
            newCountSlime++;
        }
    }

    free(arraySlime);

    arraySlime = newArraySlime;
    countSlime = newCountSlime;
}