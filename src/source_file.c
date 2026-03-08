#include <da_arena.h>
#include <da_arr.h>
#include <lexer.h>
#include <preprocessor.h>
#include <source_file.h>
#include <string.h>

void new_sf(source_file* sf, char* filename) {
  memset(sf, 0, sizeof(*sf));
  sf->_g_next_uid = 1;
  sf->filename = strdup(filename);
  sf->ar = arena_new(1024*64, 0);
  sf->source = read_file(sf->filename);
  sf->l = new_lexer(sf->source, &sf->ar);
  sf->pp = pp_new(&sf->ar, &sf->l);
}

void free_sf(source_file* sf) {
  pp_free(&sf->pp);
  darr_free(sf->symbols);
  free(sf->filename);
  free(sf->source);
  arena_free(&sf->ar);
}
