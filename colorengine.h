#include <stdio.h>
#include <string.h>
#include <time.h> 
#include "array.h"
#include "debug.h"
#include "./dimension.h"

#define SCREEN_HEIGHT 3
#define SCREEN_WIDTH 4
#define SCREEN_LAYERS 1

#define LENGTH_COLOR_CODE 44
#define LENGTH_SCREEN_BUFFER SCREEN_HEIGHT * SCREEN_WIDTH * LENGTH_COLOR_CODE

#define TILE_SIZE 4



typedef struct Color_ {
    int red;
    int green;
    int blue;
} Color;

typedef struct Tile_ {
    Color **colors;
    VectorR2 size_tiles;
} Tile;

typedef struct TileLayer_ {
    Tile **tiles; // 2D array
    VectorR2 size_tiles;
} TileLayer;

void colorengine_initColors();
void colorengine_initScreen();

Array gamelogic_allocateTileLayer(size_t siize_layers);
Color **gamelogic_transformTileSetsIntoColor(Array *p_tileset);
void gamelogic_initialize_map();
