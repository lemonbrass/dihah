#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <da_hm.h>
#include <da_arena.h>
#include <lexer.h>

struct source_file;
extern hashmap defines; // GLOBAL for all parsers.....


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

// used inside hashmap called define_table....
typedef struct {
  token* args;
  token* val;
} define_data;

// used when actually macros are accessed
typedef struct {
  define_data* data;
  token* buffer;
  size_t pos;
} macro_task;

// FOR IF, IFDEF, IFNDEF, ELIF, [ENDIF (0 TOKEN TASK)]
typedef struct {
  // TODO: needs an expression evaluator/parser.....
} cond_task;

// TODO: header file search places
typedef struct {
  char* filename;
  bool angled;
  lexer l;
} include_task;

typedef struct {
  PP_DIRECTIVE type;
  union {
    include_task inc;
    cond_task cond;
  } val;
} task_t;


typedef struct {
  lexer* l;
  task_t* task_stack;
  arena* ar;
  struct source_file* sf;
} preprocessor;

typedef preprocessor PP;

// cant find opportunity to add sizeof(pp).... ;D
PP pp_new(arena* ar, lexer* l);
token pp_next_tok(PP* pp);
void pp_free(PP* pp);

#endif
