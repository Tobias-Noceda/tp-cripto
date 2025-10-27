#include "args.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <readers.h>
#include <writers.h>
#include <logs.h>
#include <bmp.h>
#include <crypto.h>

#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    Arguments args = get_args(argc, argv);

    if (args.embed)
    {
        LOG("Embedding mode:\n");
        LOG("Input file: %s\n", args.input_path);
        LOG("Porter file: %s\n", args.porter_path);
        LOG("Output file: %s\n", args.output_path);
        LOG("Stego type: %s\n", args.stego.name);

        FILE *porter;
        long porter_size = get_output(&porter, args.output_path, args.porter_path);
        if (porter_size == 0)
        {
            perror("Failed to open porter file");
            return EXIT_FAILURE;
        }

        const BITMAPFILEHEADER header = get_bmp_file_header(porter);
        if (header.signature != 0x4D42)
        {
            fprintf(stderr, "Porter file is not a valid BMP file.\n");
            fclose(porter);
            return EXIT_FAILURE;
        }

        uint32_t header_size = header.offset;
        fseek(porter, header_size, SEEK_SET); // Skip BMP header

        uint8_t *memory;
        size_t length;
        get_message(args.input_path, &memory, &length);
        if (!memory)
        {
            perror("Failed to get input message");
            fclose(porter);
            return EXIT_FAILURE;
        }

        if (args.ssl)
        {
            uint8_t *ciphertext;
            length = args.algorithm.encrypt(memory, length, (uint8_t *)args.password, args.mode.val, &ciphertext);
            free(memory);
            memory = ciphertext;
        }

        if (!args.stego.embed(porter, memory, length))
        {
            fprintf(stderr, "Failed to embed data.\n");

            free(memory);
            fclose(porter);

            return EXIT_FAILURE;
        }

        printf("Data embedded successfully into '%s'.\n", args.output_path);

        // free memory and close files
        free(memory);
        fclose(porter);
    }
    else
    {
        LOG("Extracting mode:\n");
        LOG("Porter file: %s\n", args.porter_path);
        LOG("Output file: %s\n", args.output_path);

        FILE *porter = fopen(args.porter_path, "rb");
        if (porter == NULL)
        {
            perror("Error opening porter");
            return EXIT_FAILURE;
        }

        const BITMAPFILEHEADER header = get_bmp_file_header(porter);
        if (header.signature != 0x4D42)
        {
            fprintf(stderr, "Porter file is not a valid BMP file.\n");
            fclose(porter);
            return EXIT_FAILURE;
        }

        const uint32_t header_size = header.offset;
        LOG("BMP Header size: %u\n", header_size);

        char *extension = NULL;
        Stego *stego = args.stego.retrieve(porter, header_size, args.ssl ? NULL : &extension);
        fclose(porter);

        if (args.ssl)
        {
            uint8_t *plaintext = NULL;
            args.algorithm.decrypt(stego->data, stego->size, (uint8_t *)args.password, args.mode.val, &plaintext);

            if (plaintext != NULL)
            {
                Data decrypted_data = {
                    .size = ntohl(*(uint32_t *)plaintext),
                    .sizep = (uint32_t *)plaintext,
                    .data = (char *)(plaintext + sizeof(uint32_t)),
                    .ext = (char *)(plaintext + sizeof(uint32_t) + *(uint32_t *)plaintext)};

                LOG("Decrypted data size: %u bytes\n", decrypted_data.size);
                LOG("Decrypted data content: %.*s\n", decrypted_data.size, decrypted_data.data);
                LOG("Decrypted data extension: %s\n", decrypted_data.ext);

                free(stego);
                stego = malloc(sizeof(Stego) + *decrypted_data.size);
                if (stego == NULL)
                {
                    perror("Memory allocation failed");
                    free(extension);
                    return EXIT_FAILURE;
                }

                stego->size = *decrypted_data.size;
                memcpy(stego->data, decrypted_data.data, *decrypted_data.size);
                extension = strdup(decrypted_data.ext);

                free(plaintext);
            }
        }
        else
        {
            LOG("Extracted data size: %u bytes\n", stego->size);
            LOG("Extracted data %.*s\n", (int)stego->size, stego->data);
            LOG("Extracted data extension: %s\n", extension);
        }

        if (stego == NULL)
        {
            fprintf(stderr, "Failed to retrieve stego data.\n");
            free(extension);
            return EXIT_FAILURE;
        }

        char *full_output_file_name;
        if (extension != NULL)
        {
            full_output_file_name = malloc(strlen(args.output_path) + strlen(extension) + 2);
            if (full_output_file_name == NULL)
            {
                perror("Memory allocation failed");
                free(stego);
                free(extension);
                return EXIT_FAILURE;
            }
            sprintf(full_output_file_name, "%s%s", args.output_path, extension);
        }
        else
        {
            full_output_file_name = strdup(args.output_path);
            if (full_output_file_name == NULL)
            {
                perror("Memory allocation failed");
                free(stego);
                return EXIT_FAILURE;
            }
        }

        FILE *output_file = fopen(full_output_file_name, "wb");
        if (output_file == NULL)
        {
            perror("Failed to open output file");
            free(stego);
            free(extension);
            free(full_output_file_name);
            return EXIT_FAILURE;
        }

        size_t written = fwrite(stego->data, 1, stego->size, output_file);
        if (written != stego->size)
        {
            perror("Failed to write all data to output file");
            free(stego);
            free(extension);
            free(full_output_file_name);
            fclose(output_file);
            return EXIT_FAILURE;
        }

        printf("Data extracted successfully to '%s'.\n", full_output_file_name);

        free(stego);
        free(extension);
        free(full_output_file_name);
        fclose(output_file);
    }

    return EXIT_SUCCESS;
}
