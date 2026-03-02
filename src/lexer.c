#include <ctype.h>
#include <lexer.h>
#include <da_arr.h>

char curr_char(lexer* l) {
  return l->source[l->id];
}

char advance(lexer* l) {
  l->id++;
  char next = curr_char(l);
  if (next == '\n') {
    l->pos.line++;
    l->pos.ch = 1;
  }
  return next;
}
  
lexer new_lexer(char* source, size_t len) {
  lexer l;
  l.source = source;
  l.id = 0;
  l.length = len;
  l.pos.ch = 1;
  l.pos.line = 1;

  return l;
}

token lex_id(lexer* l) {
  char ch = curr_char(l);
  char* id = NULL;
  while (isalnum(ch)) {
    darr_push(id, ch);
    ch = advance(l);
  }
  token t;
  t.content.str = id;
  t.type = ID;

  return t;
}



token next_tok(lexer* l) {
  char ch = curr_char(l);
  while (isspace((unsigned char)ch)) {
    ch = advance(l);
  }

  if (isalpha(ch) || ch == '_') {
    return lex_id(l);
  }
}
void free_lexer();
