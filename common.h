#ifndef COMMON_H
#define COMMON_H

#define CLEAR(x) memset(&(x), 0, sizeof(x))

extern void errno_exit(const char *s);
extern char* read_file(const char* filename);

#endif