#include "../include/bmp.h"

BITMAPFILEHEADER get_bmp_file_header(FILE *file)
{
    fseek(file, 0, SEEK_SET);

    BITMAPFILEHEADER header;
    if (fread(&header, sizeof(BITMAPFILEHEADER), 1, file) < 1) {
        perror("Failed to read BMP file header: file may be truncated or corrupted");
        return (BITMAPFILEHEADER){0};
    }

    return header;
}
