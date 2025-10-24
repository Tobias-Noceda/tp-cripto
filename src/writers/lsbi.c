#include <writers.h>
#include <logs.h>
#include <stdbool.h>

#define PATTERN_SIZE 4
#define LSB(x, n) ((x) & ((1 << (n)) - 1))
#define GET_BYTES_NEEDED(x) (((x) * 3 + 1) / 2)


size_t embed_data_lsbi(FILE *output, const uint8_t *input, size_t size)
{
    /*
    This array will define if inversion is needed for each pattern.
    pattern[0]: 00              pattern[1]: 01
    pattern[2]: 10              pattern[3]: 11
    If value is gt 0, invert the LSB.

    Differences will be ++, matches --.
    */
    int pattern[PATTERN_SIZE] = {0};
    size_t pattern_size = PATTERN_SIZE;

    uint8_t *pattern_data = malloc(pattern_size);
    if (pattern_data == NULL)
    {
        perror("Memory allocation failed");
        return 0;
    }

    long start_offset = ftell(output);
    if (start_offset < 0)
    {
        perror("ftell failed");
        free(pattern_data);
        return 0;
    }
    
    if (fread(pattern_data, sizeof(uint8_t), pattern_size, output) != pattern_size)
    {
        if (feof(output))
        {
            fprintf(stderr, "Error: Output file is too small to hold the pattern.\n");
        }
        else
        {
            perror("Error reading output file");
        }

        free(pattern_data);
        return 0;
    }

    size *= 8;

    size_t bits_needed = size;
    size_t bytes_needed = GET_BYTES_NEEDED(bits_needed);

    uint8_t *data = malloc(bytes_needed);
    if (data == NULL)
    {
        perror("Memory allocation failed");
        free(pattern_data);
        return 0;
    }
    
    if (fread(data, sizeof(uint8_t), bytes_needed, output) != bytes_needed)
    {
        if (feof(output))
        {
            fprintf(stderr, "Error: Output file is too small to hold the data.\n");
        }
        else
        {
            perror("Error reading output file");
        }

        free(pattern_data);
        free(data);
        
        return 0;
    }
    if (fseek(output, start_offset, SEEK_SET))
    {
        perror("fseek failed");

        free(pattern_data);
        free(data);
        
        return 0;
    }
    
    size_t curr_byte = 0;
    size_t curr_byte_input = 0;
    size_t total_bits = bits_needed;
    while(curr_byte_input < total_bits)
    {
        uint8_t byte = input[curr_byte_input / 8];

        //LOG("Embedding byte: %02X\n", byte);
        for (int i = 7 ; i >= 0 && curr_byte_input < total_bits ; i--)
        {
            uint8_t bit = (byte >> i) & 1;
            //LOG("Embedding bit: %d\n", bit);

            if((curr_byte % 3) == 1) // it will be a R
            {
                curr_byte++;
                if (curr_byte >= bytes_needed)
                {
                    fprintf(stderr, "Error: Overflow while skipping R\n");
                    free(pattern_data);
                    free(data);
                    return 0;
                }
            }

            if (curr_byte >= bytes_needed)
            {
                fprintf(stderr, "Error: Overflow before accessing data\n");
                free(pattern_data);
                free(data);
                return 0;
           }

            uint8_t value = data[curr_byte];
            uint8_t last3 = LSB(value, 3);
            int pattern_index = last3 >> 1;
            //LOG("Pattern of bit: %d\n", pattern_index);
            if (bit == (last3 & 1))
            {
                pattern[pattern_index]--;
            } else {
                pattern[pattern_index]++;
            }
            
            data[curr_byte] = (data[curr_byte] & 0xFE) | bit;
            curr_byte++;
            curr_byte_input++;
            if (curr_byte >= bytes_needed)
            {
                fprintf(stderr, "Error: Overflow during embedding\n");
                free(pattern_data);
                free(data);
                return 0;
            }
        }
    }

    bool has_to_invert = false;
    for(int i = 0; i < PATTERN_SIZE; i++)
    {
        if(pattern[i] > 0)
        {
            has_to_invert = true;
            pattern[i] = 1;
        } else {
            pattern[i] = 0;
        }
    }

    LOG("Invertion needed: %s\n", has_to_invert ? "true" : "false");
    
    if(has_to_invert)
    {
        curr_byte = 0;
        while (curr_byte < bytes_needed)
        {
            if ((curr_byte % 3) != 1)   // not R
            {
                uint8_t value = data[curr_byte];
                uint8_t last3 = LSB(value, 3);
                int pattern_index = last3 >> 1;

                data[curr_byte] ^= (uint8_t)pattern[pattern_index];
            }
            curr_byte++;
        }
    }

    for(int i = 0 ; i < PATTERN_SIZE ; i++)
    {
        LOG("Pattern %d inversion: %d\n", i, pattern[i]);
        pattern_data[i] = (pattern_data[i] & 0xFE) | pattern[i];
    }

    size_t written = fwrite(pattern_data, sizeof(uint8_t), pattern_size, output);

    if (written != pattern_size)
    {
        perror("Error writing to output file");
        free(pattern_data);
        free(data);
        return 0;
    }

    written = fwrite(data, sizeof(uint8_t), bytes_needed, output);
    
    if (written != bytes_needed)
    {
        perror("Error writing to output file");
        free(pattern_data);
        free(data);
        return 0;
    }

    free(pattern_data);
    free(data);

    return pattern_size + bytes_needed;
}
