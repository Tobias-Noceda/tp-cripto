#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/sendfile.h>

size_t get_output(FILE **file, const char *output, const char *input)
{
    int ifd = open(input, O_RDONLY);
    if (ifd < 0)
    {
        perror("Error opening source file");
        return 0;
    }

    int ofd = open(output, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (ofd < 0)
    {
        perror("Error opening destination file");
        close(ifd);
        return 0;
    }

    struct stat stats;
    if (fstat(ifd, &stats) < 0)
    {
        perror("Error getting file stats");
        close(ifd);
        close(ofd);
        return 0;
    }

    if (sendfile(ofd, ifd, NULL, stats.st_size) < stats.st_size)
    {
        perror("Error during sendfile");
        close(ifd);
        close(ofd);
        return 0;
    }

    close(ifd);

    *file = fdopen(ofd, "r+b");
    if (*file == NULL)
    {
        perror("Error converting file descriptor to FILE*");
        close(ofd);
        return 0;
    }

    return stats.st_size;
}
