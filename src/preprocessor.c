#include "lexer.h"
#include <preprocessor.h>

PP pp_new(lexer* l) {
  PP pp;
  pp.l = l;
  pp.directives = NULL;
  return pp;
}


void parse_include(PP* pp);
void parse_define(PP* pp);
void parse_undef(PP* pp);
void parse_if(PP* pp);
void parse_ifndef(PP* pp);
void parse_elif(PP* pp);
void parse_else(PP* pp);
void parse_endif(PP* pp);
void parse_error(PP* pp);
void parse_warning(PP* pp);
void parse_ifdef(PP* pp);

void pp_parse(PP* pp) {
  token t = next_tok(pp->l);
  switch (t.type) {
    case TT_ID: {
        #define X(tt, v) if(strcmp(t.content.str, #v) == 0) parse_##v(pp);
        DIRECTIVES(X)
        #undef X
      }
    default: {}
  }
}

void pp_pass1(PP* pp) {
  lexer l_mark = *pp->l;
  token t = next_tok(pp->l);
  while (t.type != TT_EOF) {
    if (t.type == TT_PREPROCESS)
        pp_parse(pp);
    t = next_tok(pp->l);
  }
  *pp->l = l_mark;
}


void pp_pass2(PP* pp);
