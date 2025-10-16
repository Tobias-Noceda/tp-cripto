#ifndef ARGS_H
#define ARGS_H

#include <stdbool.h>

#include <stego.h>

/**
 * @brief Stego method structure
 */
typedef struct
{
    char *name;
    Writer embed;
    Reader retrieve;
} StegoMethod;

/**
 * @brief The structure that holds the program arguments
 */
typedef struct Arguments
{
    // Embed mode
    bool embed;
    // Extract mode
    bool extract;
    // Steganography method
    StegoMethod stego;
    // Porter file
    char *porter_path;
    // File to embed
    char *input_path;
    // Output file (bmp when embedding, input file when extracting)
    char *output_path;
} Arguments;

/**
 * @brief Get the args object from argv
 *
 * @param argc Argument count
 * @param argv Argument values
 * @return Arguments the parsed arguments
 */
Arguments get_args(int argc, char *argv[]);

#endif
