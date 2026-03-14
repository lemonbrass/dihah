#include <stddef.h>
#include <stdio.h>
#include <utils.h>
#include <da_arena.h>

size_t hash_str(void* str) {
  size_t hash = 5381;
  unsigned char c;

  while ((c = *(char*)str++))
    hash = ((hash << 5) + hash) + c; // hash * 33 + c

  return hash;
}

char* get_dir(arena* ar, const char* path) {
  const char* last = strrchr(path, '/');
  if (!last) return ".";

  size_t len = (size_t)(last - path);

  char* dir = arena_alloc(ar, len + 1);
  memcpy(dir, path, len);
  dir[len] = '\0';

  return dir;
}

char* read_file(arena* ar, const char *path) {
  FILE *f = fopen(path, "rb");
  if (!f) {
    return NULL;
  }

  fseek(f, 0, SEEK_END);
  size_t size = (size_t)ftell(f);
  rewind(f);

  char *buf = arena_alloc(ar, size + 1);
  fread(buf, 1, size, f);
  buf[size] = '\0';
  
  fclose(f);
  return buf;
}

