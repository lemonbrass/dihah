#ifndef UID_H
#define UID_H

#include <da_arr.h>
#include <stdint.h>

typedef uint32_t uid_t;
extern void** SYMBOLS;

void* set_sym(uid_t uid, void* data);
uid_t new_uid(void);
void* get_sym(uid_t uid);
void free_symbol_table();

#endif
