#define DA_ARR_IMPLEMENTATION
#define DA_ARENA_IMPLEMENTATION
#define DA_HM_IMPLEMENTATION
#include <da_hm.h>
#include <stdio.h>

size_t hash(void* ch) {
  return (size_t)(*((char*)ch));
}

int main() {
  hashmap hm = dahm_new(char, int, &hash);
  dahm_set(hm, 'h', 69);
  dahm_set(hm, 'i', 34);
  dahm_set(hm, 'j', 35);
  printf("val %d\n", *dahm_get(hm, 'h', int));
  printf("val %d\n", *dahm_get(hm, 'i', int));
  printf("val %d\n", *dahm_get(hm, 'j', int));
  dahm_free(hm);
  return 0;
}
