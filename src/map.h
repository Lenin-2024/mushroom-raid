#ifndef MAP_H
#define MAP_H

extern float scrollingMid;
extern float scrollingFore;

int **initializeMap(int xMax, int yMax);
void loadMap(const char *filename, int **map, int xMax, int yMax);
void drawMap(int **map, int xMax, int yMax, int backGroundSize, int size, Texture textureGround, Rectangle arrayGround[]);
void drawBackGround(Texture2D midground, Texture2D foreground);
void updateScrolling(float x);

#endif