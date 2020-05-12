#include <stdio.h>
#include <string.h>
#include <time.h> 
#include "array.h"
#include "debug.h"
#include "./dimension.h"

#define SCREEN_HEIGHT 4
#define SCREEN_WIDTH 4
#define SCREEN_LAYERS 1
#define TILE_SIZE 4

#define LENGTH_COLOR_CODE 44
#define LENGTH_SCREEN_BUFFER SCREEN_HEIGHT * TILE_SIZE * TILE_SIZE* SCREEN_WIDTH * LENGTH_COLOR_CODE


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

void printColorArray(Color **color);

Array gamelogic_allocateTileLayer(size_t size_layers);
Color **gamelogic_transformTileSetsIntoColor(Array *p_tileset);
void gamelogic_initialize_map();
