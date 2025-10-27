#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#include <readers.h>
#include <logs.h>

#define PATTERN_SIZE 4
#define EXTENSION_BLOCK_LENGTH 8
#define LSB(x, n) ((x) & ((1 << (n)) - 1))
#define GET_BYTES_NEEDED(x) (((x) * 3 + 1) / 2)


Stego *retrieve_lsbi(FILE *file, size_t offset, char **extension)
{
    if (fseek(file, offset, SEEK_SET) < 0)
    {
        perror("Failed to seek to offset in file");
        return NULL;
    }

    int pattern[PATTERN_SIZE] = {0};

    uint8_t pattern_bytes[PATTERN_SIZE];
    
    if (fread(pattern_bytes, sizeof(uint8_t), PATTERN_SIZE, file) != PATTERN_SIZE)
    {
        perror("Not enough bytes to read pattern");
        return NULL;
    }

    for(int i = 0 ; i < PATTERN_SIZE ; i++)
    {
        pattern[i] = pattern_bytes[i] & 1;
        LOG("Pattern %d inversion: %d\n", i, pattern[i]);
    }

    /*
                0  1  2  3  4  5 ...
    B  G  R  B  G  R  B  G  R  B ...
    00 01 10 11 S  -  I  Z  -  E ...
    */

    size_t bits_needed = sizeof(uint32_t) * 8;
    size_t bytes_needed = GET_BYTES_NEEDED(bits_needed);
    uint8_t length_bytes[bytes_needed];

    if (fread(length_bytes, sizeof(uint8_t), bytes_needed, file) != bytes_needed)
    {
        perror("Not enough bytes to read length");
        return NULL;
    }

    uint32_t message_length = 0;
    size_t bits_read = 0;
    for (int i = 0 ; i < bytes_needed && bits_read < bits_needed ; i++)
    {
        if (i % 3 != 1) // not R
        {
            uint8_t byte = length_bytes[i];
            uint8_t last3 = LSB(byte, 3);
            int pattern_index = last3 >> 1;
            uint8_t bit = (last3 & 1) ^ pattern[pattern_index];

            message_length = (message_length << 1) | (bit & 1);

            bits_read++;
        }
    }

    if (bits_read < bits_needed)
    {
        LOG("Could not read enough bits for length: got %zu expected %zu\n", bits_read, bits_needed);
        return NULL;
    }

    LOG("Message length: %u\n", message_length);

    Stego *stego = malloc(sizeof(Stego) + message_length);
    if (stego == NULL)
    {
        perror("Memory allocation for Stego failed");
        return NULL;
    }

    size_t total_bits = (size_t)message_length * 8;
    size_t bits_collected = 0;
    int index = 0;

    while (bits_collected < total_bits)
    {
        char byte = 0;
        do {
            byte = fgetc(file);
            if (feof(file))
            {
                perror("Not enough bytes to read message");
                free(stego);
                return NULL;
            }
        } while (index++ % 3 == 1); // skip R

        uint8_t last3 = LSB((uint8_t)byte, 3);
        int pattern_index = last3 >> 1;
        uint8_t bit = (last3 & 1) ^ pattern[pattern_index];

        size_t idx = bits_collected / 8;
        (stego->data)[idx] <<= 1;
        (stego->data)[idx] |= (bit & 1);

        bits_collected++;
    }

    stego->size = message_length;

    if (extension != NULL)
    {
        int length = 0;
        uint8_t ext_byte = 0;
        int ext_index = 0;

        do
        {
            if (length % EXTENSION_BLOCK_LENGTH == 0)
            {
                void *tmp = realloc(*extension, length + EXTENSION_BLOCK_LENGTH);
                if (tmp == NULL)
                {
                    perror("Memory allocation for extension failed");
                    free(stego);
                    return NULL;
                }

                *extension = tmp;
            }

            ext_byte = 0;
            for (size_t i = 0; i < 8; i++)
            {
                int image_byte;

                do
                {
                    image_byte = fgetc(file);
                    LOG("Image byte: %02x\n", image_byte);
                    if (feof(file))
                    {
                        perror("Error reading file");
                        free(stego);
                        return NULL;
                    }
                } while (ext_index++ % 3 == 1); // skip R

                uint8_t last3 = LSB(image_byte, 3);
                int pattern_index = last3 >> 1;
                uint8_t bit = (last3 & 1) ^ pattern[pattern_index];
                
                ext_byte = (ext_byte << 1) | bit;
            }

            LOG("Extension byte: %c\n", ext_byte);
            (*extension)[length++] = ext_byte;

        } while (ext_byte != 0);
    }
 
    return stego;
}
