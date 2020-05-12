#include <stdlib.h>

#include "colorengine.h"

VectorR2 getSizeOfTileColors(Tile *tile)
{
    return (VectorR2){
        sizeof_array(tile->colors),
        sizeof_array(tile->colors[0])};
}

void copyColorsFromTileToColorBuffer(Tile *p_tile, VectorR2 size_tiles, Color **p_colors, VectorR3 pos)
{

    /*  
        printf("__________________________________\n");
        debug_pointer(p_colors);
    */
    VectorR2 size_colors = p_tile->size_tiles; 

    size_t preset_x = pos.x * size_tiles.x;
    size_t preset_y = pos.y * size_colors.x * size_tiles.x * size_colors.y;

    size_t offset_x = size_colors.x * size_tiles.x;

    for (size_t y = 0; y < size_colors.y; y++){
        for (size_t x = 0; x < size_colors.x; x++){
            
            size_t pointer_increment = preset_x + preset_y + x + offset_x*y;

            Color **p_color = (p_colors + pointer_increment);
            Color **p_tile_color = (p_tile->colors + x + size_colors.x * y);

            *p_color = *p_tile_color;
            // (p_colors + preset_x + x + preset_y + offset_x * y) = tile->colors[y][x];
        }
    }
}

Color **gamelogic_transformTileSetsIntoColor(Array *p_tileLayers)
{    

    size_t size_p_colors = SCREEN_HEIGHT *TILE_SIZE * SCREEN_WIDTH * TILE_SIZE * SCREEN_LAYERS * sizeof(Color *);
    Color **p_colors = malloc(size_p_colors);



    for (size_t z = 0; z < p_tileLayers->size; z++){
        TileLayer *p_tileLayer=  (p_tileLayers->value + z);
        Tile **p_tiles = p_tileLayer->tiles;
        
        size_t size_x = p_tileLayer->size_tiles.x;
        size_t size_y = p_tileLayer->size_tiles.y;
            

        Tile **p_tile = p_tileLayer->tiles;
        for (size_t y = 0; y < size_y; y++){
            for (size_t x = 0; x < size_x; x++){
                Tile *tile = *(p_tile + x + y * size_x);


                if (tile != NULL){
                    VectorR3 currentCursor = {x, y, z};
                    copyColorsFromTileToColorBuffer(tile, p_tileLayer->size_tiles, p_colors, currentCursor);
                }
            }
        }
    }
    return p_colors; 
}


void printColorArray(Color **p_colors){
    for (size_t i = 0; i < SCREEN_HEIGHT * TILE_SIZE * SCREEN_WIDTH * TILE_SIZE * SCREEN_LAYERS; i++){
        debug_int(i);
        debug_pointer((*(p_colors + i)));
        debug_int((*(p_colors + i))->blue);
    }
    
}



Tile **createTile(Color **color, size_t size_color){
    Color **p_colors = malloc(sizeof(color));
    memcpy(p_colors, &color, sizeof(color));
    Tile *p_tile_grey = malloc(sizeof(Tile));

    *p_tile_grey = (Tile) {
        p_colors,
        (VectorR2) {TILE_SIZE, TILE_SIZE}
    };

}


TileLayer *gamelogic_allocateTilelayer(){
    Color *black =      malloc(sizeof(Color));
    Color *grey_dark =  malloc(sizeof(Color));
    Color *grey_light = malloc(sizeof(Color));
    Color *white =      malloc(sizeof(Color));

    *black = (Color) {0, 0, 0};
    *grey_dark = (Color) {78, 78, 78};
    *grey_light = (Color) {170, 170, 170};
    *white = (Color) {255, 255, 255};

    Color *grey_array[TILE_SIZE * TILE_SIZE] = {
        white, grey_light, grey_light, white,
        grey_light, white, white, grey_light,
        grey_light, white, white, grey_light,
        white, grey_light, grey_light, white,
    };

    Color *white_array[TILE_SIZE * TILE_SIZE] = {
            grey_light, white, white, white,
            grey_light, white, white, white,
            grey_light, grey_light, white, white,
            grey_light, grey_light, grey_light, grey_light,
    };

    Color **p_color_grey = malloc(sizeof(grey_array));
    Color **p_color_white = malloc(sizeof(white_array));

    memcpy(p_color_grey, &grey_array, sizeof(grey_array));
    memcpy(p_color_white, &white_array, sizeof(white_array));
    
    Tile tile_white = {
        p_color_white,
        (VectorR2) {TILE_SIZE, TILE_SIZE}
    };

    Tile tile_grey = {
        p_color_grey,
        (VectorR2) {TILE_SIZE, TILE_SIZE}
    };
    
    Tile *p_tile_grey = malloc(sizeof(Tile));
    memcpy(p_tile_grey, &tile_grey, sizeof(tile_grey));

    Tile *p_tile_white = malloc(sizeof(Tile));
    memcpy(p_tile_white, &tile_white, sizeof(tile_white));


    Tile *tile_layer_array[SCREEN_HEIGHT * SCREEN_WIDTH] = {
        p_tile_grey, p_tile_grey, p_tile_grey, p_tile_grey,
        p_tile_grey, p_tile_white, p_tile_white, p_tile_grey,
        p_tile_grey, p_tile_white, p_tile_white, p_tile_grey,
        p_tile_grey, p_tile_grey, p_tile_grey, p_tile_grey
    };

    Tile **p_tile_layer_array = malloc(sizeof(tile_layer_array));
    memcpy(p_tile_layer_array, &tile_layer_array, sizeof(tile_layer_array));

    TileLayer tilelayer = {
        p_tile_layer_array,
        {
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
        }
    };

    TileLayer *p_tilelayer = malloc(sizeof(tilelayer));
        memcpy(p_tilelayer, &tilelayer, sizeof(tilelayer));

    return p_tilelayer;
}


Array gamelogic_allocateTileLayer(size_t size_layers)
{

    Array tileLayers;

    tileLayers.size = size_layers;
    tileLayers.value = gamelogic_allocateTilelayer();

    // TileLayer *tilelayer = gamelogic_allocateTilelayer();


/*
    debug_pointer(tilelayer);
    debug_pointer((*(*(tilelayer->tiles))->colors)->blue);
*/

    return tileLayers;
}
