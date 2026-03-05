#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>
#include <da_arena.h>
#include <da_arr.h>


// validate a character 
#define VALIDATE_CHAR(ch) ({\
  int tmp = (ch);\
  (char)((tmp != -1) ? tmp : (exit(1), '\0'));\
})

typedef enum {
  CC_NUM,
  CC_ALPHA,
  CC_SEPERATOR,
  CC_OPERATOR,
  CC_WHITESPACE,
  CC_EOF,
} CHAR_CLASS;

typedef enum {
  TT_OPAREN,     // ( 
  TT_CPAREN,     // ) 
  TT_OCURLY,     // {  
  TT_CCURLY,     // } 
  TT_OSQUARE,    // [  
  TT_CSQUARE,    // }  
  TT_ID,         //     
  TT_NUM,        //     
  TT_OP,         // + - / * &.....
  TT_KEYWORD,    // struct, int, bool, ....
  TT_EOF
} tok_type;

typedef struct {
  tok_type type;
  union {
    char* str;
    signed long num;
    char op;
  } content;
} token;

typedef struct {
  const char* source;
  arena ar;
  size_t length;
  size_t id;
  struct {
    size_t line;
    size_t ch;
  } pos;
} lexer;


void dump_lexer_state(lexer* l);
char curr_char(lexer* l);
int advance(lexer* l);
  
lexer new_lexer(const char* source);
token next_tok(lexer* l);
void free_lexer(lexer* l);
int peek(lexer* l);
char* read_file(const char* path);


#endif
