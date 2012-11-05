#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

// Doubly linked list implementation

typedef struct list_el list_el;
typedef struct list list;


struct list_el {
  void *val;
  list_el *next, *prev;
};

struct list {
  list_el *head, *tail;
  // Whether to free the value pointer, too, when an element is removed
  bool freeValPointer;
};


void listInsert(list *l, void *s);
void listAppend(list *l, void *s);
void listInsertBefore(list *l, list_el *item, void *s);
void listInsertAfter(list *l, list_el *item, void *s);
void listRemove(list *l, list_el *item);
void listRemoveByValue(list *l, void *s);
void listRemoveAllByValue(list *l, void *s);

#endif
