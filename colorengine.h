#include <stdio.h>
#include <string.h>
#include <time.h> 
#include "./debug.h"
#include "./dimension.h"

#define SCREEN_HEIGHT 16
#define SCREEN_WIDTH 16
#define SCREEN_LAYERS 4

#define LENGTH_COLOR_CODE 44
#define LENGTH_SCREEN_BUFFER SCREEN_HEIGHT * SCREEN_WIDTH * LENGTH_COLOR_CODE

#define TILE_SIZE 4


// colorengine
typedef struct Color {
    int red;
    int green;
    int blue;
} Color;



extern Color COLOR_BLACK;
extern Color COLOR_GREY_DARK;
extern Color COLOR_GREY_LIGHT;
extern Color COLOR_WHITE;

void colorengine_initColors();
void colorengine_initScreen();




// gameloginc

typedef struct Tile {
    Color *colors[TILE_SIZE][TILE_SIZE];
    size_t size_colors;
    
    // here are properties about the object
    // as colission enemy etc    
} Tile;

typedef struct TileSet {
    Tile *tiles; // 3D array
    VectorR2 size_tiles;
} TileSet;


void gamelogic_initialize_map();
