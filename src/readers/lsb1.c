#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#include "../include/readers.h"

#define EXTENSION_BLOCK_LENGTH 8

Stego *retrieve_lsb1(const char *file_name, size_t offset, char **extension)
{
    FILE *file = fopen(file_name, "rb");
    if (file == NULL)
    {
        perror("Error opening file");
        return NULL;
    }

    if (fseek(file, offset, SEEK_SET) < 0)
    {
        perror("File too small for offset");
        fclose(file);
        return NULL;
    }

    // read the first 32 bits to get the message length
    uint32_t message_length = 0;
    for (size_t i = 0; i < 32; i++)
    {
        int byte = fgetc(file);
        if (byte == EOF)
        {
            perror("Error reading file");
            fclose(file);
            return NULL;
        }
        message_length = (message_length << 1) | (byte & 1);
    }

    printf("Message length: %u\n", message_length);

    // Allocate memory for the message
    char *message = malloc(message_length);
    if (message == NULL)
    {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    // Read the message bits
    for (uint32_t i = 0; i < message_length * 8; i++)
    {
        int byte = fgetc(file);
        if (byte == EOF)
        {
            perror("Error reading file");
            free(message);
            fclose(file);
            return NULL;
        }
        message[i / 8] = (message[i / 8] << 1) | (byte & 1);
    }

    printf("Message: %.*s\n", message_length, message);

    Stego *stego = malloc(sizeof(Stego) + message_length);
    if (stego == NULL)
    {
        perror("Memory allocation for Stego failed");
        free(message);
        fclose(file);
        return NULL;
    }
    stego->size = message_length;
    memcpy(stego->data, message, message_length);
    free(message);

    if (extension != NULL)
    {
        int length = 0;
        while (true)
        {
            if (length % EXTENSION_BLOCK_LENGTH == 0)
            {
                *extension = realloc(*extension, length + EXTENSION_BLOCK_LENGTH);
            }
            uint8_t byte = 0;
            for (int i = 0; i < 8; i++)
            {
                int image_byte = fgetc(file);
                printf("Image byte: %02x\n", image_byte);
                if (image_byte == EOF)
                {
                    perror("Error reading file");
                    free(stego);
                    fclose(file);
                    return NULL;
                }
                byte = (byte << 1) | (image_byte & 1);
            }
            printf("Extension byte: %c\n", byte);
            *extension[length++] = byte;

            if (byte == '\0')
            {
                break;
            }
        }
    }

    fclose(file);
    return stego;
}