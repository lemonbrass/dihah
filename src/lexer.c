#include <token.h>
#include <assert.h>
#include <da_arena.h>
#include <ctype.h>
#include <lexer.h>
#include <da_arr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <source_file.h>

char curr_char(lexer* l) {
  return l->sf->source[l->id];
}

lexer new_scratch_lexer() {
  arena* ar = arena_new(1024*16, 0);
  source_file* sf = arena_alloc(ar, sizeof(source_file));
  lexer l = {0};
  l.sf = sf;
  return l;
}

void free_scratch_lexer(lexer* l) {
  arena_free(l->sf->ar);
}

int advance(lexer* l) {
  if (l->id + 1 >= l->length) return -1;
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
  char* id = NULL;
  while (isalnum(ch) || ch == '_') {
    darr_push(id, ch);
    ch = VALIDATE_CHAR(advance(l));
  }
  darr_push(id, '\0');
  char* id_cpy = arena_alloc(l->sf->ar, darr_len(id));
  memcpy(id_cpy, id, darr_len(id));
  darr_free(id);
  
  #define X(tt, k) \
  if (strcmp(id_cpy, k) == 0) {\
    token t;\
    t.type = tt; \
    t.content.str = id_cpy; \
    return t;\
  }

  KEYWORDS(X)

  #undef X

  return new_token_ident(l, id_cpy);
}

token lex_str(lexer* l) {
  char* str = NULL;
  char ch = VALIDATE_CHAR(advance(l));
  while (true) {
    if (ch == '\\') {
      switch (peek(l)) {
        
        #define X(code, val)\
           case code: darr_push(str, val); VALIDATE_CHAR(advance(l)); break;
          ESCAPES(X)
        #undef X

        default:
          printf("INVALID STRING ESCAPE\n");
          dump_lexer_state(l);
          darr_free(str);
          exit(1);
      }
      ch = VALIDATE_CHAR(advance(l));
      continue;
    }
    if (ch == '\"') {
      VALIDATE_CHAR(advance(l));
      break;
    }
    darr_push(str, ch);
    ch = VALIDATE_CHAR(advance(l));
  }
  darr_push(str, '\0');
  char* str_cpy = arena_alloc(l->sf->ar, strlen(str)+1);
  strcpy(str_cpy, str);
  darr_free(str);
  return new_token_string(l, str_cpy);
}

token lex_num(lexer* l) {
  char ch = curr_char(l);
  size_t num = 0;
  while (isdigit(ch)) {
    num = num * 10 + (ch - '0');
    ch = VALIDATE_CHAR(advance(l));
  }
  return new_token_num(l, num);
}

token next_tok(lexer* l) {
  char ch = curr_char(l);
  while (isspace((unsigned char)ch)) {
    ch = VALIDATE_CHAR(advance(l));
  }

  if (ch == '\0') {
    token t;
    t.type = TT_EOF;
    return t;
  }

  // DONOT VALIDATE_CHAR(PEEK) HERE, ITS USELSS + IT WILL ERROR OUT IF ITS EOF
  if (ch == '/' && peek(l) == '/') {
    VALIDATE_CHAR(advance(l)); //skip first /
    VALIDATE_CHAR(advance(l)); //skip second /
    while (ch != '\0' && ch != '\n') {
      ch = VALIDATE_CHAR(advance(l));
    }
    return next_tok(l);
  }

  if (ch == '/' && peek(l) == '*') {
    VALIDATE_CHAR(advance(l)); //skip /
    VALIDATE_CHAR(advance(l)); //skip *
    while (ch != '\0' && (ch != '*' || VALIDATE_CHAR(peek(l)) != '/')) {
      ch = VALIDATE_CHAR(advance(l));
    }
    if (ch == '\0') {
      printf("Unterminated /**/ comment :D\n");
      dump_lexer_state(l);
      exit(1);
    }
    VALIDATE_CHAR(advance(l)); //skip *
    VALIDATE_CHAR(advance(l)); //skip /
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
    ch = VALIDATE_CHAR(advance(l));
    while (isspace(ch) && ch != '\n') ch = VALIDATE_CHAR(advance(l));
    if (ch != '\n') return ERROR_TOKEN("Invalid character after \\");
    VALIDATE_CHAR(advance(l));
    return next_tok(l);
  }

  #define X(tt, v) if (lstrmatch(l, v)) {\
    token t; \
    t.type = tt; \
    t.content.str = v; \
    VALIDATE_CHAR(advance(l));\
    return t;\
  }
  OPERATORS(X)
  #undef X
  
  VALIDATE_CHAR(advance(l));
  switch (ch) {
    #define X(tok_type, c) case c: return new_token_simple(l, tok_type);
      SEPERATORS(X)
    #undef X
    case '#': return new_token_simple(l, TT_PREPROCESS);
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

bool eat_str(lexer* l, const char* str) {
  lexer l_new = new_scratch_lexer();
  l_new.sf->source = str;
  token t = next_tok(l);
  token t_0 = next_tok(&l_new);
  bool result = true;
  while (l_new.length > l_new.id && t_0.type != TT_EOF) {
    if (!tok_cmp(t, t_0)) {
      result = false;
      break;
    }
    t = next_tok(l);
    t_0 = next_tok(&l_new);
  }
  return result;
}

bool try_eat_str(lexer* l, const char* str) {
  lexer l_new = new_scratch_lexer();
  lexer l_mark = *l;
  token t = next_tok(l);
  token t_0 = next_tok(&l_new);
  bool result = true;
  l_new.sf->source = str;
  while (l_new.length > l_new.id && t_0.type != TT_EOF) {
    if (!tok_cmp(t, t_0)) {
      result = false;
      goto cleanup;
    }
    t = next_tok(l);
    t_0 = next_tok(&l_new);
  }

  cleanup:
    if (!result) *l = l_mark;
    return result;
  
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
