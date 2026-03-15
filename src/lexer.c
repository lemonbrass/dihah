#include <token.h>
#include <assert.h>
#include <da_string.h>
#include <da_arena.h>
#include <ctype.h>
#include <lexer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <source_file.h>

char curr_char(lexer* l) {
  return l->sf->source[l->id];
}

lexer new_scratch_lexer(const char* str) {
  arena* ar = arena_new(1024*16, 0);
  source_file* sf = arena_alloc(ar, sizeof(source_file));
  lexer l = {0};
  l.sf = sf;
  l.sf->ar = ar;
  l.sf->filename = "scratch";
  l.sf->source = str;
  return l;
}

void free_scratch_lexer(lexer* l) {
  arena_free(l->sf->ar);
}

char advance(lexer* l) {
  if (l->id + 1 >= l->length) return '\0';
  l->id++;
  char next = curr_char(l);
  if (next == '\n') {
    l->pos.line++;
    l->pos.ch = 1;
  }
  else {
    l->pos.ch++;
  }
  return next;
}
  
lexer new_lexer(source_file* sf) {
  lexer l;
  l.id = 0;
  l.length = strlen(sf->source) + 1;
  l.pos.ch = 1;
  l.pos.line = 1;
  l.sf = sf;

  return l;
}

token lex_id(lexer* l) {
  char ch = curr_char(l);
  size_t currpos = l->id;
  size_t size = 0;
  while (isalnum(ch) || ch == '_') {
    size++;
    ch = advance(l);
  }
  string_view id = sv_new(l->sf->source + currpos, size);
  #define X(tt, k) \
  if (memcmp(s_str(id), k, size) == 0) {\
    token t;\
    t.type = tt; \
    t.content.str = id; \
    return t;\
  }

  KEYWORDS(X)

  #undef X

  return new_token_ident(l, id);
}

token lex_str(lexer* l) {
  char ch = advance(l);
  da_string ds = ds_new(l->sf->ar);
  while (true) {
    if (ch == '\\') {
      switch (ch) {
        
        #define X(code, val)\
           case code: ds_push_char(&ds,  val) ; break;
          ESCAPES(X)
        #undef X

        default:
          printf("INVALID STRING ESCAPE\n");
          dump_lexer_state(l);
          exit(1);
      }
      ch = advance(l);
      continue;
    }
    if (ch == '\"') {
      advance(l);
      break;
    }
    ds_push_char(&ds, ch);
    ch = advance(l);
  }
  ds_push_char(&ds, '\0'); // "" => c auto adds a '\0'
  string_view sv = ds_build(&ds);
  return new_token_string(l, sv);
}

token lex_num(lexer* l) {
  char ch = curr_char(l);
  size_t num = 0;
  while (isdigit(ch)) {
    num = num * 10 + (ch - '0');
    ch = advance(l);
  }
  return new_token_num(l, num);
}

token next_tok(lexer* l) {
  char ch = curr_char(l);
  while (isspace((unsigned char)ch)) {
    ch = advance(l);
  }

  if (ch == '\0') {
    token t;
    t.type = TT_EOF;
    return t;
  }

  // DONOT PEEK) HERE ITS USELSS + IT WILL ERROR OUT IF ITS EOF
  if (ch == '/' && peek(l) == '/') {
    advance(l); //skip first /
    advance(l); //skip second /
    while (ch != '\0' && ch != '\n') {
      ch = advance(l);
    }
    return next_tok(l);
  }

  if (ch == '/' && peek(l) == '*') {
    advance(l); //skip /
    advance(l); //skip *
    while (ch != '\0' && (ch != '*' || peek(l) != '/')) {
      ch = advance(l);
    }
    if (ch == '\0') {
      printf("Unterminated /**/ comment :D\n");
      dump_lexer_state(l);
      exit(1);
    }
    advance(l); //skip *
    advance(l); //skip /
    return next_tok(l);
  }

  if (ch == '\"') {
    return lex_str(l);
  }


  if (isalpha(ch) || ch == '_') {
    return lex_id(l);
  }

  if (isdigit(ch)) {
    return lex_num(l);
  }

  if (ch == '\\') {
    ch = advance(l);
    while (isspace(ch) && ch != '\n') ch = advance(l);
    if (ch != '\n') return ERROR_TOKEN("Invalid character after \\");
    advance(l);
    return next_tok(l);
  }

  #define X(tt, v) if (lstrmatch(l, v)) {\
    token t; \
    t.type = tt; \
    t.content.str = sv_new(v, strlen(v)); \
    advance(l);\
    return t;\
  }
  OPERATORS(X)
  #undef X
  
  switch (ch) {
    #define X(tok_type, c) case c: advance(l); return new_token_simple(l, tok_type);
      SEPERATORS(X)
    #undef X
    case '#': advance(l); return new_token_simple(l, TT_PREPROCESS);
    default: return ERROR_TOKEN("INVALID CHARACTER");
  }
}

bool lstrmatch(lexer* l, const char* str) {
  size_t len = strlen(str);
  bool res = strncmp(l->sf->source + l->id, str, len) == 0;
  if (res) l->id+=len-1;
  return res;
}

int peek(lexer* l) {
  if (l->id + 1 >= l->length) return -1;
  return l->sf->source[l->id+1];
} 

void dump_lexer_state(lexer* l) {
  printf("lexer l @ %p, id = %zu, length = %zu\n", l, l->id, l->length);
  printf("Cursor @ line %zu column %zu: ", l->pos.line, l->pos.ch);
  size_t id = l->id - (l->pos.ch - 1);
  char ch = l->sf->source[id];
  if (ch == '\n')  {
    id += 1;
    ch = l->sf->source[id];
  }
  while (ch != '\n' && ch != '\0' && id < l->length) {
    printf("%c", ch);
    id++;
    ch = l->sf->source[id];
  }
  printf("\n");
}

bool eat_tok(lexer* l, const token t) {
  token t1 = next_tok(l);
  return tok_cmp(t1, t);
}
bool try_eat_tok(lexer* l, const token t) {
  lexer l_mark = *l;
  token t1 = next_tok(l);
  bool res = tok_cmp(t, t1);
  if (!res) *l = l_mark;
  return res;
}

token match_consume(lexer* l, const tok_type tt) {
  token t = next_tok(l);
  assert(t.type == tt);
  return t;
}
