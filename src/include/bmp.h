#ifndef BMP_H
#define BMP_H

#include <stdio.h>
#include <stdint.h>

#pragma pack(push, 1)

typedef struct
{
    uint16_t signature; // Specifies the file type, must be "BM" (0x4D42)
    uint32_t size;      // Specifies the size of the file in bytes
    uint16_t rsv1;      // Reserved; must be 0
    uint16_t rsv2;      // Reserved; must be 0
    uint32_t offset;    // Specifies the offset to the pixel data
} BITMAPFILEHEADER;

typedef struct
{
    uint32_t size;             // Specifies the size of the BITMAPINFOHEADER structure in bytes
    int32_t width;             // Specifies the width of the bitmap in pixels
    int32_t height;            // Specifies the height of the bitmap in pixels
    uint16_t planes;           // Specifies the number of planes for the target device, must be 1
    uint16_t bit_count;        // Specifies the number of bits per pixel
    uint32_t compression;      // Specifies the type of compression
    uint32_t image_size;       // Specifies the size of the image data in bytes
    int32_t XPxlsPerMeter;     // Specifies the horizontal resolution in pixels per meter
    int32_t YPxlsPerMeter;     // Specifies the vertical resolution in pixels per meter
    uint32_t colors;           // Specifies the number of colors used in the color palette
    uint32_t important_colors; // Specifies the number of important colors
} BITMAPINFOHEADER;

#pragma pack(pop)

/**
 * @brief Get the BMP file header data
 * 
 * @param file The file pointer
 * @return BITMAPFILEHEADER The BMP file header
 */
BITMAPFILEHEADER get_bmp_file_header(FILE *file);

#endif
