#define DA_ARR_IMPLEMENTATION
#define DA_ARENA_IMPLEMENTATION
#include "preprocessor.h"
#include <da_arena.h>
#include <lexer.h>
#include <source_file.h>

int main() {
  source_file sf;
  new_sf(&sf, "test/src1.dihh");

  for (token t = pp_next_tok(&sf.pp); t.type != TT_EOF; t = pp_next_tok(&sf.pp)) {
    print_token_str(&t);
    printf(" ");
    if (t.type == TT_ERROR) break;
  }
  printf("\n");
  
  free_sf(&sf);
  return 0;
}
