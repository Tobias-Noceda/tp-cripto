#include "../include/bmp.h"

BITMAPFILEHEADER get_bmp_file_header(FILE *file)
{
    fseek(file, 0, SEEK_SET);

    BITMAPFILEHEADER header;
    if (fread(&header, sizeof(BITMAPFILEHEADER), 1, file) < 1) {
        perror("Error reading BMP file header");
        return (BITMAPFILEHEADER){0};
    }

    return header;
}
