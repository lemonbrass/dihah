#define DA_ARR_IMPLEMENTATION
#define DA_ARENA_IMPLEMENTATION

#include <uid.h>
#include <stdio.h>

int main() {
  for (int i = 0; i < 10; i++) {
    uid_t uid = new_uid();
    set_sym(uid, NULL);
    printf("uid: %d %p ;; ", uid, get_sym(uid));
  }
  printf("\n");
  free_symbol_table();
  return 0;
}
