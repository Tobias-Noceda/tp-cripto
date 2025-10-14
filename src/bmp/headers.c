#include "../include/bmp.h"

BITMAPFILEHEADER get_bmp_file_header(FILE *file)
{
    BITMAPFILEHEADER header;
    fseek(file, 0, SEEK_SET);
    fread(&header, sizeof(BITMAPFILEHEADER), 1, file);
    return header;
}
