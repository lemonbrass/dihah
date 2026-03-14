#ifndef TOKEN_H
#define TOKEN_H

#include <da_string.h>
#include <stdbool.h>
#include <stddef.h>

struct lexer;

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

#define ERROR_TOKEN(msg) ({\
  token t;\
  t.type = TT_ERROR;\
  t.content.str = sv_new(msg, strlen(msg));\
  t;\
})


typedef enum {
  TT_ID,  
  TT_NUM, 
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
  size_t line;
  size_t ch;
  size_t id;
} pos_t;

typedef union {
  string_view str;
  size_t num;
  char op;
} tok_data;
  
typedef struct {
  tok_type type;
  pos_t pos;
  tok_data content;
} token;

void print_token(token* t);
void print_token_str(token* t);
string_view tok_to_str(token t);
bool tok_cmp(token t1, token t2);

token new_token_string(struct lexer* l, string_view str);
token new_token_ident(struct lexer* l, string_view str);
// TODO: 123F and 123UL etc..... and different number types
token new_token_num(struct lexer* l, size_t num);
token new_token_simple(struct lexer* l, tok_type tt);
token new_token_complex(struct lexer* l, tok_type tt, tok_data data);

#endif
