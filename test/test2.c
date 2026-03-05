#define DA_ARR_IMPLEMENTATION
#define DA_ARENA_IMPLEMENTATION
#include "lexer.h"
#include <stdio.h>

int main() {
  lexer l = new_lexer(read_file("test2.cc"));
  token t = next_tok(&l);
  while (t.type != TT_EOF && l.id != l.length) {
    if (t.type == TT_NUM) {
      printf("TOK_NUM(%ld)\n", t.content.num);
    }
    if (t.type == TT_ID) {
      printf("TOK_ID(%s)\n", t.content.str);
    }
    if (t.type == TT_OP) {
      printf("TOK_OP(%c)\n", t.content.op);
    }
    dump_lexer_state(&l);
    t = next_tok(&l);
  }
  return 0;
}
