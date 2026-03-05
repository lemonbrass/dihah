#ifndef DA_HM_H
#define DA_HM_H

#ifndef HM_DEFAULT_CAPACITY
#define HM_DEFAULT_CAPACITY 128
#endif

#ifndef HM_LOAD_FACTOR
#define HM_LOAD_FACTOR 0.75
#endif

#include <linkedlist.h>
#include <stddef.h>
#include <stdint.h>
#include <da_arena.h>
#include <da_arr.h>

#define ID_FROM_HASH(hm, hash) (hash)%(hm.cap)
#define dahm_new(k_type, v_type, hash) _dahm_new_helper(sizeof(k_type), sizeof(v_type), hash)
// #define dahm_set(hm, k, val) ({\
//   typeof(k) key_temp = k;\
//   uint32_t h = hm.hash(&key_temp);\
//   linked_list* ll = darr_get(hm.values, ID_FROM_HASH(hm, h));\
//   hm_val hv = {0};\
//   hv.value = arena_alloc(&hm.ar, hm.valsize);\
//   hv.key = arena_alloc(&hm.ar, hm.keysize);\
//   *(typeof(k)*)hv.key = key_temp;\
//   *((typeof(val)*)hv.value) = val;\
//   hv.h = h;\
//   ll_push(ll, &hm.ar, hv);\
// })

#define dahm_set(hm, k, val) ({\
  if ((double)(hm).len >= (double)(hm).cap * HM_LOAD_FACTOR){\
    (hm) = _hashmap_resize(hm);\
  }\
  typeof(k) key_temp = (k);\
  uint32_t h = (hm).hash(&key_temp);\
  linked_list* ll = darr_get((hm).values, ID_FROM_HASH(hm, h));\
  size_t id = 0;\
  hm_val* existing = ll ? ll_get(ll, id) : NULL;\
  while (existing && (*(typeof(k)*)existing->key != key_temp)) {\
    id++;\
    existing = ll_get(ll, id);\
  }\
  if (existing) {\
    *((typeof(val)*)existing->value) = val;\
  } else {\
    hm_val hv = {0};\
    hv.value = arena_alloc(&hm.ar, hm.valsize);\
    hv.key = arena_alloc(&hm.ar, hm.keysize);\
    *(typeof(k)*)hv.key = key_temp;\
    *((typeof(val)*)hv.value) = val;\
    hv.h = h;\
    ll_push(ll, &hm.ar, hv);\
    (hm).len++;\
  }\
})

#define dahm_get(hm, k, valtype) ({\
  typeof(k) key_temp = k;\
  uint32_t h = hm.hash(&key_temp);\
  linked_list* ll = darr_get(hm.values, ID_FROM_HASH(hm, h));\
  size_t id = 0;\
  hm_val* val = ll ? ll_get(ll, id) : NULL;\
  while (val && (val->h != h || *(typeof(k)*)val->key != key_temp)) {\
    id++;\
    val = ll_get(ll, id);\
  }\
  (valtype*)(val ? val->value : NULL);\
})
#define dahm_free(hm) ({ arena_free(&(hm).ar); darr_free((hm).values); })

typedef struct {
  uint32_t h;
  void* key;
  void* value;
} hm_val;

typedef struct {
  size_t keysize;
  size_t valsize;
  linked_list* values;
  arena ar;
  uint32_t (*hash)(void*);
  size_t cap, len;
} hashmap;


hashmap _hashmap_resize(hashmap hm);
uint32_t __nearest_pow2(uint32_t n);
hashmap _dahm_new_helper(size_t keysize, size_t valsize, uint32_t (*hash)(void*));


#ifdef DA_HM_IMPLEMENTATION

hashmap _dahm_new_helper(size_t keysize, size_t valsize, uint32_t (*hash)(void*)){
  hashmap hm;
  darr_new(hm.values, HM_DEFAULT_CAPACITY);
  hm.keysize = keysize;
  hm.valsize = valsize;
  hm.hash = hash;
  hm.cap = HM_DEFAULT_CAPACITY;
  hm.len = 0;
  hm.ar = arena_new(valsize*4, true);
  for (size_t i = 0; i<darr_len(hm.values); i++) {
    linked_list* ll = darr_get(hm.values, i);
    *ll = ll_new(hm_val);
  }
  return hm;
}

hashmap _dahm_new_with_cap(size_t keysize, size_t valsize, uint32_t (*hash)(void*), size_t cap, arena ar){
  hashmap hm;
  darr_new(hm.values, cap);
  hm.keysize = keysize;
  hm.valsize = valsize;
  hm.hash = hash;
  hm.cap = cap;
  hm.len = 0;
  hm.ar = ar;
  for (size_t i = 0; i<darr_len(hm.values); i++) {
    linked_list* ll = darr_get(hm.values, i);
    *ll = ll_new(hm_val);
  }
  return hm;
}

hashmap _hashmap_resize(hashmap hm) {
  hashmap hm_new = _dahm_new_with_cap(hm.keysize, hm.valsize, hm.hash, hm.cap*2, hm.ar);
  hm_new.len = hm.len;
  for (size_t i = 0; i < darr_len(hm.values); i++) {
    linked_list* ll = darr_get(hm.values, i);
    size_t id = 0;
    hm_val* hv = ll_get(ll, id);
    while (hv != NULL) {
      uint32_t hash = ((hm_val*)hv)->h;
      linked_list* ll_ = darr_get(hm_new.values, ID_FROM_HASH(hm_new, hash));
      ll_push_borrowed(ll_, &hm_new.ar, hv);
      id++;
      hv = ll_get(ll, id);
    }
  }
  darr_free((hm).values);
  return hm_new;
}

uint32_t __nearest_pow2(uint32_t x) {
    if (x <= 1) return 1;
    return 1u << (32 - __builtin_clz(x - 1));
}

#endif

#endif
