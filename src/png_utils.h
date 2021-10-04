/* aidan bird 2021 */
#ifndef PNG_UTILS_H 
#define PNG_UTILS_H 

#include <png.h> 
#include <stdio.h> 
#include <stdint.h>

typedef struct Img Img;
typedef struct Rgba Rgba;

struct Rgba
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct Img
{
    png_bytep *grid;
    int32_t w;
    int32_t h;
};

png_bytep *openPng(char *path, FILE **fp, png_struct **png, png_info **info);
int writePng(int bitDepth, Img *bmp, FILE *fp);

#endif
