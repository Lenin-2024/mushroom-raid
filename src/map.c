#include <stdlib.h>
#include <raylib.h>
#include <stdio.h>
#include "map.h"

float midScrollSpeed = 0.1f;   // Скорость среднего плана
float foreScrollSpeed = 0.2f;  // Скорость переднего плана

float scrollingMid = 0.0f;
float scrollingFore = 0.0f;

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

void drawMap(int **map, int xMax, int yMax, int backGroundSize, int size, Texture textureGround, Rectangle arrayGround[]) {
    for (int y = 0; y < yMax; y++) {
        for (int x = 0; x < xMax; x++) {
            Vector2 position = { x * backGroundSize, y * backGroundSize };
            int tileType = map[y][x];

            if (tileType > 0 && tileType <= size /*sizeof(arrayGround)/sizeof(arrayGround[0])*/) {
                DrawTextureRec(textureGround, arrayGround[tileType - 1], position, WHITE);
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

void drawBackGround(Texture2D midground, Texture2D foreground) {
        DrawTextureEx(midground, (Vector2){ scrollingMid, 0 }, 0.0f, 3.0f, WHITE);
        DrawTextureEx(midground, (Vector2){ midground.width * -2 + scrollingMid, 0 }, 0.0f, 3.0f, WHITE);

        DrawTextureEx(foreground, (Vector2){ scrollingFore, 0 }, 0.0f, 3.0f, WHITE);
        DrawTextureEx(foreground, (Vector2){ foreground.width * -2 + scrollingFore, 0 }, 0.0f, 3.0f, WHITE);
}