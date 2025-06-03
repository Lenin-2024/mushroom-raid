#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"
#include "money.h"
#include "player.h"
#include "map.h"
#include "slime.h"
#include "bomber.h"
#include "bomb.h"
#include "vase.h"
#include "stone.h"

const int windowWidth = 640;
const int windowHeight = 480;
const int backGroundSize = 16;

int unlockLevel = 1;
int startGame = 0;
int stratAnimCircle = 0;
float radiusCircle = 0.f;
float speedCircle = 6.f;

typedef struct {
    Texture2D texture;
    Vector2 position;
} Door;

void initTexture();
void removeInactiveMoney();
void removeInactiveSlime();
void removeInactiveBomber();
void unloadTextureAndMemory(int **map, int yMax, Door *door);
void initialize(int **map, Vector2 *playerStartPosition, Camera2D *camera, int yMax, int xMax, int initAll, char *nameMap, Door *door);
void draw(Camera2D *camera, int **map, int yMax, int xMax, Door *door);
void update(int **map, Camera2D *camera, Door *door);
void game(int **map, Vector2 *playerStartPosition, int yMax, int xMax, Door *door);
void drawMenu(int **map, Vector2 *playerStartPosition, int yMax, int xMax, Door *door);

Texture2D textureHealthHud;
Texture2D textureMenu;

Rectangle arrayButtonNum[18];

Rectangle healthRect;

Player player = { 0 };
Camera2D camera = { 0 };

int gameOver = 0;
int gamePause = 0;

int countMoney = 0;
Money *arrayMoney = NULL;

int countSlime = 0;
Slime *arraySlime = NULL;

int countBomber = 0;
Bomber *arrayBomber = NULL;

int countVase = 0;
Vase *arrayVase = NULL;

int countStone = 0;
Stone *arrayStone = NULL;

Bomb arrayBomb[10] = {0};
Bomb *currentBomb = NULL;

int main(int argc, char *argv[]) {
    int yMax = 15;
    int xMax = 25;
    Door door;

    int **map = initializeMap(xMax, yMax);

    Vector2 playerStartPosition = { 0 };
    initialize(map, &playerStartPosition, &camera, yMax, xMax, 1, "level_0.txt", &door);

    while(!WindowShouldClose()) {
        if (startGame != 1) {
            drawMenu(map, &playerStartPosition, yMax, xMax, &door);
            if (radiusCircle >= 0) {
                radiusCircle -= speedCircle;
            }
        } else if (startGame == 1) {
            game(map, &playerStartPosition, yMax, xMax, &door);
        }
    }

    unloadTextureAndMemory(map, yMax, &door);
    return 0;
}

void drawMenu(int **map, Vector2 *playerStartPosition, int yMax, int xMax, Door *door) {
    for (int i = 0; i < unlockLevel; i++) {
        Rectangle sourceMenu = { 
            i * 32 * 2.2f + windowWidth / 2 - (32 * unlockLevel), 
            windowHeight / 2 - 32,
            textureMenu.width / 15 * 2.0f, 
            textureMenu.height / 10 * 2.0f 
        };

        if (IsMouseButtonDown(0) && 
            GetMouseX() > sourceMenu.x &&
            GetMouseX() < (sourceMenu.x + sourceMenu.width) &&
            GetMouseY() > sourceMenu.y &&
            GetMouseY() < (sourceMenu.y + sourceMenu.height)) {
            startGame = 1;
            if (i == 0) {
                initialize(map, playerStartPosition, &camera, yMax, xMax, 0, "level_0.txt", door);
            } else if (i == 1) {
                initialize(map, playerStartPosition, &camera, yMax, xMax, 0, "level_1.txt", door);
            }
        }
    }

    BeginDrawing();
        ClearBackground(RAYWHITE);
        drawBackGround();
        for (int i = 0; i < unlockLevel; i++) {
            Rectangle sourceMenu = { i * 32 * 2.2f + windowWidth / 2 - (32 * unlockLevel), windowHeight / 2 - 32, textureMenu.width / 15 * 2.0f, textureMenu.height / 10 * 2.0f };
            DrawTexturePro(textureMenu, (Rectangle){12 * 32, i * 32, 32, 32}, sourceMenu, (Vector2){0, 0}, 0.0f, WHITE);
        }

        DrawCircle(windowWidth / 2, windowHeight / 2, radiusCircle, BLACK);
    EndDrawing();
}

void game(int **map, Vector2 *playerStartPosition, int yMax, int xMax, Door *door) {
    if (gameOver == 0) {
        if (IsKeyPressed(KEY_Q)) {
            startGame = 0;
        }
        update(map, &camera, door);
        draw(&camera, map, yMax, xMax, door);
    } else if (gameOver == 1) {
        player.position = *playerStartPosition;
        gameOver = 0;
        
        startGame = 1;
    }
}

void update(int **map, Camera2D *camera, Door *door) {
    if ((player.state == STATE_DEAD) && (player.stopDeathAnim == 1)) {
        startGame = 0;
    }

    camera->target = (Vector2) {
        player.position.x + (player.tileSize / 2), 
        player.position.y + (player.tileSize / 2)
    };

    if ((player.position.x <= door->position.x + door->texture.width) &&
        (player.position.x + player.tileSize >= door->position.x) &&
        (player.position.y + player.tileSize >= door->position.y) &&
        (player.position.y <= door->position.y + door->texture.height) &&
        IsKeyPressed(KEY_UP)) {
            unlockLevel++;
            stratAnimCircle = 1;
            //startGame = 0;
    }

    if (stratAnimCircle == 0) {
        updatePlayer(&player, 1.5f, map, backGroundSize);

        updateScrolling(player.position.x);
        
        for (int i = 0; i < countMoney; i++) {
            updateMoney(&arrayMoney[i], player.position.x, player.position.y, 14);
        }
        removeInactiveMoney();

        for (int i = 0; i < countSlime; i++) {
            updateSlime(&arraySlime[i], &player, map);
        }
        removeInactiveSlime();
        
        for (int i = 0; i < countVase; i++) {
            updateVase(&player, &arrayVase[i]);
        }
        
        for (int i = 0; i < countStone; i++) {
            updateStone(&player, map, &arrayStone[i]);
        }

        for (int i = 0; i < sizeof(arrayBomb) / sizeof(arrayBomb[0]); i++) {
            if (arrayBomb[i].isActivated == 0) {
                currentBomb = &arrayBomb[i];
                break;
            }
        }

        for (int i = 0; i < sizeof(arrayBomb) / sizeof(arrayBomb[0]); i++) {
            if (arrayBomb[i].isActivated == 1) {
                updateBomb(&arrayBomb[i], &player, map);
            }
        }

        for (int i = 0; i < countBomber; i++) {
            updateBomber(&arrayBomber[i], &player, map);
            if (arrayBomber[i].isAttack == 1) {
                initializeBomb(arrayBomber[i].position.x, arrayBomber[i].position.y, currentBomb, player.position.x, player.position.y);
                arrayBomber[i].isAttack = 0;
            }
        }
        removeInactiveBomber();
    } else if (stratAnimCircle == 1) {
        if (radiusCircle <= windowHeight) {
            radiusCircle += speedCircle;
        } else {
            stratAnimCircle = 0;
            startGame = 0;
        }

    }
}

void draw(Camera2D *camera, int **map, int yMax, int xMax, Door *door) {
    BeginDrawing();
        ClearBackground(RAYWHITE);
        drawBackGround();                
            BeginMode2D(*camera);
                drawMap(map, xMax, yMax, backGroundSize);
                
                Rectangle sourceDoor = { door->position.x, door->position.y, door->texture.width, door->texture.height };
                DrawTexturePro(door->texture, (Rectangle){0,0,24,24}, sourceDoor, (Vector2){0, 0}, 0.0f, WHITE);

                for (int i = 0; i < countVase; i++) {
                    drawVase(&arrayVase[i]);
                }
                for (int i = 0; i < countMoney; i++) {
                    drawMoney(&arrayMoney[i]);
                }
                for (int i = 0; i < countBomber; i++) {
                    drawBomber(&arrayBomber[i]);
                }
                for (int i = 0; i < countStone; i++) {
                    drawStone(&arrayStone[i]);
                }
                for (int i = 0; i < countSlime; i++) {
                    drawSlime(&arraySlime[i]);
                }
                for (int i = 0; i < sizeof(arrayBomb) / sizeof(arrayBomb[0]); i++) {                  
                    if (arrayBomb[i].isActivated == 1) {
                        drawBomb(&arrayBomb[i]);
                    }
                }
                drawPlayer(&player);
            EndMode2D();

        for (int i = 0; i < player.health; i++) {
            Rectangle sourceHealth = { i * 16 * 2.2f + 2, 2, textureHealthHud.width * 2.0f, textureHealthHud.height * 2.0f };
            DrawTexturePro(textureHealthHud, healthRect, sourceHealth, (Vector2){0, 0}, 0.0f, WHITE);
        }

        DrawCircle(windowWidth / 2, windowHeight / 2, radiusCircle, BLACK);

        //DrawFPS(565, 0);
    EndDrawing();
}

void initialize(int **map, Vector2 *playerStartPosition, Camera2D *camera, 
                int yMax, int xMax, int initAll, char *nameMap, Door *door) {

    camera->target = (Vector2){
        player.position.x + (player.tileSize / 2), 
        player.position.y + (player.tileSize / 2)
    };
    
    door->position = (Vector2){-100, -100};
    countStone = 0;
    arrayStone = calloc(sizeof(Stone), countStone);
    countMoney = 0;
    arrayMoney = calloc(sizeof(Money), countMoney);
    countSlime = 0;
    arraySlime = calloc(sizeof(Slime), countSlime);
    countBomber = 0;
    arrayBomber = calloc(sizeof(Bomber), countBomber);
    countVase = 0;
    arrayVase = calloc(sizeof(Vase), countVase);

    for (int i = 0; i < sizeof(arrayBomb)/sizeof(arrayBomb[0]); i++) {
        arrayBomb[i].isActivated = 0;
    }

    if (initAll == 1) {
        InitWindow(windowWidth, windowHeight, "Мухоморный Рейд");
        SetTargetFPS(60);
        InitAudioDevice();

        camera->offset = (Vector2){ windowWidth / 2.0f, windowHeight / 2.0f };
        camera->zoom = 3.8f;

        // иницилизация текстур
        textureHealthHud = LoadTexture("resource/hud elements/hearts_hud.png");
        if (textureHealthHud.id == 0) {
            exit(1);
        }
        textureMenu = LoadTexture("resource/menu sprites/Green Buttons Icons.png");
        if (textureMenu.id == 0) {
            exit(1);
        }

        initTexture(backGroundSize);

        healthRect = (Rectangle){
            0, 0, textureHealthHud.width, textureHealthHud.height
        };
        
        door->texture = LoadTexture("resource/miscellaneous sprites/door.png");
        if (door->texture.id == 0) {
            exit(1);
        }
    }

    loadMap(nameMap, map, xMax, yMax);

    // Иницилизация карты
    for (int y = 0; y < yMax; y++) {
        for (int x = 0; x < xMax; x++) {
            //-----------------Игрок--------------------//
            if (map[y][x] == 19) {
                initializePlayer(x * backGroundSize, y * backGroundSize, &player);
                *playerStartPosition = (Vector2) {
                    x * backGroundSize, 
                    y * backGroundSize
                };  
                map[y][x] = 0;
            }
            //----------------Монетки--------------------//
            if (map[y][x] == 20) {
                countMoney++; 
                Money *money = malloc(sizeof(Money));
                if (money == NULL) {
                    countMoney--;
                    continue;
                }

                Money *temp = realloc(arrayMoney, sizeof(Money) * countMoney);
                if (temp == NULL) {
                    free(money);
                    countMoney--;
                    continue;
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
            }
            //----------------Слизень--------------------//
            if (map[y][x] == 21) {
                countSlime++;
                Slime *slime = malloc(sizeof(Slime));
                if (slime == NULL) {
                    countSlime--;
                    continue;
                }

                Slime *temp = realloc(arraySlime, sizeof(Slime) * countSlime);
                if (temp == NULL) {
                    free(slime);
                    countSlime--;
                    continue;
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
            }
            //----------------Гоблин с бомбой--------------------//
            if (map[y][x] == 22) {
                countBomber++;
                Bomber *bomber = malloc(sizeof(Bomber));
                if (bomber == NULL) {
                    countBomber--;
                    continue;
                }

                Bomber *temp = realloc(arrayBomber, sizeof(Bomber) * countBomber);
                if (temp == NULL) {
                    free(bomber);
                    countBomber--;
                    continue;
                } else {
                    arrayBomber = temp;
                }
                
                initializeBomber(x * backGroundSize, y * backGroundSize, bomber);
                if (countBomber != 0) {
                    arrayBomber[countBomber - 1] = *bomber;
                } else {
                    arrayBomber[0] = *bomber;
                }
                map[y][x] = 0;
            }
            //------------------Ваза----------------------------//
            if (map[y][x] == 25) {
                countVase++;
                Vase *vase = malloc(sizeof(Vase));
                if (vase == NULL) {
                    countVase--;
                    continue;
                }

                Vase *temp = realloc(arrayVase, sizeof(Vase) * countVase);
                if (temp == NULL) {
                    free(vase);
                    countVase--;
                    continue;
                } else {
                    arrayVase = temp;
                }

                initializeVase(x * backGroundSize, y * backGroundSize, vase);
                if (countVase != 0) {
                    arrayVase[countVase - 1] = *vase;
                } else {
                    arrayVase[0] = *vase;
                }
                map[y][x] = 0;
            }
            //-----------------Камни------------------//
            if (map[y][x] == 26) {
                countStone++;
                Stone *stone = malloc(sizeof(Stone));
                if (stone == NULL) {
                    countStone--;
                    continue;
                }

                Stone *temp = realloc(arrayStone, sizeof(Stone) * countStone);
                if (temp == NULL) {
                    free(stone);
                    countStone--;
                    continue;
                } else {
                    arrayStone = temp;
                }

                initializeStone(x * backGroundSize, y * backGroundSize, stone);
                if (countStone != 0) {
                    arrayStone[countStone - 1] = *stone;
                } else {
                    arrayStone[0] = *stone;
                }
                map[y][x] = 0;
            }
            //-------------------Дверь-------------------//
            if (map[y][x] == 27) {
                door->position = (Vector2){x * backGroundSize, y * backGroundSize - 8};
            }
        }
    }
}

void unloadTextureAndMemory(int **map, int yMax, Door *door) {
    UnloadTexture(textureHealthHud);
    UnloadTexture(textureMenu);
    UnloadTexture(door->texture);
    unloadTextureMap();
    unloadPlayer();
    unloadMoney();
    unloadSlime();
    unloadBomber();
    unloadStone();

    CloseAudioDevice();
    
    for (int i = 0; i < yMax; i++) {
        free(map[i]);
    }
    free(map);
    free(arrayMoney);
    free(arraySlime);
    free(arrayBomber);
    free(arrayVase);
    free(arrayStone);
    CloseWindow();
}

void removeInactiveMoney() {
    int newCountMoney = 0;

    Money *newArrayMoney = malloc(sizeof(Money) * countMoney);
    if (newArrayMoney == NULL) {
        return;
    }

    for (int i = 0; i < countMoney; i++) {
        if (arrayMoney[i].isAlive == 1) {
            newArrayMoney[newCountMoney] = arrayMoney[i];
            newCountMoney++;
        }
    }

    free(arrayMoney);

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

void removeInactiveBomber() {
    int newCountBomber = 0;
    Bomber *newArrayBomber = malloc(sizeof(Bomber) * countBomber);
    if (newArrayBomber == NULL) {
        return;
    }

    for (int i = 0; i < countBomber; i++) {
        if (arrayBomber[i].isAlive == 1) {
            newArrayBomber[newCountBomber] = arrayBomber[i];
            newCountBomber++;
        }
    }

    free(arrayBomber);

    arrayBomber = newArrayBomber;
    countBomber = newCountBomber;
}