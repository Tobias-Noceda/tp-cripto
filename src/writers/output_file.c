#include <writers.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char *set_output(const char *path, const char *extension, const uint8_t *data, size_t size)
{
    char *filename;
    if (extension != NULL)
    {
        size_t len = strlen(path) + strlen(extension) + 1;
        filename = malloc(len);

        if (filename == NULL)
        {
            perror("Memory allocation failed");
            return NULL;
        }

        snprintf(filename, len, "%s%s", path, extension);
    }
    else
    {
        filename = strdup(path);
        if (filename == NULL)
        {
            perror("String duplication failed");
            return NULL;
        }
    }

    FILE *output_file = fopen(filename, "wb");
    if (output_file == NULL)
    {
        perror("Failed to open output file");
        free(filename);
        return NULL;
    }

    size_t written = fwrite(data, 1, size, output_file);
    fclose(output_file);

    if (written != size)
    {
        perror("Failed to write all data to output file");
        free(filename);
        return NULL;
    }

    return filename;
}
