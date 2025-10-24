#ifndef STEGO_H
#define STEGO_H

#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint32_t size;
    // big endian (network byte order) pointer to data size, preserved for legacy reasons
    uint32_t *sizep;
    char *data;
    char *ext;
} Data;

typedef struct {
    uint32_t size;
    uint8_t data[];
} Stego;

typedef size_t (*Writer)(FILE *output, const uint8_t *input, size_t size);
typedef Stego *(*Reader)(FILE *file, size_t offset, char **extension);

#endif
