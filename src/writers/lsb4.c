#include <writers.h>
#include <logs.h>

size_t embed_data_lsb4(FILE *output, const uint8_t *input, size_t size)
{
    size *= 2;

    uint8_t *data = malloc(size);
    if (data == NULL)
    {
        perror("Memory allocation failed");
        return 0;
    }

    if (fread(data, sizeof(uint8_t), size, output) != size)
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
        uint8_t byte = input[curr_byte / 2];

        LOG("Embedding byte: %02X\n", byte);
        for (int i = 1; i >= 0; i--)
        {
            uint8_t bit = (byte >> (i * 4)) & 0x0F;
            LOG("Embedding bit: %d\n", bit);

            data[curr_byte] = (data[curr_byte] & 0xF0) | bit;
            curr_byte++;
        }
    }

    size_t written = fwrite(data, sizeof(uint8_t), size, output);
    free(data);

    return written;
}
