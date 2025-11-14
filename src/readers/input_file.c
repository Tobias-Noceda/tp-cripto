#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <stego.h>
#include <logs.h>

struct Mapping
{
    // Big endian (network byte order) pointer to data size
    uint32_t *size;
    char *data;
    char *ext;
};

size_t get_message(const char *input, uint8_t **memory)
{
    FILE *file = fopen(input, "rb");
    if (file == NULL)
    {
        perror("Error opening input file");
        return 0;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size < 0)
    {
        perror("Error determining file size");
        fclose(file);
        return 0;
    }

    // Extract file extension including the dot
    const char *dot = strrchr(input, '.');
    if (!dot || dot == input)
    {
        dot = ""; // No extension found
    }

    size_t memory_length = sizeof(Stego) + file_size + strlen(dot) + 1;
    *memory = malloc(memory_length); // 4 bytes for size, file data, extension, null terminator
    if (*memory == NULL)
    {
        perror("Memory allocation failed");
        fclose(file);
        return 0;
    }

    struct Mapping memory_map = {
        .size = (uint32_t *)(*memory),
        .data = (char *)(*memory + sizeof(uint32_t)),
        .ext = (char *)(*memory + sizeof(uint32_t) + file_size),
    };

    *memory_map.size = htonl(file_size);

    size_t read_size = fread(memory_map.data, 1, file_size, file);
    fclose(file);

    if (read_size != file_size)
    {
        perror("Error reading file");
        free(*memory);
        return 0;
    }

    strcpy(memory_map.ext, dot);

    LOG("Input file size: %ld bytes\n", file_size);
    LOG("Input file data: %.*s\n", (int)file_size, memory_map.data);
    LOG("Input file extension: %s\n", memory_map.ext);

    return memory_length;
}
