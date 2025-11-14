#include "args.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <err.h>

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
            err(EXIT_FAILURE, "Failed to open porter file");
        }

        const BITMAPFILEHEADER header = get_bmp_file_header(porter);
        if (header.signature != 0x4D42)
        {
            fclose(porter);
            errx(EXIT_FAILURE, "Porter file is not a valid BMP file");
        }

        uint32_t header_size = header.offset;
        fseek(porter, header_size, SEEK_SET); // Skip BMP header

        uint8_t *memory;
        size_t length = get_message(args.input_path, &memory);
        if (!length)
        {
            fclose(porter);
            err(EXIT_FAILURE, "Failed to get input message");
        }

        if (args.ssl)
        {
            uint8_t *ciphertext;
            size_t encrypted = args.algorithm.encrypt(memory, length, args.password, args.mode.val, &ciphertext);
            LOG("Encryption size: %zu\n", encrypted);

            Encrypted *tmp = realloc(memory, sizeof(Encrypted) + encrypted);
            if (!tmp)
            {
                free(memory);
                free(ciphertext);
                fclose(porter);

                err(EXIT_FAILURE, "Memory reallocation failed");
            }

            tmp->size = htonl(encrypted);
            memcpy(tmp->data, ciphertext, encrypted);

            memory = (uint8_t *)tmp;
            length = sizeof(Encrypted) + encrypted;

            free(ciphertext);
        }

        if (!args.stego.embed(porter, memory, length))
        {
            free(memory);
            fclose(porter);
            errx(EXIT_FAILURE, "Failed to embed data");
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
            err(EXIT_FAILURE, "Error opening porter");
        }

        const BITMAPFILEHEADER header = get_bmp_file_header(porter);
        if (header.signature != 0x4D42)
        {
            fclose(porter);
            errx(EXIT_FAILURE, "Porter file is not a valid BMP file");
        }

        const uint32_t header_size = header.offset;
        LOG("BMP Header size: %u\n", header_size);

        char *extension = NULL;
        Stego *stego = args.stego.retrieve(porter, header_size, args.ssl || args.no_extension ? NULL : &extension);
        fclose(porter);

        if (stego == NULL)
        {
            free(extension);
            errx(EXIT_FAILURE, "Failed to retrieve stego data");
        }

        if (args.ssl)
        {
            uint8_t *plaintext = NULL;
            const size_t extracted = args.algorithm.decrypt(stego->data, stego->size, args.password, args.mode.val, &plaintext);
            free(stego);

            if (!plaintext)
            {
                err(EXIT_FAILURE, "Failed to decrypt");
            }

            stego = (Stego *)plaintext;
            stego->size = ntohl(stego->size);
            LOG("Decrypted data size: %u bytes\n", stego->size);

            if (stego->size > extracted - sizeof(uint32_t))
            {
                free(stego);
                errx(EXIT_FAILURE, "Decrypted data size is larger than extracted data");
            }

            extension = args.no_extension ? NULL : strdup((char *)stego->data + stego->size);

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
            err(EXIT_FAILURE, "Failed to write to output file");
        }

        printf("Data extracted successfully to '%s'.\n", path);
        free(path);
    }

    return EXIT_SUCCESS;
}
