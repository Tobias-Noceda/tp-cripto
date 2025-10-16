#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <stego.h>

Data get_message(const char *input)
{
    Data input_data = {
        .size = 0,
        .data = NULL,
        .ext = NULL};

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

    input_data.data = malloc(file_size);
    if (input_data.data == NULL)
    {
        perror("Memory allocation failed");
        fclose(file);
        return (Data){};
    }

    size_t read_size = fread(input_data.data, 1, file_size, file);
    if (read_size != file_size)
    {
        perror("Error reading file");
        free(input_data.data);
        fclose(file);
        return (Data){};
    }

    fclose(file);
    input_data.size = (uint32_t)file_size;

    // Extract file extension including the dot
    const char *dot = strrchr(input, '.');
    if (!dot || dot == input)
    {
        dot = ""; // No extension found
    }

    input_data.ext = strdup(dot);
    if (input_data.ext == NULL)
    {
        perror("Memory allocation for extension failed");
        free(input_data.data);
        input_data.data = NULL;
        input_data.size = 0;
        return (Data){};
    }

    LOG("Input file size: %u bytes\n", input_data.size);
    LOG("Input file extension: %s\n", input_data.ext);
    LOG("Input file data: %.*s\n", input_data.size, input_data.data);

    return input_data;
}
