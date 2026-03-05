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
  if (l->id >= l->length) return -1;
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
  l.length = strlen(source);
  l.pos.ch = 1;
  l.pos.line = 1;
  l.ar = arena_new(1024, false);

  return l;
}

token lex_id(lexer* l) {
  char ch = curr_char(l);
  char* id = arena_mark(&l->ar);
  while (isalnum(ch) || ch == '_') {
    *(char*)arena_alloc(&l->ar, sizeof(char)) = ch;
    ch = VALIDATE_CHAR(advance(l));
  }
  *(char*)arena_alloc(&l->ar, sizeof(char)) = '\0';
  token t;
  t.content.str = id;
  t.type = TT_ID;
  return t;
}

token lex_num(lexer* l) {
  char ch = curr_char(l);
  signed long num = 0;
  int sign = 1;
  if (ch == '-') {
    sign = -1;
    ch = VALIDATE_CHAR(advance(l));
  }
  while (isdigit(ch)) {
    num = num * 10 + (ch - '0');
    ch = VALIDATE_CHAR(advance(l));
    printf("%ld", num);
  }
  token t;
  t.content.num = num * sign;
  t.type = TT_NUM;
  return t;
}

token next_tok(lexer* l) {
  char ch = curr_char(l);
  while (isspace((unsigned char)ch)) {
    ch = VALIDATE_CHAR(advance(l));
  }

  if (isalpha(ch) || ch == '_') {
    return lex_id(l);
  }

  if (isdigit(ch) || (ch == '-' && isdigit(VALIDATE_CHAR(peek(l))) )) {
    return lex_num(l);
  }

  if (ch == '\0') {
    token t;
    t.type = TT_EOF;
    VALIDATE_CHAR(advance(l));
    return t;
  }

  VALIDATE_CHAR(advance(l));
  switch (ch) {
    case '(': {
        token t;
        t.type = TT_OPAREN;
        return t;
      }
    case ')': {
        token t;
        t.type = TT_CPAREN;
        return t;
      }
    case '{': {
        token t;
        t.type = TT_OCURLY;
        return t;
      }
    case '}': {
        token t;
        t.type = TT_CCURLY;
        return t;
      }
    case '[': {
        token t;
        t.type = TT_OSQUARE;
        return t;
      }
    case ']': {
        token t;
        t.type = TT_CSQUARE;
        return t;
      }
    case '&':
    case ':':
    case '*':
    case '+':
    case '-':
    case '/':
    case '#':
    case '!':
    case '?':
    case '<':
    case '>':
    case '%':
      {
        token t;
        t.content.op = ch;
        t.type = TT_OP;
        return t;
      }
    default: {
      printf("Invalid character: %c\n", curr_char(l));
      dump_lexer_state(l);
      exit(1);
    }
  }
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
  size_t id = l->id;
  char ch = l->source[id];
  while (ch != '\n' && ch != '\0' && id < l->length) {
    printf("%c", ch);
    id++;
    ch = l->source[id];
  }
  printf("\nCursor @ line %zu column %zu\n", l->pos.line, l->pos.ch);
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
