#ifndef DA_STR_H
#define DA_STR_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  size_t len;
  char* str;
} const_str;

const_str str_from(char* str) {
  size_t len = strlen(str);

  char* copy = malloc(len * sizeof(char));
  if (copy)
    memcpy(copy, str, len * sizeof(char));
  
  const_str cstr = {0};
  cstr.len = len;
  cstr.str = copy;
  
  return cstr;
}

#endif
