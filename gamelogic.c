#include <stdlib.h>

#include "colorengine.h"
#include "array.h"

VectorR2 getSizeOfTileColors(Tile *tile)
{
    return (VectorR2){
        sizeof_array(tile->colors),
        sizeof_array(tile->colors[0])};
}

void copyColorsFromTileToColorBuffer(Tile *tile, VectorR2 size_tile, Color *p_color, VectorR3 pos)
{
    VectorR2 size_colors = getSizeOfTileColors(tile);
    


    int preset_x = pos.x * size_colors.x;

    int offset_x = size_colors.x * size_tile.x;
    int preset_y = pos.y * size_colors.x * size_tile.x * size_colors.y;


    for (size_t y = 0; y < size_colors.y; y++){
        for (size_t x = 0; x < size_colors.x; x++){
            
            // printf("\n");

            // debug_int(preset_x + x);
            /*
            printf("x = %i y = %i, t = %i\n",
                preset_x + x,
                preset_y + y,
                preset_x + x + preset_y + offset_x * y
            );
            */
           // this is some monstrosity of a pointer formula
            *(p_color + preset_x + x + preset_y + offset_x * y) = *tile->colors[y][x];

            //*(p_colors + x + ix + widht_x*y + iy  + size_tileset*z) = tile->colors[iy][ix];
        }
    }
}

void gamelogic_transformTileSetsIntoColor(TileSet *p_tileset, size_t size_tileset, Color **p_colors)
{
    int counter_buffer_position = 0;

    const int startpoint_x = 0;
    const int startpoint_y = 0;

    const int widht_x = p_tileset->size_tiles.x;
    const int widht_y = p_tileset->size_tiles.y;

    p_colors = malloc(SCREEN_HEIGHT * SCREEN_WIDTH * SCREEN_LAYERS * sizeof(Color));

    for (size_t z = 0; z < size_tileset; z++)
    {
        Tile *tiles = (p_tileset + z)->tiles;

        if (tiles != NULL)
        {
            for (size_t y = 0; y < widht_y; y++)
            {
                for (size_t x = 0; x < widht_x; x++)
                {

                    Tile *tile = ((tiles + x) + y * widht_x);

                    if (tile != NULL)
                    {
                        Color *color = tile->colors[0][0];
                        copyColorsFromTileToColorBuffer(tile, (VectorR2){widht_x, widht_y}, color, (VectorR3){x, y, z});
                    }
                }
            }
        }
    }
}

void gamelogic_initialize_map(Color **p_colors)
{
    Tile tile_grey = {
        {
            {&COLOR_GREY_DARK, &COLOR_GREY_DARK, &COLOR_GREY_DARK, &COLOR_GREY_DARK},
            {&COLOR_GREY_DARK, &COLOR_GREY_DARK, &COLOR_GREY_DARK, &COLOR_GREY_DARK},
            {&COLOR_GREY_DARK, &COLOR_GREY_DARK, &COLOR_GREY_DARK, &COLOR_GREY_DARK},
            {&COLOR_GREY_DARK, &COLOR_GREY_DARK, &COLOR_GREY_DARK, &COLOR_GREY_DARK},
        },
        TILE_SIZE * TILE_SIZE};

    Tile tile = {
        {
            {&COLOR_WHITE, &COLOR_WHITE, &COLOR_GREY_DARK, &COLOR_WHITE},
            {&COLOR_WHITE, &COLOR_WHITE, &COLOR_WHITE, &COLOR_WHITE},
            {&COLOR_WHITE, &COLOR_WHITE, &COLOR_WHITE, &COLOR_WHITE},
            {&COLOR_WHITE, &COLOR_WHITE, &COLOR_WHITE, &COLOR_WHITE},
        },
        TILE_SIZE * TILE_SIZE};

    Tile tiles[3][4] = {
        {tile, tile, tile, tile},
        {tile, tile, tile, tile},
        {tile, tile, tile_grey, tile},
    };

    TileSet tileSet[SCREEN_LAYERS] = {
        {(Tile *)tiles, {sizeof_array(tiles[0]), sizeof_array(tiles)}}};

    gamelogic_transformTileSetsIntoColor(tileSet, sizeof_array(tileSet), p_colors);

    /* 
    printf("\n------------------COLOR POINTER-----------\n");

    printf("&COLOR_WHITE\t%p\n", &COLOR_WHITE);
    printf("&COLOR_GREY_DARK\t%p\n", &COLOR_GREY_DARK);

    printf("\n-----------------tile---------------------\n");

    printf("tile\t\t%p\n", tile);

    printf("tile.colors\t%p\n", ((tile.colors)[0][0]));
    printf("tile.colors\t%i\n", ((tile.colors)[0][0]->blue));

    printf("\n-----------------tiles-------------------\n");

    printf("tiles   \t%p\n", tiles);
    printf("tiles[0][0]\t%p\n", tiles[0][0]);
    printf("*tiles[0][0]  \t%p\n", (tiles[0][0]));
    printf("tiles.colors  \t%p\n", ((tiles[0][0]).colors[0][0]));
    printf("tiles.colors  \t%p\n", tiles[0][0].colors[0][0]->blue);
    printf("tiles.colors  \t%i\n", tiles[0][0].colors[0][0]->blue);

    printf("\n---------------tileset-------------------\n");

    printf("tiles  \t%p\n", (tileSet[0].tiles));
    printf("tiles  \t%p\n", tileSet[0].tiles->colors[0][0]->blue);
    printf("tiles  \t%p\n", tileSet[0].tiles->colors[0][0]->blue);
    printf("tiles  \t%i\n", tileSet[0].tiles->colors[0][0]->blue);

    gamelogic_transformTileSetsIntoColor(tileSet, sizeof_array(tileSet));




    //printf("%i\n", (*color).red);

    Tile *p_tile = tileSet[0].tiles;
    
    Color ****color3 = (*p_tile).colors;


    printf("c3 %p\n", (*p_tile).colors);



    printf("c3 %i\n", (****color3).red);
    printf("c3 %p\n", (color3));
    printf("*c3 %p\n", (*color3));
    printf("**c3 %p\n", (**color3));
    printf("***c3 %p\n", (***color3));
    printf("****c3 %p\n", (****color3));


    printf("white %p\n", COLOR_WHITE);
    printf("%i\n", COLOR_WHITE.blue);
  


   printf("%s\n", t1.colors[0][0]->blue);

    debug_string("--------------");


    */
}
