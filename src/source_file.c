#include <da_arena.h>
#include <source_file.h>
#include <da_arr.h>
#include <lexer.h>
#include <preprocessor.h>
#include <string.h>

void new_sf(source_file* sf, char* filename) {
  memset(sf, 0, sizeof(*sf));
  sf->_g_next_uid = 1;
  darr_new(sf->symbols, 8*sizeof(void*), CAPACITY_BASED_BOUNDS);
  sf->ar = arena_new(1024*64, 0);
  sf->filename = arena_alloc(&sf->ar, strlen(filename) + 1);
  strcpy(sf->filename, filename);
  sf->source = read_file(&sf->ar, sf->filename);
  sf->l = new_lexer(sf->source, &sf->ar);
  sf->pp = pp_new(&sf->ar, &sf->l);
}

void free_sf(source_file* sf) {
  pp_free(&sf->pp);
  darr_free(sf->symbols);
  arena_free(&sf->ar);
}
