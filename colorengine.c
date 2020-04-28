#include <stdio.h>
#include "./colorengine.h"
#include <string.h>
#include <time.h> 


typedef struct {
    int red;
    int green;
    int blue;
} Color;


Color rgb(int r,int g, int b){
    return (Color) {r, g, b};
}






void printColor(Color *top, Color *bottom){
    printf(
        "\x1b[38;2;%s;%s;%sm\x1b[48;2;%s;%s;%sm▀\x1b[0m", 
        top->red, 
        top->green, 
        top->blue,
        bottom->red, 
        bottom->green,
        bottom->blue
    );
}


void writeColorCodesToBuffer(char *buffer, size_t buffer_size, Color **p_colors, size_t height, size_t width){
    int cx = 0;

    cx += snprintf(buffer + cx, buffer_size - cx,  "\e[2J");


    for (size_t y = 0; y < height; y += 2){
        for (size_t x = 0; x < width; x++){
             
            Color *top = *(p_colors + x + (y * width));
            Color *bottom = *(p_colors + x  + width + (y * width));

            cx += snprintf(buffer + cx, buffer_size - cx,  "\x1b[38;2;%i;%i;%im\x1b[48;2;%i;%i;%im▀\x1b[0m", 
                top->red, 
                top->green, 
                top->blue,
                bottom->red, 
                bottom->green,
                bottom->blue
            );
        
        }
        cx += snprintf(buffer + cx, buffer_size - cx,  "\n\x1b[0m");
    }
}




void initScreen(){

    Color *colors[SCREEN_HEIGHT][SCREEN_WIDTH];
    const size_t COLOR_CODE_LEN = 44;

    char screenBuffer[SCREEN_HEIGHT * SCREEN_WIDTH * COLOR_CODE_LEN];

    Color red   = rgb(255, 0, 0);
    Color green = rgb(0, 255, 0);
    Color blue  = rgb(0, 0, 255);
    Color black = rgb(0, 0, 0);

        for(size_t y = 0; y < SCREEN_HEIGHT; y = y + 2){
            for(size_t x = 0; x < SCREEN_WIDTH; x++){
                colors[y][x] = &green;
                colors[y+1][x] = &red;
            }
        }
    }


    int i =20;
    while (1){




            writeColorCodesToBuffer(screenBuffer, sizeof(screenBuffer), &colors[0][0], SCREEN_HEIGHT, SCREEN_WIDTH);    

            printf(screenBuffer);

            usleep(60000);

    }


}
