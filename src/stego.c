#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <readers.h>
#include <writers.h>

#define BMP_HEADER_SIZE 128

StegoMethod stego_methods[] = {
    {"LSB1", embed_data_lsb1, retrieve_lsb1},
    {"LSB4", embed_data_lsb4, retrieve_lsb4}
};

static const StegoMethod *get_stego_method(const char *name);

int main(int argc, char *argv[])
{
    const char *input_file_name = NULL;
    const char *porter_file_name = NULL;
    const char *output_file_name = NULL;
    const StegoMethod *stego_type = NULL;
    bool extract_mode = false;

    if (argc < 9 || strcmp(argv[1], "-embed") != 0)
    {
        if (argc < 7 || strcmp(argv[1], "-extract") != 0)
        {
            fprintf(stderr, "Usage: %s -embed -in <input_file> -p <porter_file> -out <output_file> -stego <stego_type>\n", argv[0]);
            return EXIT_FAILURE;
        }
        extract_mode = true;
    }

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-in") == 0 && i + 1 < argc)
        {
            input_file_name = argv[++i];
        }
        else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc)
        {
            porter_file_name = argv[++i];
        }
        else if (strcmp(argv[i], "-out") == 0 && i + 1 < argc)
        {
            output_file_name = argv[++i];
        }
        else if (strcmp(argv[i], "-stego") == 0 && i + 1 < argc)
        {
            stego_type = get_stego_method(argv[++i]);
        }
    }

    if (porter_file_name == NULL || output_file_name == NULL || (!extract_mode && input_file_name == NULL) || stego_type == NULL)
    {
        fprintf(stderr, "Input file, porter file, output file, and stego type must be specified.\n");
        return EXIT_FAILURE;
    }

    if (!extract_mode)
    {
        printf("Embedding mode:\n");
        printf("Input file: %s\n", input_file_name);
        printf("Porter file: %s\n", porter_file_name);
        printf("Output file: %s\n", output_file_name);
        printf("Stego type: %s\n", stego_type->name);

        FILE *porter;
        long porter_size = get_output(&porter, output_file_name, porter_file_name);
        if (porter_size == 0)
        {
            perror("Failed to open porter file");
            return EXIT_FAILURE;
        }

        Data input_data = get_message(input_file_name);
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

        fseek(porter, BMP_HEADER_SIZE, SEEK_SET); // Skip BMP header

        // turn the size into 4 bytes
        uint8_t size_bytes[4];
        size_bytes[0] = (input_data.size >> 24) & 0xFF;
        size_bytes[1] = (input_data.size >> 16) & 0xFF;
        size_bytes[2] = (input_data.size >> 8) & 0xFF;
        size_bytes[3] = input_data.size & 0xFF;

        if (stego_methods->embed(porter, size_bytes, 4) == 0)
        {
            fprintf(stderr, "Failed to embed size data.\n");
            free(input_data.data);
            free(input_data.ext);
            fclose(porter);
            return EXIT_FAILURE;
        }
        if (stego_methods->embed(porter, (uint8_t *)input_data.data, input_data.size) == 0)
        {
            fprintf(stderr, "Failed to embed input data.\n");
            free(input_data.data);
            free(input_data.ext);
            fclose(porter);
            return EXIT_FAILURE;
        }
        if (stego_methods->embed(porter, (uint8_t *)input_data.ext, strlen(input_data.ext) + 1) == 0)
        {
            fprintf(stderr, "Failed to embed file extension data.\n");
            free(input_data.data);
            free(input_data.ext);
            fclose(porter);
            return EXIT_FAILURE;
        }

        printf("Data embedded successfully into '%s'.\n", output_file_name);

        // free memory and close files
        free(input_data.data);
        free(input_data.ext);
        fclose(porter);
    }
    else
    {
        printf("Extracting mode:\n");
        printf("Porter file: %s\n", porter_file_name);
        printf("Output file: %s\n", output_file_name);

        char *extension = NULL;
        Stego *stego = stego_methods->retrieve(porter_file_name, BMP_HEADER_SIZE, &extension);
        if (stego == NULL)
        {
            fprintf(stderr, "Failed to retrieve stego data.\n");
            free(extension);
            return EXIT_FAILURE;
        }

        char *full_output_file_name;
        if (extension != NULL)
        {
            full_output_file_name = malloc(strlen(output_file_name) + strlen(extension) + 2);
            if (full_output_file_name == NULL)
            {
                perror("Memory allocation failed");
                free(stego);
                free(extension);
                return EXIT_FAILURE;
            }
            sprintf(full_output_file_name, "%s%s", output_file_name, extension);
        }
        else
        {
            full_output_file_name = strdup(output_file_name);
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

static const StegoMethod *get_stego_method(const char *name)
{
    size_t methods_count = sizeof(stego_methods) / sizeof(StegoMethod);
    for (size_t i = 0; i < methods_count; i++)
    {
        if (strcmp(stego_methods[i].name, name) == 0)
        {
            return &stego_methods[i];
        }
    }
    return NULL;
}