#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#include <readers.h>
#include <logs.h>

#define EXTENSION_BLOCK_LENGTH 8

static uint32_t get_length(FILE *file);

Stego *retrieve_lsb4(FILE *file, size_t offset, char **extension)
{
    if (fseek(file, offset, SEEK_SET) < 0)
    {
        perror("File too small for offset");
        return NULL;
    }

    const uint32_t message_length = get_length(file);
    if (message_length == 0)
    {
        return NULL;
    }

    LOG("Message length: %u\n", message_length);
    
    Stego *stego = malloc(sizeof(Stego) + message_length);
    if (stego == NULL)
    {
        perror("Memory allocation for Stego failed");
        return NULL;
    }
    
    // Read the message bits
    for (size_t i = 0; i < message_length * 2; i++)
    {
        char byte = fgetc(file);

        if (feof(file))
        {
            perror("Not enough bytes to read message");
            LOG("Reached EOF at byte %zu\n", i);

            return NULL;
        }

        (stego->data)[i / 2] <<= 4;
        (stego->data)[i / 2] |= (byte & 0x0F);
    }

    stego->size = message_length;

    if (extension != NULL)
    {
                int length = 0;
        uint8_t byte = 0;
        do
        {
            if (length % EXTENSION_BLOCK_LENGTH == 0)
            {
                void *tmp = realloc(*extension, length + EXTENSION_BLOCK_LENGTH);
                if (tmp == NULL)
                {
                    perror("Memory allocation for extension failed");

                    free(*extension);
                    free(stego);

                    return NULL;
                }

                *extension = tmp;
            }

            for (size_t i = 0; i < 2; i++)
            {
                char image_byte = fgetc(file);
                LOG("Image byte: %02x\n", image_byte);

                if (feof(file))
                {
                    perror("Error reading file");
                    free(*extension);
                    free(stego);
                    return NULL;
                }

                byte <<= 4;
                byte |= (image_byte & 0x0F);
            }

            LOG("Extension byte: %c\n", byte);
            (*extension)[length++] = byte;
        } while (byte != 0);
    }

    return stego;
}

static uint32_t get_length(FILE *file)
{
    const size_t count = sizeof(uint32_t) * 2;
    uint8_t img_bytes[count];

    if (fread(img_bytes, sizeof(uint8_t), count, file) != count)
    {
        perror("Not enough bytes to read length");
        return 0;
    }

    uint32_t message_length = 0;
    for (size_t i = 0; i < count; i++)
    {
        uint8_t byte = img_bytes[i];
        message_length <<= 4;
        message_length |= byte & 0x0F;
    }

    return message_length;
}
