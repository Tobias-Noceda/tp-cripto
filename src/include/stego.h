#include <stdint.h>

typedef struct {
    uint32_t size;
    char *data;
    char *ext;
} Data;

typedef struct {
    uint32_t size;
    uint8_t data[];
} Stego;

