#include "thirdparty/kvec.h"
#include "utils.h"
#include <da_arena.h>
#include <source_file.h>
#include <lexer.h>
#include <preprocessor.h>
#include <string.h>

source_file* new_sf(arena* ar, char* filename) {
  source_file* sf = arena_alloc(ar, sizeof(source_file));
  memset(sf, 0, sizeof(*sf));
  kv_init(sf->search_paths);
  kv_push(char*, sf->search_paths, get_dir(ar, filename));
  sf->_g_next_uid = 1;
  sf->ar = ar;
  sf->filename = arena_alloc(ar, strlen(filename) + 1);
  strcpy(sf->filename, filename);
  sf->source = read_file(ar, sf->filename);
  sf->l = new_lexer(sf);
  sf->pp = pp_new(sf);
  return sf;
}

void free_sf(source_file* sf) {
  pp_free(&sf->pp);
  kv_destroy(sf->symbols);
  kv_destroy(sf->search_paths);
  kv_destroy(sf->sys_search_paths);
}
