#include <stdlib.h>
#include <raylib.h>
#include <stdio.h>
#include "map.h"

Texture2D textureGrass;
Texture2D textureGround;
Texture2D midground;
Texture2D foreground;
Texture2D textureArrowsRight;
Texture2D textureSpikesTrap;

float midScrollSpeed = 0.1f;   // Скорость среднего плана
float foreScrollSpeed = 0.2f;  // Скорость переднего плана

float scrollingMid = 0.0f;
float scrollingFore = 0.0f;

Rectangle arrayTexture[21];

void initTexture(int backGroundSize) {
    textureGrass = LoadTexture("resource/miscellaneous sprites/grass_props.png");
    if (textureGrass.id == 0) {
        exit(1);
    }
    textureGround = LoadTexture("resource/tiles and background_foreground (new)/tileset.png");
    if (textureGround.id == 0) {
        exit(1);
    }
    midground = LoadTexture("resource/tiles and background_foreground (new)/bg_0.png");
    if (midground.id == 0) {
        exit(1);
    }
    foreground = LoadTexture("resource/tiles and background_foreground (new)/bg_1.png");
    if (foreground.id == 0) {
        exit(1);
    }
    textureArrowsRight = LoadTexture("resource/miscellaneous sprites/arrow_plate_right.png");
    if (textureArrowsRight.id == 0) {
        exit(1);
    }
    textureSpikesTrap = LoadTexture("resource/miscellaneous sprites/spikes.png");
    if (textureSpikesTrap.id == 0) {
        exit(1);
    }
    textureSpikesTrap.height = -textureSpikesTrap.height;

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

int **initializeMap(int xMax, int yMax) {
    int **map = calloc(yMax, sizeof(int*));
    if (map == NULL) {
        exit(1);
    }

    for (int y = 0; y < yMax; y++) {
        map[y] = calloc(xMax, sizeof(int));
        for (int x = 0; x < xMax; x++) {
            map[y][x] = 0;
        }
    }
    return map;
}

void drawMap(int **map, int xMax, int yMax, int backGroundSize) {
    for (int y = 0; y < yMax; y++) {
        for (int x = 0; x < xMax; x++) {
            Vector2 position = { x * backGroundSize, y * backGroundSize };
            int tileType = map[y][x];

            if (tileType > 0 && tileType <= 21) {
                DrawTextureRec(textureGround, arrayTexture[tileType - 1], position, WHITE);
            }

            if (tileType == 23) {
                position.y += 8;
                DrawTextureRec(textureGrass, (Rectangle){0, 0, 16, 8}, position, WHITE);
            }

            if (tileType == 24) {
                DrawTextureRec(textureArrowsRight, (Rectangle){0, 0, 16, 16}, position, WHITE);
            }

            if (tileType == 28) {
                DrawTextureRec(textureSpikesTrap, (Rectangle){0, 0, 16, 16}, position, WHITE);
            }
        }
    }
}

void loadMap(const char *filename, int **map, int xMax, int yMax) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) return;

    for (int y = 0; y < yMax; y++) {
        for (int x = 0; x < xMax; x++) {
            fscanf(file, "%d", &map[y][x]);
        }
    }

    fclose(file);
}

void updateScrolling(float x) {
    scrollingMid = x * midScrollSpeed;
    scrollingFore = x * foreScrollSpeed;
}

void drawBackGround() {
        DrawTextureEx(midground, (Vector2){ scrollingMid, 0 }, 0.0f, 3.0f, WHITE);
        DrawTextureEx(midground, (Vector2){ midground.width * -2 + scrollingMid, 0 }, 0.0f, 3.0f, WHITE);

        DrawTextureEx(foreground, (Vector2){ scrollingFore, 0 }, 0.0f, 3.0f, WHITE);
        DrawTextureEx(foreground, (Vector2){ foreground.width * -2 + scrollingFore, 0 }, 0.0f, 3.0f, WHITE);
}

void unloadTextureMap() {
    if (midground.id != 0) {
        UnloadTexture(midground);
    }
    if (foreground.id != 0) {
        UnloadTexture(foreground);
    }
    if (textureGround.id != 0) {
        UnloadTexture(textureGround);
    }
    if (textureGrass.id != 0) {
        UnloadTexture(textureGrass);
    }
}