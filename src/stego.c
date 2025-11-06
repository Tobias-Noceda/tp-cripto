#include "args.h"

#include <arpa/inet.h>
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

typedef struct
{
    uint32_t size;
    uint8_t data[];
} Encrypted;

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
            length = args.algorithm.encrypt(memory, length, args.password, args.mode.val, &ciphertext);

            Encrypted *tmp = realloc(memory, sizeof(Encrypted) + length);
            if (!tmp)
            {
                perror("Failed to get input message");

                free(memory);
                fclose(porter);

                return EXIT_FAILURE;
            }

            tmp->size = htonl(length);

            memcpy(tmp->data, ciphertext, length);

            LOG("Encryption size: %zu\n", length);

            memory = (uint8_t *)tmp;
            length += sizeof(Encrypted);

            free(ciphertext);
        }

        if (!args.stego.embed(porter, memory, length))
        {
            fprintf(stderr, "Failed to embed data.\n");

            free(memory);
            fclose(porter);

            return EXIT_FAILURE;
        }

        free(memory);
        fclose(porter);

        printf("Data embedded successfully into '%s'.\n", args.output_path);
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
        Stego *stego = args.stego.retrieve(porter, header_size, args.ssl || args.no_extension ? NULL : &extension);
        fclose(porter);

        if (stego == NULL)
        {
            fprintf(stderr, "Failed to retrieve stego data.\n");
            free(extension);
            return EXIT_FAILURE;
        }

        if (args.ssl)
        {
            uint8_t *plaintext = NULL;
            const size_t extracted = args.algorithm.decrypt(stego->data, stego->size, args.password, args.mode.val, &plaintext);

            free(stego);

            if (!plaintext)
            {
                perror("Failed to decrypt");
                return EXIT_FAILURE;
            }

            stego = (Stego *)plaintext;
            stego->size = ntohl(stego->size);
            LOG("Decrypted data size: %u bytes\n", stego->size);

            if (stego->size > extracted - sizeof(uint32_t))
            {
                fprintf(stderr, "Decrypted size is larger than extracted data.\n");
                free(plaintext);
                return EXIT_FAILURE;
            }

            extension = args.no_extension ? NULL : strdup((char *)(plaintext + sizeof(uint32_t) + stego->size));

            LOG("Decrypted length: %zu bytes\n", extracted);
        }

        LOG("Extracted data size: %u bytes\n", stego->size);
        LOG("Extracted data %.*s\n", stego->size, stego->data);
        LOG("Extracted data extension: %s\n", extension);

        char *path = set_output(args.output_path, extension, stego->data, stego->size);
        free(extension);
        free(stego);

        if (path == NULL)
        {
            perror("Failed to write to output file");
            return EXIT_FAILURE;
        }

        printf("Data extracted successfully to '%s'.\n", path);
        free(path);
    }

    return EXIT_SUCCESS;
}
