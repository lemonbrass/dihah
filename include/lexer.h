#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>
#include <da_arena.h>
#include <token.h>
#include <stdio.h>


#define EOF_CHECK(ch) do {assert(ch!='\0' && "Unexpected EOF");} while(0)

// TODO: symantic analysis: ID CANT COME AFTER NUM... etc
typedef enum {
  CC_NUM,
  CC_ALPHA,
  CC_SEPERATOR,
  CC_OPERATOR,
  CC_WHITESPACE,
  CC_EOF,
} CHAR_CLASS;

struct source_file;

typedef struct lexer lexer;

struct lexer{
  struct source_file* sf;
  size_t length;
  size_t id;
  pos_t pos;
};

bool lstrmatch(lexer* l, const char* str);
void dump_lexer_state(lexer* l);
char curr_char(lexer* l);
char advance(lexer* l);

lexer new_scratch_lexer(const char* str);
void free_scratch_lexer(lexer* l);
  
lexer new_lexer(struct source_file* sf);
token next_tok(lexer* l);
int peek(lexer* l);

bool try_eat_tok(lexer* l, const token t);
bool eat_tok(lexer* l, const token t);
token match_consume(lexer* l, const tok_type t);

#endif
