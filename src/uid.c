#include <da_arr.h>
#include <uid.h>



// auto initialised on push
void** SYMBOLS = NULL;

uid_t new_uid() {
  if (!SYMBOLS) darr_push(SYMBOLS, NULL);
  darr_push(SYMBOLS, NULL);
  static uid_t g_uid = 1;
  return g_uid++;
}

void* set_sym(uid_t uid, void* data) {
  void** x = darr_get(SYMBOLS, uid);
  return x ? (*x=data, *x) : NULL; 
}

void* get_sym(uid_t uid) {
  return darr_get(SYMBOLS, uid);
}

void free_symbol_table() {
  darr_free(SYMBOLS);
}
