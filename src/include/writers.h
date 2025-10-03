#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef size_t (*Writer)(FILE *output, const uint8_t *input, size_t size);

/**
 * @brief Write data into the output file using LSB1 steganography.
 *
 * @note The function moves the file header position as it writes data.
 *
 * @param output The output file where data will be embedded (writes at current header position)
 * @param input The input data to be embedded
 * @param size The size of the input data
 * @return size_t The number of bytes successfully embedded, or 0 on failure
 */
size_t embed_data_lsb1(FILE *output, const uint8_t *input, size_t size);
