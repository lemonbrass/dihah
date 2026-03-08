#define DA_ARR_IMPLEMENTATION
#define DA_ARENA_IMPLEMENTATION
#include <preprocessor.h>
#include <da_arena.h>
#include <lexer.h>
#include <source_file.h>

int main() {
  arena ar = arena_new(1024*64, 0);

  char* src = read_file("test/src1.dihh");
  lexer l = new_lexer(src, &ar);

  for (token t = next_tok(&l); t.type != TT_EOF; t = next_tok(&l)) {
    print_token(&t);
    printf(" ");
    if (t.type == TT_ERROR) break;
  }

  arena_free(&ar);
  free(src);
  return 0;
}
