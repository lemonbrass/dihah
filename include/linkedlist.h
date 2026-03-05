#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <da_arena.h>
#include <stddef.h>
#include <string.h>

typedef struct {
  void* next;
  void* value;
} node;

typedef struct {
  node* head;
} linked_list;

#define ll_new(type) ({\
  linked_list ll;\
  ll.head = NULL;\
  ll;\
})

// It doesn't allocate new memory for the data, just a new node for the list.
#define ll_push_borrowed(ll, arena, val_ptr) ({ \
  node* _new = arena_alloc(arena, sizeof(node)); \
  _new->value = (val_ptr); \
  _new->next = (ll)->head; \
  (ll)->head = _new; \
})

// Use this for NEW insertions
#define ll_push(ll, arena, val) ({ \
  node* _new = arena_alloc(arena, sizeof(node)); \
  _new->value = arena_alloc(arena, sizeof(val)); \
  *((typeof(val)*)_new->value) = (val); \
  _new->next = (ll)->head; \
  (ll)->head = _new; \
})


#define ll_get(ll_ptr, id) ({          \
    node* n = (ll_ptr)->head;          \
    size_t idx = 0;                    \
    while (n && idx != (id)) {         \
        n = n->next;                   \
        idx++;                         \
    }                                  \
    n ? n->value : NULL;               \
})


#endif
