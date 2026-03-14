#include <uid.h>
#include <thirdparty/kvec.h>



void set_sym(uid_t uid, source_file* sf, void* data) {
  kv_A(sf->symbols, uid) = data;
}

uid_t new_uid(source_file* sf) {
  kv_push(void*, sf->symbols, NULL);
  return sf->_g_next_uid++;
}
void* get_sym(uid_t uid, source_file* sf) {
  return kv_A(sf->symbols, uid);
}

