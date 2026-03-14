#ifndef DA_ARR_H
#define DA_ARR_H

#include<stdlib.h>
#include<stdalign.h>
#include<stddef.h>
#include<stdint.h>
#include<stdbool.h>


#define CAPACITY_BASED_BOUNDS (1u << 0)

#ifndef ARR_DEFAULT_CAPACITY
#define ARR_DEFAULT_CAPACITY 4
#endif

#define darr_new_custom(arr, cap, flags, alloc, free, realloc) arr = _darr_new_helper(sizeof(*arr), cap, flags, alloc, free, realloc)
#define darr_new(arr, cap, flags) arr = _darr_new_helper(sizeof(*arr), cap, flags, &malloc, &free, &realloc)
// AVOID Double Evaluation
#define darr_len(arr) _darr_len_helper(arr)
#define darr_cap(arr) _darr_cap_helper(arr)
#define darr_get(arr, num) (_darr_get_helper(arr, num) ? &arr[num] : NULL)
#define darr_grow(arr, cap) _darr_grow_helper(arr, cap)

/* arr must be lvalue

 TODO: Realloc NULL rewrites arr instead of returning false. (fix using ({}) and use #if defined (__GNU__).....)
 #define darr_push(arr, item) (\
   (arr) = _darr_grow(sizeof(*arr), arr),\
   (arr) != NULL ? ((arr)[_darr_get_data(arr)->count++] = item, true) : false\
)  */

#define darr_push(arr, item) ({\
  bool res = false;\
  void* tmp = _darr_grow(sizeof(*arr), arr);\
  if (tmp) {\
    res = true;\
    arr = tmp;\
    arr[_darr_get_data(arr)->count++] = item;\
  }\
  res;\
})

#define darr_pop(arr) _darr_pop_helper(sizeof(*arr), arr)


typedef struct {
  size_t count;
  size_t capacity;
  size_t valsize;
  void* (*alloc)(size_t);
  void* (*realloc)(void*, size_t);
  void (*free)(void*);
  uint8_t flags;
  alignas(max_align_t) char padding[];
} darr_arr;


void darr_free(void* arr);
void* _darr_grow_helper(void* arr, size_t new_cap);
darr_arr* _darr_get_data(void* arr);
void* _darr_new_helper(size_t size, size_t cap, uint8_t flags, void*(*alloc)(size_t), void(*free)(void*), void* (*realloc)(void*, size_t));
bool _darr_get_helper(void* arr, size_t id);
void* _darr_grow(size_t size, void* arr);
void* _darr_pop_helper(size_t size, void* arr);
size_t _darr_cap_helper(void* arr);
size_t _darr_len_helper(void* arr);



#ifdef DA_ARR_IMPLEMENTATION

void darr_free(void* arr) {
  darr_arr* data = _darr_get_data(arr);
  if(data) {
    data->free(data);
  }
}

void* _darr_new_helper(size_t size, size_t cap, uint8_t flags, void*(*alloc)(size_t), void(*free)(void*), void* (*realloc)(void*, size_t)) {
  if (cap > (SIZE_MAX - sizeof(darr_arr)) / size)
    return NULL;
  size_t total = size * cap + sizeof(darr_arr);
  darr_arr* data = alloc(total);
  if (!data) return NULL;
  void* arr = data + 1;
  data->alloc = alloc;
  data->realloc = realloc;
  data->free = free;
  data->capacity = cap;
  data->count = 0;
  data->valsize = size;
  data->flags = flags;
  return arr;
}

darr_arr* _darr_get_data(void* arr) {
  if (!arr) return NULL;
  return (darr_arr*)arr - 1;
}

bool _darr_get_helper(void* arr, size_t id) {
  darr_arr* data = _darr_get_data(arr);
  if (!data) return false;
  size_t bound = (data->flags & CAPACITY_BASED_BOUNDS)
                 ? data->capacity
                 : data->count;
  return id < bound;
}

void* _darr_grow(size_t size, void* arr) {
  if (!arr) {
    arr = _darr_new_helper(size, ARR_DEFAULT_CAPACITY, 0, &malloc, &free, &realloc);
    if (!arr) return NULL;
  }
  darr_arr* data = _darr_get_data(arr);
  
  // GROW
  if (data->count >= data->capacity) {
    size_t new_cap = (data->capacity) ?
                        (data->capacity > SIZE_MAX/2 ?
                          SIZE_MAX
                          : data->capacity*2)
                      : ARR_DEFAULT_CAPACITY;
    void* new_arr = _darr_grow_helper(arr, new_cap);
    if (new_arr) arr = new_arr;
  }
  
  return arr;
}

void* _darr_grow_helper(void* arr, size_t new_cap) {
  darr_arr* data = _darr_get_data(arr);
  if (data->capacity < new_cap) {
    size_t size = data->valsize;
    if (cap > (SIZE_MAX - sizeof(darr_arr)) / size)
      return NULL;
    void* new_data = data->realloc(data, new_cap*size + sizeof(darr_arr));
    if (!new_data) return NULL;
    data = new_data;
    data->capacity = new_cap;
    arr = (char*)data + sizeof(darr_arr);
  }
  return arr;
}

void* _darr_pop_helper(size_t size, void* arr) {
  darr_arr* data = _darr_get_data(arr);
  if (!data) return NULL;
  if (data->count == 0) return NULL;
  data->count -= 1;
  return (char*)arr + size*data->count;
}

size_t _darr_cap_helper(void* arr) {
  darr_arr* data = _darr_get_data(arr);
  if (!data) return 0;
  return data->capacity;
}

size_t _darr_len_helper(void* arr) {
  darr_arr* data = _darr_get_data(arr);
  if (!data) return 0;
  return data->count;
}

#endif
#endif
