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
  ll.len = 0;\
  ll;\
})

#define ll_push(ll, arena, val) ({\
  node* new_node = arena_alloc(arena, sizeof(node));\
  new_node->value = arena_alloc(arena, sizeof(val));\
  new_node->next = NULL;\
  *((typeof(val)*)new_node->value) = val;\
  node* n = ll->head;\
  if (!n) {\
    ll->head = new_node;\
  }\
  else {\
    while(n->next) {\
      n = n->next;\
    }\
    n->next = new_node;\
  }\
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
