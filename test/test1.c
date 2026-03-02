#define DA_ARR_IMPLEMENTATION
#define DA_ARENA_IMPLEMENTATION
#define DA_HM_IMPLEMENTATION
#include <da_hm.h>
#include <stdio.h>

uint32_t hash(void* ch) {
  return (uint32_t)(*((char*)ch));
}

int main() {
  hashmap hm = dahm_new(char, int, &hash, &hash, &hash);
  dahm_set(hm, 'h', 69);
  printf("val %d\n", *dahm_get(hm, 'h', int));
  dahm_free(hm);
  return 0;
}
