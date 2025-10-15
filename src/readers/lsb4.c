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
        fclose(file);
        return NULL;
    }

    const uint32_t message_length = get_length(file);
    if (message_length == 0)
    {
        fclose(file);
        return NULL;
    }

    LOG("Message length: %u\n", message_length);

    // Allocate memory for the message
    uint8_t *message = malloc(message_length);
    if (message == NULL)
    {
        perror("Memory allocation for message failed");
        fclose(file);
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

            free(message);
            fclose(file);

            return NULL;
        }

        message[i / 2] <<= 4;
        message[i / 2] |= (byte & 0x0F);
    }

    LOG("Message: %.*s\n", message_length, message);

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
                void *tmp = realloc(*extension, length + EXTENSION_BLOCK_LENGTH);
                if (tmp == NULL)
                {
                    perror("Memory allocation for extension failed");

                    free(*extension);
                    free(stego);
                    fclose(file);

                    return NULL;
                }

                *extension = tmp;
            }

            uint8_t byte = 0;
            for (size_t i = 0; i < 2; i++)
            {
                char image_byte = fgetc(file);
                LOG("Image byte: %02x\n", image_byte);

                if (feof(file))
                {
                    perror("Error reading file");
                    free(*extension);
                    free(stego);
                    fclose(file);
                    return NULL;
                }

                byte <<= 4;
                byte |= (image_byte & 0x0F);
            }

            LOG("Extension byte: %c\n", byte);
            (*extension)[length++] = byte;

            if (!byte)
            {
                break;
            }
        }
    }

    fclose(file);
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
