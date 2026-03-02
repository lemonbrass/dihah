#ifndef LEXER_H
#define LEXER_H

#include <da_arr.h>

typedef enum {
  OPAREN,     // ( 
  CPAREN,     // ) 
  OCURLY,     // {  
  CCURLY,     // }  
  ID,         //     
  NUM,        //     
  OP,         // + - / * &.....
  KEYWORD,    // struct, int, bool, ....
  EOF
} tok_type;

typedef struct {
  tok_type type;
  union {
    char* str;
    size_t num;
    char op;
  } content;
} token;

typedef struct {
  const char* source;
  size_t length;
  size_t id;
  struct {
    size_t line;
    size_t ch;
  } pos;
} lexer;


char curr_char(lexer* l);
char advance(lexer* l);
  
lexer new_lexer(char* source, size_t len);
token next_tok(lexer* l);
void free_lexer();



#endif
