#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <readers.h>
#include <writers.h>

#define BMP_HEADER_SIZE 128

int main(int argc, char *argv[]) {

    const char *input_file_name = NULL;
    const char *porter_file_name = NULL;
    const char *output_file_name = NULL;

    if (argc < 7 || strcmp(argv[1], "-embed") != 0) {
        fprintf(stderr, "Usage: %s -embed -in <input_file> -p <porter_file> -out <output_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-in") == 0 && i + 1 < argc) {
            input_file_name = argv[++i];
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            porter_file_name = argv[++i];
        } else if (strcmp(argv[i], "-out") == 0 && i + 1 < argc) {
            output_file_name = argv[++i];
        }
    }

    if (input_file_name == NULL || porter_file_name == NULL || output_file_name == NULL) {
        fprintf(stderr, "Input file, porter file, and output file must be specified.\n");
        return EXIT_FAILURE;
    }

    FILE *porter;
    long porter_size = get_output(&porter, output_file_name, porter_file_name);
    if (porter_size == 0) {
        perror("Failed to open porter file");
        return EXIT_FAILURE;
    }

    Data input_data = get_message(input_file_name);
    if (input_data.data == NULL) {
        fclose(porter);
        return EXIT_FAILURE;
    }
    if (input_data.ext == NULL) {
        perror("File extension couldn't be determined");
        free(input_data.data);
        fclose(porter);
        return EXIT_FAILURE;
    }
    if (input_data.size == 0) {
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

    if (embed_data_lsb1(porter, size_bytes, 4) == 0) {
        fprintf(stderr, "Failed to embed size data.\n");
        free(input_data.data);
        free(input_data.ext);
        fclose(porter);
        return EXIT_FAILURE;
    }
    if (embed_data_lsb1(porter, input_data.data, input_data.size) == 0) {
        fprintf(stderr, "Failed to embed input data.\n");
        free(input_data.data);
        free(input_data.ext);
        fclose(porter);
        return EXIT_FAILURE;
    }
    if (embed_data_lsb1(porter, input_data.ext, strlen(input_data.ext) + 1) == 0) {
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

    return EXIT_SUCCESS;
}
