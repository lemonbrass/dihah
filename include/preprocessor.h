#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <da_arena.h>
#include <lexer.h>

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


typedef struct {
  char* name;
  token* args;
  token* val;
} define_data;
typedef struct {
  
} ifelif_data;
typedef struct {} else_data;
typedef struct {} include_data;
typedef struct {} undef_data;
typedef struct {} ifdefifndef_data;
typedef struct {} message_data;


typedef enum {
  #define X(tt, _) tt,
  DIRECTIVES(X)
  #undef X
} DIRECTIVE_TYPE;

typedef struct {
  DIRECTIVE_TYPE dt;
  union {} data; //TODO
} directive_t;

typedef struct {
  lexer* l;
  uid_t* directives;
} preprocessor;

typedef preprocessor PP;

// missed opportunity to add sizeof(pp)....
PP pp_new(lexer* l);
void pp_pass1(PP* pp);
void pp_pass2(PP* pp);

#endif
