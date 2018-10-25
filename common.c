#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include "common.h"
#include "gl_common.h"

#ifdef DEBUG
int log_verbose_enabled = 0;
#endif

extern void errno_exit(const char *s)
{
    fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
    exit(EXIT_FAILURE);
}

extern char* read_file(const char* filename)
{
    FILE *file = fopen(filename, "rb");

    if (!file) {
        fprintf(stderr, "Cannot open '%s': %d, %s\n",
             filename, errno, strerror(errno));
        return 0;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);

    fseek(file, 0, SEEK_SET);
    char* buffer = malloc(length + 1);

    long totalBytesRead = 0;
    long bytesRead = 0;

    while ((bytesRead = fread(&buffer[totalBytesRead], 1, length, file)) > 0)
        totalBytesRead += bytesRead;

    if (bytesRead < 0) {
        fprintf(stderr, "Error while reading '%s': %d, %s\n",
             filename, errno, strerror(errno));
        free(buffer);
        return 0;
    }

    assert(totalBytesRead == length);
    if (fclose(file) != 0)
        errno_exit("fclose");

    buffer[length] = 0;

    return buffer;
}