#ifndef ARGS_H
#define ARGS_H

#include <stdbool.h>

#include <stego.h>
#include <crypto.h>

/**
 * @brief Stego method structure
 */
typedef struct
{
    char *name;
    Writer embed;
    Reader retrieve;
} StegoMethod;

typedef struct
{
    const char *name;
    Encrypt encrypt;
    Decrypt decrypt;
} CipherAlgorithms;

typedef struct
{
    const char *name;
    CIPHER_MODES val;
} CipherMode;

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
    // Encryption enabled
    bool ssl;
    // Cipher algorithm
    CipherAlgorithms algorithm;
    // Cipher mode
    CipherMode mode;
    // Password for encryption/decryption
    char *password;
    // Do not restore original file extension when extracting
    bool no_extension;
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
