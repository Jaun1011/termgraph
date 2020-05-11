#include <stdlib.h>

#include "colorengine.h"

VectorR2 getSizeOfTileColors(Tile *tile)
{
    return (VectorR2){
        sizeof_array(tile->colors),
        sizeof_array(tile->colors[0])};
}

void copyColorsFromTileToColorBuffer(Tile *tile, VectorR2 size_tile, Color *p_colors, VectorR3 pos)
{
    VectorR2 size_colors = getSizeOfTileColors(tile);
    /*  
        printf("__________________________________\n");
        debug_pointer(p_colors);
    */

    size_t preset_x = pos.x * size_colors.x;
    size_t offset_x = size_colors.x * size_tile.x;
    size_t preset_y = pos.y * size_colors.x * size_tile.x * size_colors.y;

    for (size_t y = 0; y < size_colors.y; y++)
    {

        for (size_t x = 0; x < size_colors.x; x++)
        {

            //(p_colors + preset_x + x + preset_y + offset_x * y) = tile->colors[y][x];
        }
    }
}

Color **gamelogic_transformTileSetsIntoColor(Array *p_tileLayers)
{
/*
    TileLayer *p_tileLayer = p_tileLayers->value;

    const int widht_x = sizeof_array(p_tileLayer->tiles[0]);
    const int widht_y = sizeof_array(p_tileLayer->tiles);

    Color **p_colors = malloc(SCREEN_HEIGHT * SCREEN_WIDTH * SCREEN_LAYERS * sizeof(Color));

    debug_pointer(p_tileLayers->size);

    for (size_t z = 0; z < p_tileLayers->size; z++)
    {

        Tile *tiles = (p_tileLayer + z)->tiles;

        if (tiles != NULL)
        {
            for (size_t y = 0; y < widht_y; y++)
            {
                for (size_t x = 0; x < widht_x; x++)
                {
                    Tile *tile = ((tiles + x) + y * widht_x);

                    if (tile != NULL)
                    {
                        copyColorsFromTileToColorBuffer(tile, (VectorR2){widht_x, widht_y}, p_colors, (VectorR3){x, y, z});
                    }
                }
            }
        }
    }
 
    return p_colors;   */
}



Tile **gamelogic_allocateTilelayer(){
    Color *black =      malloc(sizeof(Color));
    Color *grey_dark =  malloc(sizeof(Color));
    Color *grey_light = malloc(sizeof(Color));
    Color *white =      malloc(sizeof(Color));

    *black = (Color) {0, 0, 0};
    *grey_dark = (Color) {78, 78, 78};
    *grey_light = (Color) {170, 170, 170};
    *white = (Color) {255, 255, 255};

    debug_int(grey_dark->blue);

    Color *grey_array[TILE_SIZE * TILE_SIZE] = {
            grey_dark, grey_dark, grey_dark, grey_dark,
            grey_dark, grey_dark, grey_dark, grey_dark,
            grey_dark, grey_dark, grey_dark, grey_dark,
            grey_dark, grey_dark, grey_dark, grey_dark,
    };

    Color *white_array[TILE_SIZE * TILE_SIZE] = {
            white, white, white, white,
            white, white, white, white,
            white, white, white, white,
            white, white, white, white,
    };

    Color **p_grey_array = malloc(sizeof(grey_array));
    Color **p_white_array = malloc(sizeof(white_array));
    
    memcpy(p_grey_array, &white_array, sizeof(white_array));
    memcpy(p_white_array, &grey_array, sizeof(grey_array));
    
    print_splitter();
    debug_pointer((*(white_array))->blue);
    debug_int((*(white_array))->blue);

    print_splitter();
    debug_pointer((*(white_array))->blue);
    debug_int((*(p_white_array))->blue);

    print_splitter();

    Tile tile_white = {
        p_white_array,
        (VectorR2) {TILE_SIZE, TILE_SIZE}
    };




    Tile tile_grey = {
        p_grey_array,
        (VectorR2) {TILE_SIZE, TILE_SIZE}
    };
    
    Tile *p_tile_grey = malloc(sizeof(Tile));
    memcpy(p_tile_grey, &tile_grey, sizeof(tile_grey));




    debug_pointer(p_tile_grey);
    debug_pointer((*(p_tile_grey->colors)));
    debug_pointer((*(p_tile_grey->colors))->blue);
    debug_int((*(p_tile_grey->colors))->blue);
    print_splitter();




    Tile *tile_layer_array[SCREEN_HEIGHT * SCREEN_WIDTH] = {
        p_tile_grey, p_white_array,p_white_array,p_white_array,
        p_tile_grey, p_white_array,p_white_array,p_white_array,
        p_white_array, p_white_array,p_white_array,p_white_array
    };

    Tile **p_tile_layer_array = malloc(sizeof(tile_layer_array));
    debug_pointer(p_tile_layer_array);

    memcpy(p_tile_layer_array, &tile_layer_array, sizeof(tile_layer_array));

    debug_pointer(p_tile_layer_array);



    debug_pointer((*(tile_layer_array[0])));
    debug_pointer((*(tile_layer_array[0]->colors))->blue);


    debug_pointer(((p_tile_layer_array)));
    debug_pointer((*(p_tile_layer_array)));
    debug_pointer((*( (*(p_tile_layer_array))  ->colors))->blue);
    print_splitter();

    return p_tile_layer_array;

/*



    Tile *tile_layer_array[SCREEN_HEIGHT * SCREEN_WIDTH] = {
        &p_white, &p_white, &p_white, &p_white,
        &p_grey, &p_white, &p_grey, &p_grey,
        &p_grey, &p_grey, &p_grey, &p_grey
    };


    TileLayer tilelayer = {
        &p_tile_layer_array,
        {
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
        }
    };
    TileLayer *p_tilelayer = malloc(sizeof(tilelayer));
    
    memcpy(p_tilelayer, &tilelayer, sizeof(tilelayer));

    return p_tilelayer;

    */
}


Array gamelogic_allocateTileLayer(size_t size_layers)
{

    Array tileLayers;

    tileLayers.size = size_layers;
    //tileLayers.value = gamelogic_allocateTilelayer();

    Tile **tile = gamelogic_allocateTilelayer();




    debug_pointer(tile);
    debug_pointer((*( (*(tile))  ->colors))->blue);







    for (size_t y = 0; y < 3; y++){
        for (size_t x = 0; x < 4; x++){
            //debug_pointer((tileLayers.value)[y][x]);
        }
    }
        

    return tileLayers;
}



void gamelogic_initialize_map(Color **p_colors)
{
}

