#include "./colorengine.h"




void colorengine_writeColorCodesToBuffer(char *buffer, size_t buffer_size, Color **p_colors, size_t layers, size_t height, size_t width){

    int cx = 0;

    const int OFFSET_Z = width * height;
    const int OFFSET_Y = width;
    //cx += snprintf(buffer + cx, buffer_size - cx,  "\e[2J");


    // printColorArray(p_colors);
    

    for (size_t y = 0; y < height; y = y+2){


        for (size_t x = 0; x < width; x++){
            
            int pointer_increment_top = x + y * OFFSET_Y;
            int pointer_increment_bottom = x + (y+1) * OFFSET_Y;


            Color *p_top = *(p_colors + pointer_increment_top);
            Color *p_bottom = *(p_colors + pointer_increment_bottom);

            cx += snprintf(buffer + cx, buffer_size - cx, "\x1b[38;2;%i;%i;%im\x1b[48;2;%i;%i;%im▀\x1b[0m",
                           p_top->red,
                           p_top->green,
                           p_top->blue,
                           p_bottom->red,
                           p_bottom->green,
                           p_bottom->blue);
        }
        cx += snprintf(buffer + cx, buffer_size - cx, "\n\x1b[0m");

    
    }

            print_splitter();

}

/** 
* this is the first step to be done. 
* the reason this is not in colorengine_ininitScreen() 
* is that it can be the case that the references of the colors are used earlier 
*/

void colorengine_initScreen(){
    



    Array tl = gamelogic_allocateTileLayer(1);



    debug_int(tl.size);
    TileLayer *tilelayer = (TileLayer*) tl.value;
        
    Color **p_colors = gamelogic_transformTileSetsIntoColor(&tl);

    //gamelogic_initialize_map(&p_colors);


    char screenBuffer[LENGTH_SCREEN_BUFFER];


    colorengine_writeColorCodesToBuffer(screenBuffer, sizeof(screenBuffer), p_colors, SCREEN_LAYERS , SCREEN_HEIGHT * TILE_SIZE , SCREEN_WIDTH * TILE_SIZE);
    
    
    
   // gamelogic_transformTileSetsIntoColor(tileLayer, SCREEN_LAYERS, p_colors);



    
    printf(screenBuffer);
    //usleep(60000);
}

void printColor(Color *top, Color *bottom)
{
    printf(
        "\x1b[38;2;%s;%s;%sm\x1b[48;2;%s;%s;%sm▀\x1b[0m",
        top->red,
        top->green,
        top->blue,
        bottom->red,
        bottom->green,
        bottom->blue);
}