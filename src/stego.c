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

        Data input_data = get_message(args.input_path);
        if (input_data.data == NULL)
        {
            fclose(porter);
            return EXIT_FAILURE;
        }
        if (input_data.ext == NULL)
        {
            perror("File extension couldn't be determined");
            free(input_data.data);
            fclose(porter);
            return EXIT_FAILURE;
        }
        if (input_data.size == 0)
        {
            perror("Input file cannot be empty.");
            free(input_data.data);
            free(input_data.ext);
            fclose(porter);
            return EXIT_FAILURE;
        }

        const BITMAPFILEHEADER header = get_bmp_file_header(porter);
        if (header.signature != 0x4D42)
        {
            fprintf(stderr, "Porter file is not a valid BMP file.\n");
            free(input_data.data);
            free(input_data.ext);
            fclose(porter);
            return EXIT_FAILURE;
        }

        uint32_t header_size = header.offset;
        fseek(porter, header_size, SEEK_SET); // Skip BMP header

        // turn the size into 4 bytes
        uint8_t size_bytes[4];
        size_bytes[0] = (input_data.size >> 24) & 0xFF;
        size_bytes[1] = (input_data.size >> 16) & 0xFF;
        size_bytes[2] = (input_data.size >> 8) & 0xFF;
        size_bytes[3] = input_data.size & 0xFF;

        if (args.stego.embed(porter, size_bytes, 4) == 0)
        {
            fprintf(stderr, "Failed to embed size data.\n");
            free(input_data.data);
            free(input_data.ext);
            fclose(porter);
            return EXIT_FAILURE;
        }
        if (args.stego.embed(porter, (uint8_t *)input_data.data, input_data.size) == 0)
        {
            fprintf(stderr, "Failed to embed input data.\n");
            free(input_data.data);
            free(input_data.ext);
            fclose(porter);
            return EXIT_FAILURE;
        }
        if (args.stego.embed(porter, (uint8_t *)input_data.ext, strlen(input_data.ext) + 1) == 0)
        {
            fprintf(stderr, "Failed to embed file extension data.\n");
            free(input_data.data);
            free(input_data.ext);
            fclose(porter);
            return EXIT_FAILURE;
        }

        printf("Data embedded successfully into '%s'.\n", args.output_path);

        // free memory and close files
        free(input_data.data);
        free(input_data.ext);
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
        Stego *stego = args.stego.retrieve(porter, header_size, &extension);
        fclose(porter);

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
        if (extension != NULL)
        {
            printf("Extracted file extension: %s\n", extension);
        }
        free(stego);
        free(extension);
        free(full_output_file_name);
        fclose(output_file);
    }

    return EXIT_SUCCESS;
}
