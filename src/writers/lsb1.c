#include "../include/writers.h"

size_t embed_data_lsb1(FILE *output, const uint8_t *input, size_t size)
{
    size *= 8;

    uint8_t *data = malloc(size);
    if (data == NULL)
    {
        perror("Memory allocation failed");
        return 0;
    }
    
    if (fread(data, 1, size, output) != size)
    {
        if (feof(output))
        {
            fprintf(stderr, "Error: Output file is too small to hold the data.\n");
        }
        else
        {
            perror("Error reading output file");
        }

        free(data);
        return 0;
    }
    fseek(output, -size, SEEK_CUR);

    size_t curr_byte = 0;
    while (curr_byte < size)
    {
        uint8_t byte = input[curr_byte / 8];

        printf("Embedding byte: %02X\n", byte);
        for (int i = 7; i >= 0; i--)
        {
            uint8_t bit = (byte >> i) & 1;
            printf("Embedding bit: %d\n", bit);
            
            data[curr_byte] = (data[curr_byte] & 0xFE) | bit;
            curr_byte++;
        }
    }

    size_t written = fwrite(data, 1, size, output);
    free(data);

    return written;
}
