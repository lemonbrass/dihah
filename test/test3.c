#define DA_ARR_IMPLEMENTATION
#define DA_ARENA_IMPLEMENTATION

#include <source_file.h>
#include <uid.h>
#include <stdio.h>

int main() {
  source_file sf;
  new_sf(&sf, "test/src1.dihh");
  for (int i = 0; i < 10; i++) {
    uid_t uid = new_uid(&sf);
    set_sym(uid, &sf, NULL);
    printf("uid: %d %p ;; ", uid, get_sym(uid, &sf));
  }
  printf("\n");
  free_sf(&sf);
  return 0;
}
