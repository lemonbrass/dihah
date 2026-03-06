#define DA_ARR_IMPLEMENTATION
#define DA_ARENA_IMPLEMENTATION
#include <lexer.h>
#include <stdio.h>

int main() {
  lexer l = new_lexer(read_file("test2.cc"));
  token t = next_tok(&l);
  while (t.type != TT_EOF && l.id < l.length) {
    print_token(&t);
    printf("\n");
    dump_lexer_state(&l);
    t = next_tok(&l);
  }
  free_lexer(&l);
  return 0;
}
