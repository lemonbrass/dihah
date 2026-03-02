#ifndef DA_HM_H
#define DA_HM_H

#include <linkedlist.h>
#include <stddef.h>
#include <stdint.h>
#include <da_arena.h>
#include <da_arr.h>

#define dahm_new(k_type, v_type, hash1, hash2, hash3) _dahm_new_helper(sizeof(k_type), sizeof(v_type), hash1, hash2, hash3)
#define dahm_set(hm, key, val) ({\
  typeof(key) key_temp = key;\
  uint32_t h1 = hm.hash1(&key_temp);\
  uint32_t h2 = hm.hash2(&key_temp);\
  uint32_t h3 = hm.hash3(&key_temp);\
  darr_grow(hm.values, __nearest_pow2(h1+1));\
  linked_list* ll = darr_get(hm.values, h1);\
  hm_val hv = {0};\
  hv.value = arena_alloc(&hm.ar, hm.valsize);\
  *((typeof(val)*)hv.value) = val;\
  hv.h1 = h1;\
  hv.h2 = h2;\
  hv.h3 = h3;\
  ll_push(ll, &hm.ar, hv);\
})
#define dahm_get(hm, key, valtype) ({\
  typeof(key) key_temp = key;\
  uint32_t h1 = hm.hash1(&key_temp);\
  uint32_t h2 = hm.hash2(&key_temp);\
  uint32_t h3 = hm.hash3(&key_temp);\
  linked_list* ll = darr_get(hm.values, h1);\
  size_t id = 0;\
  hm_val* val = ll ? ll_get(ll, id) : NULL;\
  while (val && (val->h1 != h1 || val->h2 != h2 || val->h3 != h3)) {\
    id++;\
    val = ll_get(ll, id);\
  }\
  (valtype*)(val ? val->value : NULL);\
})
#define dahm_free(hm) ({ arena_free(&(hm).ar); darr_free((hm).values); })

typedef struct {
  uint32_t h1, h2, h3;
  void* value;
} hm_val;

typedef struct {
  size_t keysize;
  size_t valsize;
  linked_list* values;
  arena ar;
  uint32_t (*hash1)(void*);
  uint32_t (*hash2)(void*);
  uint32_t (*hash3)(void*);
} hashmap;


uint32_t __nearest_pow2(uint32_t n);
hashmap _dahm_new_helper(size_t keysize, size_t valsize, uint32_t (*hash1)(void*), uint32_t (*hash2)(void*), uint32_t (*hash3)(void*));


#ifdef DA_HM_IMPLEMENTATION

hashmap _dahm_new_helper(size_t keysize, size_t valsize, uint32_t (*hash1)(void*), uint32_t (*hash2)(void*), uint32_t (*hash3)(void*)) {
  hashmap hm;
  darr_new(hm.values, DEFAULT_CAPACITY);
  hm.keysize = keysize;
  hm.valsize = valsize;
  hm.hash1 = hash1;
  hm.hash2 = hash2;
  hm.hash3 = hash3;
  hm.ar = arena_new(valsize*4, true);
  return hm;
}

uint32_t __nearest_pow2(uint32_t x) {
    if (x <= 1) return 1;
    return 1u << (32 - __builtin_clz(x - 1));
}

#endif

#endif
