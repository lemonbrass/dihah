#ifndef DA_ARENA_H
#define DA_ARENA_H

#include <da_arr.h>
#include <stdalign.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define ARENA_RELIABLE_MARK (1 << 0)
#define ARENA_ZERO_OUT (1 << 1)
#define ALIGN_UP(x, a) (((x) + ((a) - 1)) & ~((a) - 1))
#define DEFAULT_CAPACITY 1024
#define MIN_FREE_SIZE 8

/* ======================= memory
           ^pos          ^size
   =======================
*/

typedef struct {
  size_t pos;
  void* mem;
} chunk;

typedef struct {
  size_t pos;
  size_t chunkid;
} arena_mark_t;

typedef struct {
  chunk* chunks;
  size_t size;
  size_t minempty;
  uint8_t flags;
} arena;

void arena_mark_reset(arena_mark_t* m, arena* ar);
arena arena_new(size_t size, uint8_t flags);
chunk chunk_new(size_t size, uint8_t flags);
void* arena_alloc(arena* ar, size_t size);
void arena_free(arena* ar);
void arena_reset(arena* ar);
arena_mark_t arena_mark(arena* ar);

#ifdef DA_ARENA_IMPLEMENTATION

void arena_mark_reset(arena_mark_t* m, arena* ar) {
  for (size_t i = m->chunkid + 1; i < darr_len(ar->chunks); i++) {
    darr_get(ar->chunks, i)->pos = 0;
  }
  ar->minempty = m->chunkid;
  darr_get(ar->chunks, m->chunkid)->pos = m->pos;
}

chunk chunk_new(size_t size, uint8_t flags) {
  chunk ch;
  ch.pos = 0;
  ch.mem = malloc(size);
  if (flags & ARENA_ZERO_OUT) {
    memset(ch.mem, 0, size);
  }
  return ch;
}

arena arena_new(size_t size, uint8_t flags) {
  arena ar = {0};
  ar.chunks = NULL;
  ar.flags = flags;
  ar.size = size;
  ar.minempty = 0;
  darr_push(ar.chunks, chunk_new(size, flags));
  return ar;
}

arena_mark_t arena_mark(arena* ar) {
  arena_mark_t m;
  chunk* ch = darr_get(ar->chunks, darr_len(ar->chunks)-1);
  m.pos = ch->pos;
  m.chunkid = darr_len(ar->chunks) - 1;
  return m;
}

chunk* get_arena_chunk(arena* ar, size_t size) {
  if (!(ar->flags & ARENA_RELIABLE_MARK)) {
    size_t free_space = 0;
    size_t id = ar->minempty;
    chunk* ch;
    while (free_space < size) {
      ch = darr_get(ar->chunks, id);
      if (!ch) {
        darr_push(ar->chunks, chunk_new(ar->size, ar->flags));
        continue;
      }
      free_space = ar->size - ch->pos;
      if (free_space < MIN_FREE_SIZE) {
        ar->minempty = id + 1;
      }
      id++;
    }
    return ch;
  }
  else {
    chunk* ch = darr_get(ar->chunks, darr_len(ar->chunks)-1);
    if (ar->size - ch->pos >= size) {
      return ch;
    } else {
      darr_push(ar->chunks, chunk_new(ar->size, ar->flags));
      return darr_get(ar->chunks, darr_len(ar->chunks)-1);
    }
  }
}

void* arena_alloc(arena* ar, size_t size) {
  size = ALIGN_UP(size, alignof(max_align_t));
  assert(size <= ar->size);
  chunk* ch = get_arena_chunk(ar, size);
  void* result = (char*)ch->mem + ch->pos;
  ch->pos += size;
  return result;
}


void chunk_free(chunk* ch) {
  free(ch->mem);
  ch->mem = NULL;
  ch->pos = 0;
}

void arena_free(arena* ar) {
  for (size_t i = 0; i < darr_len(ar->chunks); i++) {
    chunk_free(darr_get(ar->chunks, i));
  }
  darr_free(ar->chunks);
}

void arena_reset(arena* ar) {
  for (size_t i = 0; i < darr_len(ar->chunks); i++) {
    chunk* ch = darr_get(ar->chunks, i);
    ch->pos = 0;
  }
  ar->minempty = 0;
}


#endif

#endif
