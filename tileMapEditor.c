#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"

const int windowWidth = 800;
const int windowHeight = 600;
const int backGroundSize = 16;

int yMax = 15;
int xMax = 25;

int **map = NULL;
int selectedTile = 0;

void initializeMap();
void freeMap();
void initTexture();
void drawMap();
void drawTextureSelector();
void saveMap(const char *filename);
void loadMap(const char *filename);

Texture2D textureGround;
Texture2D texturePlayer;
Texture2D textureMoney;
Texture2D textureSlime;
Rectangle arrayGround[21];

int main(int argc, char **argv) {    
    InitWindow(windowWidth, windowHeight, "TileMapEditor");
    initializeMap();
    initTexture();

    SetTargetFPS(60);

    Vector2 tarPos = { 0 };
    tarPos.x = 0;
    tarPos.y = 0;    
    
    Camera2D camera = { 0 };
    camera.target = (Vector2){ tarPos.x + 20.0f, tarPos.y + 20.0f };
    camera.offset = (Vector2){ windowWidth / 2.0f, windowHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 2.0f;

    loadMap(argv[1]);
    
    while(!WindowShouldClose()) {        
        if (IsKeyPressed(KEY_S)) {
            saveMap(argv[1]);
        }

        /*
        if (IsKeyPressed(KEY_S)) {
            saveMap("save_map.txt");
        }

        if (IsKeyPressed(KEY_L)) {
            loadMap("save_map.txt");
        }
        */

        if (IsKeyDown(KEY_LEFT)) {
            tarPos.x -= 5;
        }
        if (IsKeyDown(KEY_RIGHT)) {
            tarPos.x += 5;
        }
        if (IsKeyDown(KEY_UP)) {
            tarPos.y -= 5;
        }
        if (IsKeyDown(KEY_DOWN)) {
            tarPos.y += 5;
        }
        
        camera.target = (Vector2) { tarPos.x + 20.0f, tarPos.y + 20.0f };

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePosition = GetMousePosition();
            mousePosition = (Vector2) {
                (mousePosition.x - (windowWidth / 2.0f)) / camera.zoom + camera.target.x,
                (mousePosition.y - (windowHeight / 2.0f)) / camera.zoom + camera.target.y
            };


            int mapX = (int)(mousePosition.x / backGroundSize);
            int mapY = (int)(mousePosition.y / backGroundSize);
            if (mapX >= 0 && mapX < xMax && mapY >= 0 && mapY < yMax) {
                map[mapY][mapX] = selectedTile + 1;
            }
        }

        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            Vector2 mousePosition = GetMousePosition();
            mousePosition = (Vector2) {
                (mousePosition.x - (windowWidth / 2.0f)) / camera.zoom + camera.target.x,
                (mousePosition.y - (windowHeight / 2.0f)) / camera.zoom + camera.target.y
            };


            int mapX = (int)(mousePosition.x / backGroundSize);
            int mapY = (int)(mousePosition.y / backGroundSize);

            if (mapX >= 0 && mapX < xMax && mapY >= 0 && mapY < yMax) {
                map[mapY][mapX] = 0;
            }
        }
        
        BeginDrawing();
            ClearBackground(RAYWHITE);
                BeginMode2D(camera);
                    drawMap();
                EndMode2D();
            drawTextureSelector();
            DrawFPS(1, 1);
        EndDrawing();
    }

    freeMap();
    UnloadTexture(textureGround);
    CloseWindow();
    return 0;
}

void initializeMap() {
    map = calloc(yMax, sizeof(int*));
    if (map == NULL) {
        exit(1);
    }

    for (int y = 0; y < yMax; y++) {
        map[y] = calloc(xMax, sizeof(int));
        for (int x = 0; x < xMax; x++) {
            map[y][x] = 0;
        }
    }
}

void freeMap() {
    for (int y = 0; y < yMax; y++) {
        free(map[y]);
    }
    free(map);
}

void initTexture() {
    textureGround = LoadTexture("resource/tiles and background_foreground (new)/tileset.png");
    texturePlayer = LoadTexture("resource/hud elements/lifes_icon.png");
    if (texturePlayer.id == 0) {
        printf("Ошибка загрузки текстуры игрока!\n");
    }

    textureMoney = LoadTexture("resource/hud elements/coins_hud.png");
    if (textureMoney.id == 0) {
        printf("Ошибка загрузки текстуры монетки!\n");
    }

    textureSlime = LoadTexture("resource/enemies sprites/slime/slime_idle_anim_strip_5.png");
    if (textureMoney.id == 0) {
        printf("Ошибка загрузки текстуры слизня!\n");
    }

    //printf("Width: %d, Height: %d\n", texturePlayer.width, texturePlayer.height);

    // загрузка текстур карты
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 6; x++) {
            int index = y * 6 + x;
            arrayGround[index] = (Rectangle) {
                x * backGroundSize, y * backGroundSize,
                backGroundSize, backGroundSize
            };
        }
    }

    // игрок и монетка
    for (int i = 19; i <= 20; i++) {
        arrayGround[i] = (Rectangle) {
            0, 0, 16, 16
        };
    }
}

void drawMap() {
    for (int y = 0; y < yMax; y++) {
        for (int x = 0; x < xMax; x++) {
            Vector2 position = { x * backGroundSize, y * backGroundSize };
            int tileType = map[y][x];

            if (tileType == 19) {
                Rectangle sourceRect = { 0, 0, texturePlayer.width, texturePlayer.height };
                Rectangle destRect = { position.x, position.y, backGroundSize, backGroundSize };
                DrawTexturePro(texturePlayer, sourceRect, destRect, (Vector2){0, 0}, 0.0f, WHITE);
            }
            else if (tileType == 20) {
                Rectangle sourceRect = { 0, 0, textureMoney.width, textureMoney.height };
                Rectangle destRect = { position.x, position.y, backGroundSize, backGroundSize };
                DrawTexturePro(textureMoney, sourceRect, destRect, (Vector2){0, 0}, 0.0f, WHITE);
            }
            else if (tileType == 21) {
                Rectangle sourceRect = { 0, 0, textureSlime.width / 5, textureSlime.height };
                Rectangle destRect = { position.x, position.y, backGroundSize, backGroundSize };
                DrawTexturePro(textureSlime, sourceRect, destRect, (Vector2){0, 0}, 0.0f, WHITE);
            }
            else if (tileType > 0 && tileType <= sizeof(arrayGround) / sizeof(arrayGround[0])) {
                DrawTextureRec(textureGround, arrayGround[tileType - 1], position, WHITE);
            }
            else {
                DrawRectangle(position.x, position.y, backGroundSize, backGroundSize, RED);
            }
        }
    }
}

void drawTextureSelector() {
    int selectorX = 0;
    int selectorY = 0;
    float scale = 2.0f;

    for (int i = 0; i < sizeof(arrayGround)/sizeof(arrayGround[0]); i++) {
        Vector2 position = (Vector2){ 
            selectorX + (i % 6) * backGroundSize * scale, 
            selectorY + (i / 6) * backGroundSize * scale 
        };
        
        // Изменяем размеры текстуры, чтобы она заполняла весь квадрат
        Rectangle sourceRect = arrayGround[i];
        Rectangle destRect = { position.x, position.y, backGroundSize * scale, backGroundSize * scale };

        if (i == 18) {
            Rectangle sourceRect = { 0, 0, texturePlayer.width, texturePlayer.height };
            Rectangle destRect = { position.x, position.y, backGroundSize * scale, backGroundSize * scale };
            DrawTexturePro(texturePlayer, sourceRect, destRect, (Vector2){0, 0}, 0.0f, WHITE);
        }
        else if (i == 19) {
            Rectangle sourceRect = { 0, 0, textureMoney.width, textureMoney.height };
            Rectangle destRect = { position.x, position.y, backGroundSize * scale, backGroundSize * scale };
            DrawTexturePro(textureMoney, sourceRect, destRect, (Vector2){0, 0}, 0.0f, WHITE);
        }
        else if (i == 20) {
            Rectangle sourceRect = { 0, 0, textureSlime.width / 5, textureSlime.height };
            Rectangle destRect = { position.x, position.y, backGroundSize * scale, backGroundSize * scale };
            DrawTexturePro(textureSlime, sourceRect, destRect, (Vector2){0, 0}, 0.0f, WHITE);
        }

        else {
            DrawTexturePro(textureGround, sourceRect, destRect, (Vector2){0, 0}, 0.0f, WHITE);
        }

        Rectangle collisionRect = { position.x, position.y, backGroundSize * scale, backGroundSize * scale };

        if (CheckCollisionPointRec(GetMousePosition(), collisionRect)) {
            DrawRectangleLines(position.x, position.y, backGroundSize * scale, backGroundSize * scale, RED);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                selectedTile = i;
            }
        }
    }
}

void saveMap(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) return;

    for (int y = 0; y < yMax; y++) {
        for (int x = 0; x < xMax; x++) {
            fprintf(file, "%d ", map[y][x]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

void loadMap(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) return;

    for (int y = 0; y < yMax; y++) {
        for (int x = 0; x < xMax; x++) {
            fscanf(file, "%d", &map[y][x]);
        }
    }

    fclose(file);
}