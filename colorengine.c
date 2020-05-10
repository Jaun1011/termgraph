#include "./colorengine.h"

Color COLOR_BLACK = {0, 0, 0};
Color COLOR_GREY_DARK = {78, 78, 78};
Color COLOR_GREY_LIGHT = {170, 170, 170};
Color COLOR_WHITE = {255, 255, 255};


void colorengine_writeColorCodesToBuffer(char *buffer, size_t buffer_size, Color **p_colors, size_t layers, size_t height, size_t width)
{

    int cx = 0;
    const int offset_z = SCREEN_WIDTH * SCREEN_HEIGHT;
    //cx += snprintf(buffer + cx, buffer_size - cx,  "\e[2J");

    for (size_t y = 0; y < height; y += 2)
    {
        for (size_t x = 0; x < width; x++)
        {
            const int offset_y = (y * width);

            Color **p_top = p_colors + x + offset_y;
            Color **p_bottom = p_top + width;

            debug_int(x);
            debug_int(y);

            /*
             * checks which pointer is in the front
             **/
            for (size_t z = 0; z < layers - 1; z++)
            {

                const int pos_z = offset_z;

                // printf("p_top\t\tz = %i\tp_top = %p\t\tres = %p\t",z, p_top, *(p_top + pos_z));
                // printf("ISNULL %i\n",*(p_top + pos_z) != NULL);
                if (*(p_top + pos_z) != NULL)
                {
                    p_top += pos_z;

                    /* printf("z = %i \t rgb = (%i, %i, %i)\n", z,           
                            (*p_top)->red, 
                            (*p_top)->green, 
                            (*p_top)->blue); */
                }

                // printf("p_bottom\tz = %i\t%p\t\tval = %p\t",z, p_bottom, *(p_bottom + pos_z));
                // printf("ISNULL %i\n\n",*(p_bottom + pos_z) != NULL);
                if (*(p_bottom + pos_z) != NULL)
                {
                    p_bottom += pos_z;
                }
            }

            cx += snprintf(buffer + cx, buffer_size - cx, "\x1b[38;2;%i;%i;%im\x1b[48;2;%i;%i;%im▀\x1b[0m",
                           (*p_top)->red,
                           (*p_top)->green,
                           (*p_top)->blue,
                           (*p_bottom)->red,
                           (*p_bottom)->green,
                           (*p_bottom)->blue);
        }
        cx += snprintf(buffer + cx, buffer_size - cx, "\n\x1b[0m");
    }
}

/** 
* this is the first step to be done. 
* the reason this is not in colorengine_ininitScreen() 
* is that it can be the case that the references of the colors are used earlier 
*/

void colorengine_initScreen(){
    
    Color **colors;

    gamelogic_initialize_map(colors);

    debug_pointer(*colors);
    char screenBuffer[LENGTH_SCREEN_BUFFER];

    colorengine_writeColorCodesToBuffer(screenBuffer, sizeof(screenBuffer), colors, SCREEN_LAYERS, SCREEN_HEIGHT, SCREEN_WIDTH);



    
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