#include "da_arr.h"
#include "lexer.h"
#include <preprocessor.h>
#include <string.h>

#define PP_FUNC static inline

PP pp_new(arena* ar, lexer* l) {
  PP pp;
  pp.l = l;
  pp.ar = ar;
  pp.task_stack = NULL;
  return pp;
}

PP_FUNC char* get_include_filename(lexer* l) {
  char ch;
  ch = VALIDATE_CHAR(advance(l));
  char* buf = NULL;
  darr_push(buf, ch);
  
  if (ch == '<') {
    ch = VALIDATE_CHAR(advance(l));
    while (ch != '>' && ch != '\0') {
      darr_push(buf, ch);
      ch = VALIDATE_CHAR(advance(l));
    }
    VALIDATE_CHAR(advance(l));  
  }
  else if (ch == '"') {
    ch = VALIDATE_CHAR(advance(l));
    while (ch != '"' && ch != '\0') {
      darr_push(buf, ch);
      ch = VALIDATE_CHAR(advance(l));
    }
    VALIDATE_CHAR(advance(l));
  }

  /*
    HERE BUF WILL BE either: <file.... OR "file.. (without closing >)
  */

  darr_push(buf, '\0');
  char* str = arena_alloc(l->ar, strlen(buf) + 1);
  strcpy(str, buf);
  darr_free(buf);

  return str;
}

// these wont be needed.. because parse_if(PP*) handles these
PP_FUNC token parse_undef(PP* pp) { (void)pp; return ERROR_TOKEN("UNREACHABLE"); }
PP_FUNC token parse_elif(PP* pp) { (void)pp; return ERROR_TOKEN("UNREACHABLE"); }
PP_FUNC token parse_else(PP* pp) { (void)pp; return ERROR_TOKEN("UNREACHABLE"); }
PP_FUNC token parse_ifndef(PP* pp) { (void)pp; return ERROR_TOKEN("UNREACHABLE"); }
PP_FUNC token parse_endif(PP* pp) { (void)pp; return ERROR_TOKEN("UNREACHABLE"); }
PP_FUNC token parse_ifdef(PP* pp) { (void)pp; return ERROR_TOKEN("UNREACHABLE"); }

PP_FUNC token parse_include(PP* pp) {
  include_task inc = {0};
  char* str = get_include_filename(pp->l);
  inc.angled = str[0] == '<';
  inc.filename = str+1;
  // return token logic
  return (token) { .type = TT_ERROR, .content.str = "UNIMPLEMENTED" };
}
PP_FUNC token parse_define(PP* pp) { (void) pp; return (token) { .type = TT_ERROR, .content.str = "UNIMPLEMENTED"};  }
PP_FUNC token parse_if(PP* pp) { (void) pp;  return (token) { .type = TT_ERROR, .content.str = "UNIMPLEMENTED"}; }
PP_FUNC token parse_error(PP* pp) { (void) pp; return (token) { .type = TT_ERROR, .content.str = "UNIMPLEMENTED"};  }
PP_FUNC token parse_warning(PP* pp) { (void) pp; return (token) { .type = TT_ERROR, .content.str = "UNIMPLEMENTED"};  }

token pp_next_tok(PP* pp) {
  token t = next_tok(pp->l);
  if (t.type == TT_PREPROCESS) {
    t = next_tok(pp->l);
    switch (t.type) {
      case TT_ID: {
          #define X(tt, v) if(strcmp(t.content.str, #v) == 0) return parse_##v(pp);
          DIRECTIVES(X)
          #undef X
        return ERROR_TOKEN("Invalid preprocessor directive");
        }
      default: {
        return ERROR_TOKEN("Invalid preprocessor directive");
      }
    }
  }
  return t;
}

void pp_free(PP *pp) {
  // TODO:
  (void)pp;
}
