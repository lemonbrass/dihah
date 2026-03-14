#define DA_ARENA_IMPLEMENTATION
#include <da_string.h>
#include "preprocessor.h"
#include <da_arena.h>
#include <lexer.h>
#include <source_file.h>

int main() {
  arena* ar = arena_new(1024*16, 0);
  source_file* sf = new_sf(ar, "test/src1.dihc");

  for (token t = pp_next_tok(&sf->pp); t.type != TT_EOF; t = pp_next_tok(&sf->pp)) {
    print_token(&t);
    printf("\n");
    if (t.type == TT_ERROR) break;
  }
  printf("\n");
  
  free_sf(sf);
  arena_free(ar);
  return 0;
}
