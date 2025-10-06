#include <stdio.h>

#include <stego.h>

/**
 * @brief Get the output file
 *
 * @example
 * FILE *file;
 * size_t size = get_output(&file, "output.bmp", "input.bmp");
 * if (size == 0) {
 *     // Handle error
 * }
 *
 * @param file The output file pointer
 * @param output The output file name
 * @param input The input file name
 * @return size_t The size of the file
 */
size_t get_output(FILE **file, const char *output, const char *input);

/**
 * @brief Get the message data
 * 
 * @param input The input file
 * @return Data The message data
 */
Data get_message(const char *input);
