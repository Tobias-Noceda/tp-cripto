#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <stego.h>
#include <logs.h>

Data get_message(const char *input, uint8_t **memory, uint32_t *length)
{
    Data input_data = {
        .size = NULL,
        .data = NULL,
        .ext = NULL
    };

    FILE *file = fopen(input, "rb");
    if (file == NULL)
    {
        perror("Error opening input file");
        return (Data){};
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size < 0)
    {
        perror("Error determining file size");
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

        *length = 0;
        fclose(file);

        return (Data){};
    }

    input_data.size = (uint32_t *)(*memory);
    input_data.data = (char *)(*memory + sizeof(uint32_t));
    input_data.ext = (char *)(*memory + sizeof(uint32_t) + file_size);

    
    size_t read_size = fread(input_data.data, 1, file_size, file);
    if (read_size != file_size)
    {
        perror("Error reading file");
        free(*memory);
        fclose(file);
        return (Data){};
    }
    
    fclose(file);
    *input_data.size = (uint32_t)file_size;
    
    memcpy(input_data.ext, dot, strlen(dot) + 1); // Copy extension with null terminator

    LOG("Input file size: %u bytes\n", *input_data.size);
    LOG("Input file data: %.*s\n", *input_data.size, input_data.data);
    LOG("Input file extension: %s\n", input_data.ext);

    return input_data;
}
