#include "da_arena.h"
#include <da_arr.h>
#include <lexer.h>
#include <preprocessor.h>
#include <source_file.h>
#include <stdio.h>
#include <string.h>
#include <source_file.h>

#define PP_FUNC static inline

PP pp_new(source_file* sf){
  PP pp;
  pp.sf = sf;
  pp.task_stack = NULL;
  return pp;
}

PP_FUNC include_task get_include_data(lexer* l) {
  include_task inc = {0};
  char ch;
  ch = VALIDATE_CHAR(advance(l));
  char* buf = NULL;
  
  if (ch == '<') {
    inc.angled = true;
    ch = VALIDATE_CHAR(advance(l));
    while (ch != '>' && ch != '\0') {
      darr_push(buf, ch);
      ch = VALIDATE_CHAR(advance(l));
    }
    VALIDATE_CHAR(advance(l));  
  }
  else if (ch == '"') {
    inc.angled = false;
    ch = VALIDATE_CHAR(advance(l));
    while (ch != '"' && ch != '\0') {
      darr_push(buf, ch);
      ch = VALIDATE_CHAR(advance(l));
    }
    VALIDATE_CHAR(advance(l));
  }
  else {
    assert(false && "Invalid token after #include");
  }

  darr_push(buf, '\0');
  char* str = arena_alloc(l->sf->ar, strlen(buf) + 1);
  strcpy(str, buf);
  darr_free(buf);

  inc.filename = str;
  return inc;
}

char* include_read_file(PP* pp, include_task* inc) {
  char** paths = inc->angled ? pp->sf->sys_search_paths : pp->sf->search_paths;
  
  for (size_t i = 0; i < darr_len(paths); i++) {
    arena_mark_t m = arena_mark(pp->sf->ar);
    char* path = paths[i];
    size_t path_len = strlen(path);
    size_t filename_len = strlen(inc->filename);
    size_t len = path_len + filename_len + 2;
    char* buf = arena_alloc(pp->sf->ar, len);
  
    memcpy(buf, path, path_len);
    buf[path_len] = '/';
    memcpy(buf+path_len+1, inc->filename, filename_len);
    buf[len-1] = '\0';

    FILE* f = fopen(buf, "r");
    if (f) {
      fclose(f);
      return buf;
    }
    arena_mark_reset(&m, pp->sf->ar);
  }
  return NULL;
}

// these wont be needed.. because parse_if(PP*) handles these
PP_FUNC token parse_undef(PP* pp) { (void)pp; return ERROR_TOKEN("UNREACHABLE"); }
PP_FUNC token parse_elif(PP* pp) { (void)pp; return ERROR_TOKEN("UNREACHABLE"); }
PP_FUNC token parse_else(PP* pp) { (void)pp; return ERROR_TOKEN("UNREACHABLE"); }
PP_FUNC token parse_ifndef(PP* pp) { (void)pp; return ERROR_TOKEN("UNREACHABLE"); }
PP_FUNC token parse_endif(PP* pp) { (void)pp; return ERROR_TOKEN("UNREACHABLE"); }
PP_FUNC token parse_ifdef(PP* pp) { (void)pp; return ERROR_TOKEN("UNREACHABLE"); }

PP_FUNC token parse_define(PP* pp) { (void) pp; return (token) { .type = TT_ERROR, .content.str = "UNIMPLEMENTED"};  }
PP_FUNC token parse_if(PP* pp) { (void) pp;  return (token) { .type = TT_ERROR, .content.str = "UNIMPLEMENTED"}; }
PP_FUNC token parse_error(PP* pp) { (void) pp; return (token) { .type = TT_ERROR, .content.str = "UNIMPLEMENTED"};  }
PP_FUNC token parse_warning(PP* pp) { (void) pp; return (token) { .type = TT_ERROR, .content.str = "UNIMPLEMENTED"};  }

PP_FUNC token parse_include(PP* pp) {
  include_task inc = get_include_data(&pp->sf->l);
  char* source = include_read_file(pp, &inc);
  if (!source) return ERROR_TOKEN("Include file not found...");
  inc.sf = new_sf(pp->sf->ar, source);
  task_t t;
  t.type = PP_INCLUDE;
  t.val.inc = inc;
  darr_push(pp->task_stack, t);
  return pp_next_tok(&t.val.inc.sf->pp);
}

void pop_task(PP* pp) {
  task_t* t = darr_pop(pp->task_stack);

  switch (t->type) {
    case PP_INCLUDE: free_sf(t->val.inc.sf);
    default: return;
  }
}

token pp_next_tok(PP* pp) {
  size_t len = darr_len(pp->task_stack);
  if (len > 0) {
    task_t t = pp->task_stack[len-1];
    token tok = pp_next_tok(&t.val.inc.sf->pp);
    if (tok.type == TT_EOF) {
      pop_task(pp);
    }
    else
      return tok;
  }
  token t = next_tok(&pp->sf->l);
  if (t.type == TT_PREPROCESS) {
    t = next_tok(&pp->sf->l);
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
  darr_free(pp->task_stack);
}
