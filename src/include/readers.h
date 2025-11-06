#ifndef READERS_H
#define READERS_H

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
 * @note The caller is responsible for freeing the allocated memory.
 *
 * @param input The input file
 * @param memory Pointer to consecutive allocated memory
 * @return size_t The size of the allocated memory, or 0 on failure
 */
size_t get_message(const char *input, uint8_t **memory);

/**
 * @brief Retrieve a steganographed message using LSB1 method
 *
 * @param file The file pointer to read from
 * @param offset The offset to start reading from
 * @param extension Pointer to store the file extension
 * @return char* The retrieved message
 */
Stego *retrieve_lsb1(FILE *file, size_t offset, char **extension);

/**
 * @brief Retrieve a steganographed message using LSB4 method
 *
 * @param file The file pointer to read from
 * @param offset The offset to start reading from
 * @param extension Pointer to store the file extension
 * @return char* The retrieved message
 */
Stego *retrieve_lsb4(FILE *file, size_t offset, char **extension);

/**
 * @brief Retrieve a steganographed message using LSBI method
 *
 * @param file The file pointer to read from
 * @param offset The offset to start reading from
 * @param extension Pointer to store the file extension
 * @return Stego* The retrieved message
 */
Stego *retrieve_lsbi(FILE *file, size_t offset, char **extension);

#endif
