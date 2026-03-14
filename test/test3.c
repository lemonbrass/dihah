#define DA_ARENA_IMPLEMENTATION

#include <da_string.h>
#include <da_arena.h>
#include <source_file.h>
#include <uid.h>
#include <stdio.h>

int main() {
  arena* ar = arena_new(1024*16, 0);
  source_file* sf = new_sf(ar, "test/src1.dihh");
  for (int i = 0; i < 10; i++) {
    uid_t uid = new_uid(sf);
    set_sym(uid, sf, NULL);
    printf("uid: %d %p ;; ", uid, get_sym(uid, sf));
  }
  printf("\n");
  free_sf(sf);
  arena_free(ar);
  return 0;
}
