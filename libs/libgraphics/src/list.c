#include <list.h>
#include <stdlib.h>

void listInsert(list *l, void *s)
{
  list_el *el = (list_el*) malloc(sizeof(list_el));
  *el = (list_el) { s, l->head, NULL };
  if (l->head)
    l->head->prev = el;
  l->head = el;
}
void listAppend(list *l, void *s)
{
  list_el *el = (list_el*) malloc(sizeof(list_el));
  *el = (list_el) { s, NULL, l->head };
  if (l->tail)
    l->tail->next = el;
  l->tail = el;
}
void listInsertBefore(list *l, list_el *item, void *s)
{
  list_el *el = (list_el*) malloc(sizeof(list_el));
  *el = (list_el) { s, item->prev, item };
  item->prev = el;
  if (!item->prev)
    l->head = el;
}
void listInsertAfter(list *l, list_el *item, void *s)
{
  list_el *el = (list_el*) malloc(sizeof(list_el));
  *el = (list_el) { s, item, item->next };
  item->next = el;
  if (!item->next)
    l->tail = el;
}
void listRemove(list *l, list_el *item)
{
  if (item->prev)
    item->prev->next = item->next;
  else
    l->head = item->next;

  if (item->next)
    item->next->prev = item->prev;
  else
    l->tail = item->prev;

  if (l->freeValPointer)
    free(item->val);
  free(item);
}
void listRemoveByValue(list *l, void *s)
{
  list_el *t, *i = l->head;
  while (i)
  {
    t = i->next;
    if (i->val == s)
    {
      listRemove(l, i);
      return;
    }
    i = t;
  }
}
void listRemoveAllByValue(list *l, void *s)
{
  list_el *t, *i = l->head;
  while (i)
  {
    t = i->next;
    if (i->val == s)
      listRemove(l, i);
    i = t;
  }
}
