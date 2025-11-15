#ifndef WRITERS_H
#define WRITERS_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * @brief Write data into the output file using LSB1 steganography.
 *
 * @note The function does not reset the file header position
 *
 * @param output The output file where data will be embedded (writes at current header position)
 * @param input The input data to be embedded
 * @param size The size of the input data
 * @return size_t The number of bytes successfully embedded, or 0 on failure
 */
size_t embed_data_lsb1(FILE *output, const uint8_t *input, size_t size);

/**
 * @brief Write data into the output file using LSB4 steganography.
 *
 * @note The function does not reset the file header position
 *
 * @param output The output file where data will be embedded (writes at current header position)
 * @param input The input data to be embedded
 * @param size The size of the input data
 * @return size_t The number of bytes successfully embedded, or 0 on failure
 */
size_t embed_data_lsb4(FILE *output, const uint8_t *input, size_t size);

/**
 * @brief Write data into the output file using LSBI steganography.
 *
 * @note The function does not reset the file header position
 *
 * @param output The output file where data will be embedded (writes at current header position)
 * @param input The input data to be embedded
 * @param size The size of the input data
 * @return size_t The number of bytes successfully embedded, or 0 on failure
 */
size_t embed_data_lsbi(FILE *output, const uint8_t *input, size_t size);

/**
 * @brief Save the data to the specified output file.
 *
 * @param path The base path for the output file
 * @param extension The extension to append to the output file (can be NULL)
 * @param data The data to write to the output file
 * @param size The size of the data to write
 * @return char* The full path of the created output file, or NULL on failure, must be freed
 */
char *set_output(const char *path, const char *extension, const uint8_t *data, size_t size);

#endif
