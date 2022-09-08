#include "tinylist.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>


typedef struct 
{
  void *next;
  void *data;
}NODE;


bool LIST_Empty(const LIST list)
{
  return 0 == list->size;
}

bool LIST_Full(const LIST list)
{
  return list->size == list->capacity;
}

size_t LIST_Size(const LIST list)
{
  return list->size;
}

void* LIST_Front(const LIST list)
{
  return list->head;
}

void* LIST_Back(const LIST list)
{
  return list->tail;
}

void* LIST_At(const LIST list, size_t pos)
{
  NODE *node = list->head;
  for(size_t i = 0; i < pos - 1; ++i)
  {
    node = node->next;
  }

  return node->data;
}

bool LIST_PushFront(LIST list, const void *data)
{
  if(LIST_Full(list))
    return false;

  NODE *node = list->buff;
  for(size_t i = 0; i < list->capacity; ++i)
  {
    if(node == NULL)
      break;
    node = (char*)node + sizeof(NODE) - sizeof(((NODE*)0)->next) + list->element_size;
  }

  node->next = list->head;
  list->head = node;

  if(list->size == 0)
  {
    list->tail = list->head;
    node->next = node;
  }

  memcpy(list->head, data, list->element_size);
  ++list->size;

  return true;
}

bool LIST_PushBack(LIST list, const void *data)
{
  if(LIST_Full(list))
    return false;

  NODE *node = list->buff;
  for(size_t i = 0; i < list->capacity; ++i)
  {
    if(node == NULL)
      break;
    node = (char*)node + sizeof(NODE) - sizeof(((NODE*)0)->next) + list->element_size;
  }

  node->next = list->head;;
  list->tail = node;

  if(list->size == 0)
  {
    list->head = list->tail;
    node->next = node;
  }

  memcpy(list->tail, data, list->element_size);
  ++list->size;

  return true;
}

bool LIST_PopFront(LIST list)
{
  if(LIST_Empty(list))
    return false;

  NODE *node = list->head;
  list->head = node->next;
  node->next = NULL;

  --list->size;

  return true;   
}

bool LIST_PopBack(LIST list)
{
  if(LIST_Empty(list))
    return false;

  if(list->size > 1)
  {
    NODE *node = list->tail;
    NODE *PreNode = list->head;
    for(size_t i = 0; i < list->size - 1; ++i)
    {
       PreNode = (char*)PreNode + + sizeof(NODE) - sizeof(((NODE*)0)->next) + list->element_size;
    }

    list->tail = ;
    node->next = NULL;
  }

  --list->size;

  return true;   
}

bool LIST_Create(LIST list, void *buff, size_t capacity, size_t element_size)
{
  if(buff == NULL || capacity == 0 || element_size == 0)
    return false;

  list->buff = list->head = list->tail = buff;
  list->size = 0;
  list->capacity = capacity;
  list->element_size = element_size;

  return true;
}

bool LIST_Destroy(LIST list)
{
  list->head = list->tail = list->buff = NULL;
  list->size = 0;
  list->capacity = 0;
  list->element_size = 0;

  return true;
}

bool LIST_Clear(LIST list)
{
  list->head = list->tail = list->buff;
  list->size = 0;

  return true;
}

bool LIST_Traverse(const LIST list, void(*func)(void*))
{
  char *item = list->head;
  for(size_t size = 0; size < list->size; ++size)
  {
    func(item);

    item += list->element_size;
    if(item == list->buff + list->element_size * list->capacity)
      item = list->buff;
  }

  return true;
}
