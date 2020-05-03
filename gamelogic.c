#include "colorengine.h"

#define TILE_ELEMENTS SCREEN_HEIGHT * SCREEN_WIDTH / TILE_SIZE * TILE_SIZE 
#define sizeof_array(a) sizeof(a) / sizeof(a[0])



TileSet * gamelogic_initialize_map(){

    Color *night[TILE_SIZE][TILE_SIZE] = {
        {&COLOR_BLACK, &COLOR_BLACK, &COLOR_BLACK, &COLOR_BLACK},
        {&COLOR_BLACK, &COLOR_BLACK, &COLOR_BLACK, &COLOR_BLACK},
        {&COLOR_BLACK, &COLOR_BLACK, &COLOR_BLACK, &COLOR_BLACK},
        {&COLOR_BLACK, &COLOR_BLACK, &COLOR_BLACK, &COLOR_BLACK},
    };

    Color *test[TILE_SIZE][TILE_SIZE] = {
        {&COLOR_WHITE,      &COLOR_GREY_DARK,   &COLOR_BLACK,       &COLOR_GREY_DARK},
        {&COLOR_GREY_LIGHT, &COLOR_WHITE,       &COLOR_GREY_LIGHT,  &COLOR_WHITE},
        {&COLOR_BLACK,      &COLOR_GREY_DARK,   &COLOR_BLACK,       &COLOR_GREY_DARK},
        {&COLOR_GREY_LIGHT, &COLOR_WHITE,       &COLOR_GREY_LIGHT,  &COLOR_WHITE},
    };

    Color *day[TILE_SIZE][TILE_SIZE] = {
        {&COLOR_WHITE, &COLOR_WHITE, &COLOR_WHITE, &COLOR_WHITE},
        {&COLOR_WHITE, &COLOR_WHITE, &COLOR_WHITE, &COLOR_WHITE},
        {&COLOR_WHITE, &COLOR_WHITE, &COLOR_WHITE, &COLOR_WHITE},
        {&COLOR_WHITE, &COLOR_WHITE, &COLOR_WHITE, &COLOR_WHITE},
    };
    

    Tile t1 = { {0,0,1}, night[0][0] };
    Tile t2 = { {0,0,2}, day[0][0] };
    Tile t3 = { {0,0,3}, test[0][0] };


    Tile *tiles[TILE_ELEMENTS] = {
        &t1,
        &t2,
        &t2,
        &t3    
    };



}