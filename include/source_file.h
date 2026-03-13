#ifndef SOURCE_FILE_H
#define SOURCE_FILE_H

#include <utils.h>
#include <lexer.h>
#include <preprocessor.h>

typedef struct source_file source_file;

struct source_file{
  char** search_paths;
  char** sys_search_paths;
  preprocessor pp;
  lexer l;
  char* filename;
  const char* source;
  uid_t _g_next_uid;
  void** symbols; // uid_t -> symbols
  arena* ar;
};

source_file* new_sf(arena* ar, char* filename);
void free_sf(source_file* sf);

#endif
