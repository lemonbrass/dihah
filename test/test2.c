#define DA_ARR_IMPLEMENTATION
#define DA_ARENA_IMPLEMENTATION
#include <da_arena.h>
#include <lexer.h>
#include <stdio.h>

int main() {
  arena ar = arena_new(1024*1024, 0);
  lexer l = new_lexer(read_file("test/test2.dihh"), &ar);
  token t = next_tok(&l);
  while (t.type != TT_EOF && l.id < l.length) {
    print_token(&t);
    printf("\n");
    dump_lexer_state(&l);
    t = next_tok(&l);
  }
  free_lexer(&l);
  arena_free(&ar);
  return 0;
}
