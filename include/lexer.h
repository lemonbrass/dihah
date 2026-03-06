#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>
#include <da_arena.h>
#include <da_arr.h>
#include <stdio.h>

#define ESCAPES(X) \
  X('n','\n') \
  X('t','\t') \
  X('r','\r') \
  X('a','\a') \
  X('b','\b') \
  X('f','\f') \
  X('v','\v') \
  X('\\','\\') \
  X('"','"') \
  X('?','?') \
  X('0','\0')

#define BRACKETS(X)\
  X(TT_OPAREN, '(')\
  X(TT_CPAREN, ')')\
  X(TT_OCURLY, '{')\
  X(TT_CCURLY, '}')\
  X(TT_OSQUARE, '[')\
  X(TT_CSQUARE, ']')

#define KEYWORDS(X)\
  X(void, TT_VOID)\
  X(char, TT_CHAR)\
  X(short, TT_SHORT)\
  X(int, TT_INT)\
  X(long, TT_LONG)\
  X(float, TT_FLOAT)\
  X(double, TT_DOUBLE)\
  X(bool, TT_BOOL)\
  X(signed, TT_SIGNED)\
  X(unsigned, TT_UNSIGNED)\
  X(struct, TT_STRUCT)\
  X(union, TT_UNION)\
  X(enum, TT_ENUM)\
  X(typedef, TT_TYPEDEF)\
  X(if, TT_IF)\
  X(else, TT_ELSE)\
  X(switch, TT_SWITCH)\
  X(case, TT_CASE)\
  X(default, TT_DEFAULT)\
  X(for, TT_FOR)\
  X(while, TT_WHILE)\
  X(do, TT_DO)\
  X(break, TT_BREAK)\
  X(continue, TT_CONTINUE)\
  X(return, TT_RETURN)\
  X(goto, TT_GOTO)\
  X(static, TT_STATIC)\
  X(extern, TT_EXTERN)\
  X(const, TT_CONST)\
  X(volatile, TT_VOLATILE)\
  X(sizeof, TT_SIZEOF)\
  X(inline, TT_INLINE)

// validate a character 
#define VALIDATE_CHAR(ch) ({\
  int tmp = (ch);\
  (char)((tmp != -1) ? tmp : (printf("VALIDATE_CHAR error: ch = -1 & line = %d\n", __LINE__), exit(1), '\0'));\
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
  #define X(k, keyword_type) keyword_type,
  KEYWORDS(X)
  #undef X
  TT_STRING,
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



void print_token(token* t);
void dump_lexer_state(lexer* l);
char curr_char(lexer* l);
int advance(lexer* l);
  
lexer new_lexer(const char* source);
token next_tok(lexer* l);
void free_lexer(lexer* l);
int peek(lexer* l);
char* read_file(const char* path);


#endif
