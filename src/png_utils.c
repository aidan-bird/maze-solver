/* aidan bird 2021 */
#include "png_utils.h"
#include <stdint.h>
    
png_bytep *
openPng(char *path, FILE **fp, png_struct **png, png_info **info)
{
    FILE *sfile;
    char header[8];
    png_struct *spng;
    png_info *sinfo;
    size_t height;
    size_t rowsize;
    size_t i;
    png_bytep *ret;
    uint64_t colourType;
    uint64_t bitDepth;

    if (!(sfile = fopen(path, "rb")))
        return NULL;
    fread(header, 1, sizeof(header), sfile);
    if (png_sig_cmp(header, 0, sizeof(header)))
        goto notAPng;
    if (!(spng = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)))
        goto pStructError;
    if (!(sinfo = png_create_info_struct(spng)))
        goto pInfoError;
    if (setjmp(png_jmpbuf(spng)))
        goto pInfoError;
    rewind(sfile);
    png_init_io(spng, sfile);
    png_read_info(spng, sinfo);
    colourType = png_get_color_type(spng, sinfo);
    bitDepth = png_get_bit_depth(spng, sinfo);
    if (colourType == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(spng);
    if (bitDepth == 16)
        png_set_strip_16(spng);
    if (colourType == PNG_COLOR_TYPE_GRAY && bitDepth < 8)
        png_set_expand_gray_1_2_4_to_8(spng);
    if (png_get_valid(spng, sinfo, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(spng);
    if (colourType == PNG_COLOR_TYPE_RGB
        || colourType == PNG_COLOR_TYPE_GRAY)
        png_set_add_alpha(spng, 0xFF, PNG_FILLER_AFTER);
    png_read_update_info(spng, sinfo);
    rowsize = png_get_rowbytes(spng, sinfo);
    height = png_get_image_height(spng, sinfo);
    // TODO cleanup before returning NULL
    if (!(ret = png_malloc(spng, sizeof(png_bytep) * height + height * rowsize)))
        return NULL;
    for (i = 0; i < height; i++)
        ret[i] = (void *)ret + sizeof(png_bytep) * height + rowsize * i;
    png_read_image(spng, ret);
    *png = spng;
    *info = sinfo;
    *fp = sfile;
    return ret;
pInfoError:;
pStructError:;
    png_destroy_read_struct(&spng, &sinfo, NULL);
notAPng:;
    fclose(sfile);
    return NULL;
}

int
writePng(int bitDepth, Img *bmp, FILE *fp)
{
    int errorState;
    png_struct *png;
    png_info *info;

    errorState = -1;
    if (!(png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,
        NULL)))
        return -1;
    if (!(info = png_create_info_struct(png)))
        goto error;
    if (setjmp(png_jmpbuf(png)))
        goto error;
	png_init_io(png, fp);
    png_set_IHDR(png, info, bmp->w, bmp->h, bitDepth, PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png, info);
    if (setjmp(png_jmpbuf(png)))
        goto error;
    png_write_image(png, bmp->grid);
	png_write_end(png, NULL);
    errorState = 0;
error:;
	png_destroy_write_struct (&png, &info);
    return errorState;
}

