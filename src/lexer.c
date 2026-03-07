#include <da_arena.h>
#include <ctype.h>
#include <lexer.h>
#include <da_arr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char curr_char(lexer* l) {
  return l->source[l->id];
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
  
lexer new_lexer(const char* source) {
  lexer l;
  l.source = source;
  l.id = 0;
  l.length = strlen(source) + 1;
  l.pos.ch = 1;
  l.pos.line = 1;
  l.ar = arena_new(1024*1024, false);

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
  char* id_cpy = arena_alloc(&l->ar, strlen(id)+1);
  strcpy(id_cpy, id);
  darr_free(id);
  
  #define X(tt, k) \
  if (strcmp(id_cpy, #k) == 0) {\
    token t;\
    t.type = tt; \
    t.content.str = id_cpy; \
    return t;\
  }

  KEYWORDS(X)

  #undef X
  
  token t;
  t.content.str = id_cpy;
  t.type = TT_ID;
  return t;
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
          free_lexer(l);
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
  char* str_cpy = arena_alloc(&l->ar, strlen(str)+1);
  strcpy(str_cpy, str);
  darr_free(str);
  token t;
  t.content.str = str_cpy;
  t.type = TT_STRING;
  return t;
}

token lex_num(lexer* l) {
  char ch = curr_char(l);
  signed long num = 0;
  while (isdigit(ch)) {
    num = num * 10 + (ch - '0');
    ch = VALIDATE_CHAR(advance(l));
  }
  token t;
  t.content.num = num;
  t.type = TT_NUM;
  return t;
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
    while (ch != '\0' && (ch != '*' && VALIDATE_CHAR(peek(l)) != '/')) {
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
    #define X(tok_type, c) case c: {token t; t.type = tok_type; return t;}
      SEPERATORS(X)
    #undef X
    case '#': {
      token t;
      t.type = TT_PREPROCESS;
      return t;
    }
    default: {
      // printf("Invalid character: %c\n", ch);
      dump_lexer_state(l);
      exit(1);
    }
  }
}

bool lstrmatch(lexer* l, const char* str) {
  size_t len = strlen(str);
  bool res = strncmp(l->source + l->id, str, len) == 0;
  if (res) l->id+=len-1;
  return res;
}

void free_lexer(lexer* l) {
  arena_free(&l->ar);
}

int peek(lexer* l) {
  if (l->id + 1 >= l->length) return -1;
  return l->source[l->id+1];
} 

void dump_lexer_state(lexer* l) {
  printf("lexer l @ %p, id = %zu, length = %zu\n", l, l->id, l->length);
  printf("Cursor @ line %zu column %zu: ", l->pos.line, l->pos.ch);
  size_t id = l->id - (l->pos.ch - 1);
  char ch = l->source[id];
  if (ch == '\n')  {
    id += 1;
    ch = l->source[id];
  }
  while (ch != '\n' && ch != '\0' && id < l->length) {
    printf("%c", ch);
    id++;
    ch = l->source[id];
  }
  printf("\n");
}

void print_token(token* t) {
  switch (t->type) {
    case TT_PREPROCESS: printf("TT_PREPROCESS(#)"); break;
    case TT_ID: printf("TT_ID(%s)", t->content.str); break;
    case TT_NUM: printf("TT_NUM(%ld)", t->content.num); break;
    case TT_EOF: printf("TT_EOF"); break;
    case TT_STRING: {
      printf("TT_STRING(");
      for (size_t i = 0; i < strlen(t->content.str); i++) {
        char ch = t->content.str[i];
        switch (ch) {
          #define X(c, escapecode) case escapecode: printf("\\%c", c); break;
            ESCAPES(X)
            default: printf("%c", ch);
          #undef X
        }
      }
      printf(")");
      break;
    }
    #define X(tt, v) case tt: printf("%s(%c)", #tt, v); break;
      SEPERATORS(X)
    #undef X
    #define X(tt, v) case tt: printf("%s(%s)", #tt, v); break;
      KEYWORDS(X)
      OPERATORS(X)
    #undef X
  }
}

char* read_file(const char *path) {
  FILE *f = fopen(path, "rb");

  fseek(f, 0, SEEK_END);
  size_t size = (size_t)ftell(f);
  rewind(f);

  char *buf = malloc(size + 1);
  fread(buf, 1, size, f);
  buf[size] = '\0';

  fclose(f);
  return buf;
}
