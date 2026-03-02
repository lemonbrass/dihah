#ifndef DA_ARR_H
#define DA_ARR_H

#include<stdlib.h>
#include<stdalign.h>
#include<stddef.h>
#include<stdint.h>
#include<stdbool.h>


#ifndef ARR_DEFAULT_CAPACITY
#define ARR_DEFAULT_CAPACITY 4
#endif


#define DA_FUNC 
#define darr_new(arr, cap) arr = _darr_new_helper(sizeof(*arr), cap)
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

#define darr_free(arr) do { if (arr) free((darr_arr*)arr - 1); arr = NULL; } while (0)

typedef struct {
  size_t count;
  size_t capacity;
  size_t valsize;
  alignas(max_align_t) char padding[];
} darr_arr;



DA_FUNC void* _darr_grow_helper(void* arr, size_t new_cap);
DA_FUNC darr_arr* _darr_get_data(void* arr);
DA_FUNC void* _darr_new_helper(size_t size, size_t cap);
DA_FUNC bool _darr_get_helper(void* arr, size_t id);
DA_FUNC void* _darr_grow(size_t size, void* arr);
DA_FUNC void* _darr_pop_helper(size_t size, void* arr);
DA_FUNC size_t _darr_cap_helper(void* arr);
DA_FUNC size_t _darr_len_helper(void* arr);



#ifdef DA_ARR_IMPLEMENTATION

DA_FUNC void* _darr_new_helper(size_t size, size_t cap) {
  size_t total = size*cap + sizeof(darr_arr);
  if (total > SIZE_MAX / size) return NULL;
  darr_arr* data = malloc(total);
  if (!data) return NULL;
  void* arr = data + 1;
  data->capacity = cap;
  data->count = 0;
  data->valsize = size;
  return arr;
}

DA_FUNC darr_arr* _darr_get_data(void* arr) {
  if (!arr) return NULL;
  return (darr_arr*)arr - 1;
}

DA_FUNC bool _darr_get_helper(void* arr, size_t id) {
  darr_arr* data =_darr_get_data(arr);
  if (!data) return false;
  if (data->capacity <= id) return false;
  return true;
}

DA_FUNC void* _darr_grow(size_t size, void* arr) {
  if (!arr) {
    arr = _darr_new_helper(size, ARR_DEFAULT_CAPACITY);
    if (!arr) return NULL;
  }
  darr_arr* data = _darr_get_data(arr);
  
  // GROW
  if (data->count >= data->capacity) {
    size_t new_cap = (data->capacity) ? data->capacity*2 : ARR_DEFAULT_CAPACITY;
    void* new_arr = _darr_grow_helper(arr, new_cap);
    if (new_arr) arr = new_arr;
  }
  
  return arr;
}

DA_FUNC void* _darr_grow_helper(void* arr, size_t new_cap) {
  darr_arr* data = _darr_get_data(arr);
  if (data->capacity < new_cap) {
    size_t size = data->valsize;
    if (new_cap > SIZE_MAX / size) return NULL;
    void* new_data = realloc(data, new_cap*size + sizeof(darr_arr));
    if (!new_data) return NULL;
    data = new_data;
    data->capacity = new_cap;
    arr = (char*)data + sizeof(darr_arr);
  }
  return arr;
}

DA_FUNC void* _darr_pop_helper(size_t size, void* arr) {
  darr_arr* data = _darr_get_data(arr);
  if (!data) return NULL;
  if (data->count == 0) return NULL;
  data->count -= 1;
  return (char*)arr + size*data->count;
}

DA_FUNC size_t _darr_cap_helper(void* arr) {
  darr_arr* data = _darr_get_data(arr);
  if (!data) return 0;
  return data->capacity;
}

DA_FUNC size_t _darr_len_helper(void* arr) {
  darr_arr* data = _darr_get_data(arr);
  if (!data) return 0;
  return data->count;
}

#endif
#endif
