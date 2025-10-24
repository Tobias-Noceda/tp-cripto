#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <stego.h>
#include <logs.h>

Data get_message(const char *input, uint8_t **memory, size_t *length)
{
    FILE *file = fopen(input, "rb");
    if (file == NULL)
    {
        perror("Error opening input file");
        *memory = NULL;
        return (Data){};
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size < 0)
    {
        perror("Error determining file size");

        *memory = NULL;
        fclose(file);

        return (Data){};
    }

    // Extract file extension including the dot
    const char *dot = strrchr(input, '.');
    if (!dot || dot == input)
    {
        dot = ""; // No extension found
    }

    *length = sizeof(uint32_t) + file_size + strlen(dot) + 1;
    *memory = malloc(*length); // 4 bytes for size, file data, extension, null terminator
    if (*memory == NULL)
    {
        perror("Memory allocation failed");
        fclose(file);
        return (Data){};
    }

    Data input_data = {
        .size = file_size,
        .sizep = (uint32_t *)(*memory),
        .data = (char *)(*memory + sizeof(uint32_t)),
        .ext = (char *)(*memory + sizeof(uint32_t) + file_size),
    };

    *input_data.sizep = htonl(input_data.size);

    size_t read_size = fread(input_data.data, 1, file_size, file);
    if (read_size != file_size)
    {
        perror("Error reading file");

        free(*memory);
        *memory = NULL;
        fclose(file);

        return (Data){};
    }

    fclose(file);

    memcpy(input_data.ext, dot, strlen(dot) + 1); // Copy extension with null terminator

    LOG("Input file size: %u bytes\n", input_data.size);
    LOG("Input file data: %.*s\n", input_data.size, input_data.data);
    LOG("Input file extension: %s\n", input_data.ext);

    return input_data;
}
