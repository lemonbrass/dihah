#ifndef UTILS_H
#define UTILS_H
#include <da_arena.h>

size_t hash_str(void *str);
char* read_file(arena* ar, const char* path);
char* get_dir(arena* ar, const char* filename);

#endif
