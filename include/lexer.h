#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>
#include <da_arena.h>
#include <da_arr.h>
#include <stdio.h>



// TODO: MULTICHAR ESCAPES
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

#define OPERATORS(X) \
  X(TT_LSHIFT_EQ, "<<=") \
  X(TT_RSHIFT_EQ, ">>=") \
  X(TT_ELLIPSIS, "...") \
  X(TT_INC, "++") \
  X(TT_DEC, "--") \
  X(TT_ARROW, "->") \
  X(TT_LSHIFT, "<<") \
  X(TT_RSHIFT, ">>") \
  X(TT_LE, "<=") \
  X(TT_GE, ">=") \
  X(TT_EQ, "==") \
  X(TT_NEQ, "!=") \
  X(TT_ANDAND, "&&") \
  X(TT_OROR, "||") \
  X(TT_ADD_EQ, "+=") \
  X(TT_SUB_EQ, "-=") \
  X(TT_MUL_EQ, "*=") \
  X(TT_DIV_EQ, "/=") \
  X(TT_MOD_EQ, "%=") \
  X(TT_AND_EQ, "&=") \
  X(TT_OR_EQ, "|=") \
  X(TT_XOR_EQ, "^=") \
  X(TT_PLUS, "+") \
  X(TT_MINUS, "-") \
  X(TT_STAR, "*") \
  X(TT_SLASH, "/") \
  X(TT_PERCENT, "%") \
  X(TT_LT, "<") \
  X(TT_GT, ">") \
  X(TT_ASSIGN, "=") \
  X(TT_NOT, "!") \
  X(TT_AND, "&") \
  X(TT_OR, "|") \
  X(TT_XOR, "^") \
  X(TT_TILDE, "~") \
  X(TT_QUESTION, "?") \
  X(TT_COLON, ":") \
  X(TT_DOT, ".")

#define SEPERATORS(X)\
  X(TT_OPAREN, '(')\
  X(TT_CPAREN, ')')\
  X(TT_OCURLY, '{')\
  X(TT_CCURLY, '}')\
  X(TT_OSQUARE, '[')\
  X(TT_CSQUARE, ']')\
  X(TT_COL, ':')\
  X(TT_COMMA, ',')\
  X(TT_SEMICOL, ';')

#define KEYWORDS(X)\
  X(TT_VOID, "void")\
  X(TT_CHAR, "char")\
  X(TT_SHORT, "short")\
  X(TT_INT, "int")\
  X(TT_LONG, "long")\
  X(TT_FLOAT, "float")\
  X(TT_DOUBLE, "double")\
  X(TT_BOOL, "bool")\
  X(TT_SIGNED, "signed")\
  X(TT_UNSIGNED, "unsigned")\
  X(TT_STRUCT, "struct")\
  X(TT_UNION, "union")\
  X(TT_MACRO, "macro")\
  X(TT_ENUM, "enum")\
  X(TT_TYPEDEF, "typedef")\
  X(TT_IF, "if")\
  X(TT_ELSE, "else")\
  X(TT_SWITCH, "switch")\
  X(TT_CASE, "case")\
  X(TT_DEFAULT, "default")\
  X(TT_FOR, "for")\
  X(TT_WHILE, "while")\
  X(TT_DO, "do")\
  X(TT_BREAK, "break")\
  X(TT_CONTINUE, "continue")\
  X(TT_RETURN, "return")\
  X(TT_GOTO, "goto")\
  X(TT_STATIC, "static")\
  X(TT_EXTERN, "extern")\
  X(TT_CONST, "const")\
  X(TT_VOLATILE, "volatile")\
  X(TT_SIZEOF, "sizeof")\
  X(TT_INLINE, "inline")

// validate a character 
#define VALIDATE_CHAR(ch) ({\
  int tmp = (ch);\
  (char)((tmp != -1) ? tmp : (printf("VALIDATE_CHAR error: ch = -1 & line = %d\n", __LINE__), exit(1), '\0'));\
})

#define ERROR_TOKEN(msg) ({\
  token t;\
  t.type = TT_ERROR;\
  t.content.str = msg;\
  t;\
})

// TODO: symantic analysis: ID CANT COME AFTER NUM... etc
typedef enum {
  CC_NUM,
  CC_ALPHA,
  CC_SEPERATOR,
  CC_OPERATOR,
  CC_WHITESPACE,
  CC_EOF,
} CHAR_CLASS;

typedef enum {
  TT_ID,         //     
  TT_NUM,        //     
  #define X(tt, v) tt,
  KEYWORDS(X)
  SEPERATORS(X)
  OPERATORS(X)
  #undef X
  TT_STRING,
  TT_EOF,
  TT_ERROR,
  TT_PREPROCESS,
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
  arena* ar;
  size_t length;
  size_t id;
  struct {
    size_t line;
    size_t ch;
  } pos;
} lexer;



bool lstrmatch(lexer* l, const char* str);
void print_token(token* t);
void print_token_str(token* t);
void dump_lexer_state(lexer* l);
char curr_char(lexer* l);
int advance(lexer* l);
  
lexer new_lexer(const char* source, arena* ar);
token next_tok(lexer* l);
int peek(lexer* l);
char* read_file(arena* ar, const char *path);
// compare next token with a str which is transformed to a token....
bool eat_str(lexer* l, const char* str);
bool try_eat_str(lexer* l, const char* str);
bool try_eat_tok(lexer* l, const token t);
bool eat_tok(lexer* l, const token t);
token match_consume(lexer* l, const tok_type t);
char* tok_to_str(token t);

#endif
