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

typedef size_t (*Writer)(FILE *output, const uint8_t *input, size_t size);
typedef Stego *(*Reader)(FILE *file, size_t offset, char **extension);

typedef struct {
    char name[5];
    Writer embed;
    Reader retrieve;
} StegoMethod;
