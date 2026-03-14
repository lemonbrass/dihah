#ifndef UID_H
#define UID_H

#include <source_file.h>
#include <stdint.h>

typedef uint32_t uid_t;

void set_sym(uid_t uid, source_file* sf, void* data);
uid_t new_uid(source_file* sf);
void* get_sym(uid_t uid, source_file* sf);

#endif
