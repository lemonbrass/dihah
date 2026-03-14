#ifndef SOURCE_FILE_H
#define SOURCE_FILE_H

#include "thirdparty/kvec.h"
#include <utils.h>
#include <lexer.h>
#include <preprocessor.h>

typedef struct source_file source_file;

struct source_file{
  kvec_t(char*) search_paths;
  kvec_t(char*) sys_search_paths;
  preprocessor pp;
  lexer l;
  char* filename;
  const char* source;
  uid_t _g_next_uid;
  kvec_t(void*) symbols; // uid_t -> symbols
  arena* ar;
};

source_file* new_sf(arena* ar, char* filename);
void free_sf(source_file* sf);

#endif
