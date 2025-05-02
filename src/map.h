#ifndef MAP_H
#define MAP_H

extern float scrollingMid;
extern float scrollingFore;

int **initializeMap(int xMax, int yMax);
void loadMap(const char *filename, int **map, int xMax, int yMax);
void drawMap(int **map, int xMax, int yMax, int backGroundSize);
void drawBackGround();
void updateScrolling(float x);
void initTexture(int backGroundSize);
void unloadTextureMap();

#endif