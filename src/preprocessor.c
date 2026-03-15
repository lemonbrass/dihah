#include "da_string.h"
#include "thirdparty/khash.h"
#include "thirdparty/kvec.h"
#include "token.h"
#include <assert.h>
#include <da_arena.h>
#include <utils.h>
#include <lexer.h>
#include <preprocessor.h>
#include <source_file.h>
#include <stdio.h>
#include <string.h>

#define LEXER &pp->sf->l
#define ARENA pp->sf->ar
#define PP_FUNC static inline


PP pp_new(source_file* sf){
  PP pp;
  pp.sf = sf;
  kv_init(pp.task_stack);
  pp.defines = kh_init(macromap);
  return pp;
}

PP_FUNC include_task get_include_data(lexer* l) {
  include_task inc = {0};
  char ch;
  ch = advance(l);
  da_string buf = ds_new(l->sf->ar);
  
  if (ch == '<') {
    inc.angled = true;
    ch = advance(l);
    while (ch != '>' && ch != '\0') {
      ds_push_char(&buf, ch);
      ch = advance(l);
    }
    EOF_CHECK(ch);
    advance(l);  
  }
  else if (ch == '"') {
    inc.angled = false;
    ch = advance(l);
    while (ch != '"' && ch != '\0') {
      ds_push_char(&buf, ch);
      ch = advance(l);
    }
    EOF_CHECK(ch);
    advance(l);
  }
  else {
    assert(false && "Invalid token after #include");
  }

  ds_push_char(&buf, '\0');
  inc.filename = ds_build(&buf);
  return inc;
}

char* include_read_file(PP* pp, include_task* inc) {
  kvec_t(char*)* paths;
  if (inc->angled) paths = (void*)&pp->sf->sys_search_paths;
  else paths = (void*)&pp->sf->search_paths;
  
  for (size_t i = 0; i < kv_size(*paths); i++) {
    arena_mark_t m = arena_mark(ARENA);
    char* path = kv_A(*paths, i);
    size_t path_len = strlen(path);
    size_t filename_len = s_len(inc->filename);
    size_t len = path_len + filename_len + 2;
    char* buf = arena_alloc(ARENA, len);
  
    memcpy(buf, path, path_len);
    buf[path_len] = '/';
    memcpy(buf+path_len+1, s_str(inc->filename), filename_len);
    buf[len-1] = '\0';

    FILE* f = fopen(buf, "r");
    if (f) {
      fclose(f);
      return buf;
    }
    arena_mark_reset(&m, ARENA);
  }
  return NULL;
}

PP_FUNC token parse_define(PP* pp) {
  // non function ones :-
  // #define ______ _______till \n.
  token name = match_consume(LEXER, TT_ID);
  if (try_eat_tok(LEXER, TOKEN_SYM(TT_OPAREN))) {
    // TODO: Function like macros
    return ERROR_TOKEN("UNIMPLEMENTED");
  } else {
    define_data dd;
    kv_init(dd.args);
    kv_init(dd.val);
    size_t currline = name.pos.line;
    token tok = next_tok(LEXER);
    lexer l_mark = *LEXER;
    do {
      kv_push(token, dd.val, tok);
      l_mark = *LEXER;
      tok = next_tok(LEXER);
    } while (currline == tok.pos.line);
    *LEXER = l_mark;
    int res;
    khint_t k = kh_put(macromap, pp->defines, (name.content.str), &res);
    kh_val(pp->defines, k) = dd;
  }

  return pp_next_tok(pp);
}

PP_FUNC token parse_include(PP* pp) {
  include_task inc = get_include_data(LEXER);
  char* source = include_read_file(pp, &inc);
  if (!source) return ERROR_TOKEN("Include file not found...");
  inc.sf = new_sf(ARENA, source);
  task_t t;
  t.type = PP_INCLUDE;
  t.val.inc = inc;
  kv_push(task_t, pp->task_stack, t);
  return pp_next_tok(&t.val.inc.sf->pp);
}

void pop_task(PP* pp) {
  task_t* t = &kv_pop(pp->task_stack);

  switch (t->type) {
    case PP_INCLUDE:
      free_sf(t->val.inc.sf);
      break;
    default:
      break;
  }
}

token get_include_token(PP* pp, task_t* t) {
  token tok = pp_next_tok(&t->val.inc.sf->pp);
  if (tok.type == TT_EOF) {
    pop_task(pp);
    return pp_next_tok(pp);
  }
  else {
    return tok;
  }
}

void create_macro_task(PP* pp, define_data* data) {
  task_t macro_task;
  macro_task.type = PP_DEFINE;
  kv_init(macro_task.val.macro.buffer);
  macro_task.val.macro.data = data;
  macro_task.val.macro.pos = 0;
  if (kv_size(data->args)==0) {
    // macro.buffer is READ-ONLY here....
    macro_task.val.macro.buffer = data->val;
  }
  else {
    // TODO: Function like macros
    assert(false);
  }
  kv_push(task_t, pp->task_stack, macro_task);
}

token get_define_token(PP* pp, task_t* t) {
  if (kv_size(t->val.macro.data->args) == 0) {
    if (t->val.macro.pos >= kv_size(t->val.macro.buffer)) {
      pop_task(pp);
      return pp_next_tok(pp);
    }
    return kv_A(t->val.macro.buffer, t->val.macro.pos++);
  }
  else {
    // TODO: Function like macros
    return ERROR_TOKEN("UNIMPLEMENTED");
  }
}

token pp_next_tok(PP* pp) {
  size_t len = kv_size(pp->task_stack);
  if (len > 0) {
    task_t* t = &kv_A(pp->task_stack, len-1);
    switch (t->type) {
      case PP_INCLUDE: return get_include_token(pp, t);
      case PP_DEFINE: return get_define_token(pp, t);
      default: return ERROR_TOKEN("UNIMPLEMENTED");
    }
  }
  token t = next_tok(LEXER);
  if (t.type == TT_PREPROCESS) {
    t = next_tok(LEXER);
    switch (t.type) {
      case TT_ID: {
        if (cs_cmp(t.content.str, "include")) return parse_include(pp);
        if (cs_cmp(t.content.str, "define")) return parse_define(pp);
        return ERROR_TOKEN("Invalid preprocessor directive");
      }
      default: {
        return ERROR_TOKEN("Invalid preprocessor directive");
      }
    }
  }
  else if (t.type == TT_ID) { // maybe macro
    khint_t k = kh_get(macromap, pp->defines, t.content.str);
    if (k != kh_end(pp->defines)) {
      define_data* data = &kh_val(pp->defines, k);
      create_macro_task(pp, data);
      return pp_next_tok(pp);
    }
  }
  return t;
}

void pp_free(PP *pp) {
  kv_destroy(pp->task_stack);
  kh_destroy(macromap, pp->defines);
}

// PP_FUNC token parse_if(PP* pp) { (void) pp;  return ERROR_TOKEN("UNIMPLEMENTED"); }
// PP_FUNC token parse_error(PP* pp) { (void) pp; return ERROR_TOKEN("UNIMPLEMENTED");  }
// PP_FUNC token parse_warning(PP* pp) { (void) pp; return ERROR_TOKEN("UNIMPLEMENTED");  }

