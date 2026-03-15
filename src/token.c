#include "da_string.h"
#include <token.h>
#include <lexer.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

void print_token_str(token* t) {
  switch (t->type) {
    #define X(tt, v) case tt:\
      printf("%s", v);\
      break;
    OPERATORS(X)
    KEYWORDS(X)
    #undef X
    #define X(tt, v) case tt:\
      printf("%c", v);\
      break;
    SEPERATORS(X)
    #undef X
    case TT_STRING:
      printf("\"");
      s_print(t->content.str);
      printf("\"");
      break;
    case TT_ID:
      s_print(t->content.str);
      break;
    case TT_NUM:
      printf("%ld", t->content.num);
      break;
    case TT_EOF: break;
    case TT_ERROR:
      printf("ERROR: ");
      s_print(t->content.str);
      printf("\n");
      assert(false);
      break;
    case TT_PREPROCESS:
      printf("#");
      break;
  }
}

void print_token(token* t) {
  switch (t->type) {
    case TT_PREPROCESS: printf("TT_PREPROCESS(#)"); break;
    case TT_ID: printf("TT_ID(%.*s)", (int)s_len(t->content.str), s_str(t->content.str)); break;
    case TT_NUM: printf("TT_NUM(%ld)", t->content.num); break;
    case TT_EOF: printf("TT_EOF"); break;
    case TT_STRING: {
      printf("TT_STRING(\"");
      for (size_t i = 0; i < s_len(t->content.str); i++) {
        char ch = s_str(t->content.str)[i];
        switch (ch) {
          #define X(c, escapecode) case escapecode: printf("\\%c", c); break;
            ESCAPES(X)
            default: printf("%c", ch);
          #undef X
        }
      }
      printf("\")");
      break;
    }
    case TT_ERROR: printf("ERROR: %.*s\n", (int)s_len(t->content.str), s_str(t->content.str)); assert(false);
    #define X(tt, v) case tt: printf("%s(%c)", #tt, v); break;
      SEPERATORS(X)
    #undef X
    #define X(tt, v) case tt: printf("%s(%s)", #tt, v); break;
      KEYWORDS(X)
      OPERATORS(X)
    #undef X
  }
}

bool tok_cmp(token t1, token t2) {
  if (t1.type != t2.type) return false;
  switch (t1.type) {
    #define X(tt, val) case tt: 
    OPERATORS(X)
    SEPERATORS(X)
    KEYWORDS(X)
    #undef X
    case TT_EOF: {
      return true;
    }

    case TT_ID:
    case TT_STRING: {
      return s_cmp(t1.content.str, t2.content.str) == 0;
    }
    case TT_NUM: {
      return t1.content.num == t2.content.num;
    }
    default: assert(false);
  }
}

token new_token_string(struct lexer* l, string_view str) {
  token t;
  t.content.str = str;
  t.pos = l->tok_start;
  t.type = TT_STRING;
  return t;
}
token new_token_ident(struct lexer* l, string_view str) {
  token t;
  t.pos = l->tok_start;
  t.content.str = str;
  t.type = TT_ID;
  return t;
}
token new_token_num(struct lexer* l, size_t num) {
  token t;
  t.pos = l->tok_start;
  t.content.num = num;
  t.type = TT_NUM;
  return t;
}
token new_token_simple(struct lexer* l, tok_type tt) {
  token t;
  t.pos = l->tok_start;
  t.type = tt;
  return t;
}
token new_token_complex(struct lexer* l, tok_type tt, tok_data data) {
  token t;
  t.pos = l->tok_start;
  t.content = data;
  t.type = tt;
  return t;
}

