#include <da_arr.h>
#include <uid.h>



void* set_sym(uid_t uid, source_file* sf, void* data) {
  void** container = darr_get(sf->symbols, uid);
  if (container) *container = data;
  else return NULL;
  return data;
}

uid_t new_uid(source_file* sf) {
  return sf->_g_next_uid++;
}
void* get_sym(uid_t uid, source_file* sf) {
  return darr_get(sf->symbols, uid);
}

