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
void unloadTexture();

Texture2D textureGround;
Rectangle arrayTexture[21];

int gameOver = 0;
int gamePause = 0;

// монетки
int countMoney = 0;
Money *arrayMoney = NULL;
// враги
int countSlime = 0;
Slime *arraySlime = NULL;

int main(void) {
    int yMax = 15;
    int xMax = 25;
    int **map = initializeMap(xMax, yMax);
    arrayMoney = calloc(sizeof(Money), countMoney); // память для монеток

    InitWindow(windowWidth, windowHeight, "project1");
    initTexture();
    loadMap("save_map.txt", map, xMax, yMax);
    
    SetTargetFPS(60);

    Texture2D textureGround = LoadTexture("resource/tiles and background_foreground (new)/tileset.png");

    //DisableCursor();
    Player player = { 0 };
    Vector2 playerStartPosition = { 0 };

    for (int y = 0; y < yMax; y++) {
        for (int x = 0; x < xMax; x++) {
            if (map[y][x] == 19) {
                initializePlayer(x * backGroundSize, y * backGroundSize, &player);
                playerStartPosition = (Vector2){ x * backGroundSize, y * backGroundSize };  
                map[y][x] = 0;
            }

            if (map[y][x] == 20) {
                countMoney++;
                printf("%d\n", countMoney);
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
                printf("%d\n", countSlime);
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

    Camera2D camera = { 0 };
    camera.target = (Vector2){ player.position.x + 20.0f, player.position.y + 20.0f };
    camera.offset = (Vector2){ windowWidth / 2.0f, windowHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 3.8f;

    while(!WindowShouldClose()) {
        if (gameOver == 0) {
            if (IsKeyPressed(KEY_Q)) {
                gameOver = 1;
            }

            camera.target = (Vector2){ player.position.x + 20.0f, player.position.y + 20.0f };

            updatePlayer(&player, 1.5f, map, backGroundSize);
            updateScrolling(player.position.x);

            // обновление монет
            for (int i = 0; i < countMoney; i++) {
                updateMoney(&arrayMoney[i], player.position.x, player.position.y, 14);
            }
            removeInactiveMoney();

            for (int i = 0; i < countSlime; i++) {
               updateSlime(&arraySlime[i], player.position.x, player.position.y, &player.velocity.y, player.jumpHeight, 14);
            }

            BeginDrawing();
                ClearBackground(RAYWHITE);
                drawBackGround(midground, foreground);
                
                BeginMode2D(camera);
                    drawMap(map, xMax, yMax, backGroundSize, sizeof(arrayTexture)/sizeof(arrayTexture[0]), textureGround, arrayTexture);
                    // отрисовка монеток
                    for (int i = 0; i < countMoney; i++) {
                        drawMoney(&arrayMoney[i]);
                    }
                    // отрисовка слайма
                    for (int i = 0; i < countSlime; i++) {
                        drawSlime(&arraySlime[i]);
                    }

                    drawPlayer(&player);
                EndMode2D();

                DrawText(TextFormat("player.position = [%f, %f]", player.position.x, player.position.y), 1, 15, 20, BLACK);
                DrawFPS(1, 1);
            EndDrawing();
        }

        else if (gameOver == 1) {
            player.position = playerStartPosition;
            gameOver = 0;
        }
    }

    unloadTexture();

    for (int i = 0; i < yMax; i++) {
        free(map[i]);
    }

    free(map);
    free(arrayMoney);

    CloseWindow();
    return 0;
}

void unloadTexture() {
    UnloadTexture(textureRun);
    UnloadTexture(textureIdle);
    UnloadTexture(textureGround);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    unloadPlayerTexture();
    unloadMoneyTexture();
}

void initTexture() {
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