#ifndef SOURCE_FILE_H
#define SOURCE_FILE_H

#include <lexer.h>
#include <preprocessor.h>

typedef struct {
  preprocessor pp;
  lexer l;
  char* filename;
  char* source;
  uid_t _g_next_uid;
  void** symbols; // uid_t -> symbols
  arena ar;
} source_file;

void new_sf(source_file* sf, char* filename);
void free_sf(source_file* sf);

#endif
