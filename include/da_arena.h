#ifndef DA_ARENA_H
#define DA_ARENA_H

#include <stdalign.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define DEFAULT_CAPACITY 1024

/* ======================= memory
           ^pos          ^size
   =======================
*/

typedef struct {
  void* mem;
  size_t size;
  size_t pos;
  bool zero;
} arena;

arena arena_new(size_t size, bool zero);
void* arena_alloc(arena* ar, size_t size);
void arena_free(arena* ar);
void arena_reset(arena* ar);;


#ifdef DA_ARENA_IMPLEMENTATION


arena arena_new(size_t size, bool zero) {
  arena ar;
  ar.size = size;
  ar.pos = 0;
  ar.mem = malloc(size);
  ar.zero = zero;
  memset(ar.mem, 0, ar.size);
  return ar;
}

void* arena_alloc(arena* ar, size_t size) {
  size_t align = alignof(max_align_t);
  uintptr_t base = (uintptr_t)ar->mem + ar->pos;
  size_t offset = (align - (base % align)) % align;
  size_t new_pos = ar->pos + offset + size;
  if (new_pos > ar->size) {
    size_t old_size = ar->size;
    size_t new_size = old_size;
    while (new_pos > new_size)
      new_size *= 2;
    void* new_mem = realloc(ar->mem, new_size);
    if (!new_mem) return NULL;
    ar->mem = new_mem;
    ar->size = new_size;
    if (ar->zero) {
      memset((char*)ar->mem + old_size, 0, new_size - old_size);
    }
  }

  ar->pos += offset;
  void* result = (char*)ar->mem + ar->pos;
  ar->pos += size;

  return result;
}

void arena_free(arena* ar) {
  free(ar->mem);
  ar->mem = NULL;
  ar->size = 0;
  ar->pos = 0;
}

void arena_reset(arena* ar) {
  ar->pos = 0;
}


#endif

#endif
