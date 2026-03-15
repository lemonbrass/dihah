#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include "da_string.h"
#include <da_arena.h>
#include <lexer.h>
#include <thirdparty/khash.h>

struct source_file;

#define DIRECTIVES(X)\
  X(PP_INCLUDE, include)\
  X(PP_DEFINE, define)\
  X(PP_UNDEF, undef)\
  X(PP_IF, if)\
  X(PP_IFNDEF, ifndef)\
  X(PP_ELIF, elif)\
  X(PP_ELSE, else)\
  X(PP_ENDIF, endif)\
  X(PP_ERROR, error)\
  X(PP_WARNING, warning)\
  X(PP_IFDEF, ifdef)


/*
 THIS IS A STREAMING PREPROCESSOR.... MORE MEMORY EFFICIENT AND MAYBE SIMILAR IN PERFORMANCE

 IT USES TASK SYSTEM ->
   - when preprocessing starts -> returns normal tokens until a PP_DIRECTIVE is found...
   - when PP_DIRECTIVE is found. pp->task_stack changes
     to that directive.
   - this pp->task_stack contains all data needed for next call to pp_next_tok(PP*) to
     start where it was left off

    task_stack:
      include some file
        inside that file: include other file
          inside that other file: #if directive  <---- THIS IS TOP of stack

    Some tasks dont need any data at all. like #error or #warning and even #undef....
    They are "zero token" type and dont return a token technically.
    We eat all #error/#warning tokens and return next useful token revursively...
*/

typedef enum {
  #define X(ppd, _) ppd,
    DIRECTIVES(X)
  #undef X
} PP_DIRECTIVE;

typedef kvec_t(token) token_arr;
// used inside hashmap called define_table....
typedef struct {
  token_arr args;
  token_arr val;
  bool disabled;
} define_data;

// used when actually macros are accessed
typedef struct {
  define_data* data;
  token_arr buffer;
  size_t pos;
} macro_task;

// FOR IF, IFDEF, IFNDEF, ELIF, [ENDIF (0 TOKEN TASK)]
typedef struct {
  // TODO: needs an expression evaluator/parser.....
} cond_task;

// TODO: header file search places
typedef struct {
  string_view filename;
  bool angled;
  struct source_file* sf;
} include_task;

typedef struct {
  PP_DIRECTIVE type;
  union {
    include_task inc;
    cond_task cond;
    macro_task macro;
  } val;
} task_t;

khint_t sv_hash(string_view sv);

KHASH_INIT(macromap, string_view, define_data, 1, sv_hash, s_cmp);

typedef struct {
  kvec_t(task_t) task_stack;
  khash_t(macromap)* defines;
  struct source_file* sf;
} preprocessor;

typedef preprocessor PP;

// cant find opportunity to add sizeof(pp).... ;D
PP pp_new(struct source_file* sf);
token pp_next_tok(PP* pp);
void pp_free(PP* pp);

#endif
