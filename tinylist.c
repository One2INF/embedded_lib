#include "tinylist.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>


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
  return list->head->data;
}

void* LIST_Back(const LIST list)
{
  return list->tail->data;
}

void* LIST_At(const LIST list, size_t pos)
{
  NODE *node = list->head;
  for(size_t i = 0; i < pos; ++i)
  {
    node = node->next;
  }

  return node->data;
}

bool LIST_PushFront(LIST list, const void *data)
{
  if(LIST_Full(list))
    return false;

  NODE *node = (NODE*)list->buff;
  for(size_t i = 0; i < list->capacity; ++i)
  {
    if(node->next == NULL)
      break;
    node = (NODE*)((char*)node + sizeof(NODE) + list->element_size);
  }

  if(list->size == 0)
  {
    node->next = node;
    list->head = list->tail = node;
  }
  else
  {
    node->next = list->head;
    list->head = node;
  }

  memcpy(node->data, data, list->element_size);
  ++list->size;

  return true;
}

bool LIST_PushBack(LIST list, const void *data)
{
  if(LIST_Full(list))
    return false;

  NODE *node = (NODE*)list->buff;
  for(size_t i = 0; i < list->capacity; ++i)
  {
    if(node->next == NULL)
      break;
    node = (NODE*)((char*)node + sizeof(NODE) + list->element_size);
  }

  if(list->size == 0)
  {
    list->head = list->tail = node;
  }
  else
  {
    list->tail->next = node;
    list->tail = node;
  }

  node->next = node;
  memcpy(node->data, data, list->element_size);
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

  NODE *node = list->tail;
  NODE *PreNode = list->head;
  for(size_t i = 0; i + 2 < list->size; ++i)
  {
    PreNode = PreNode->next;
  }

  list->tail = PreNode;
  node->next = NULL;

  --list->size;

  return true;   
}

bool LIST_Create(LIST list, void *buff, size_t capacity, size_t element_size)
{
  if(buff == NULL || capacity == 0 || element_size == 0)
    return false;

  list->buff = buff;
  list->head = list->tail = buff;
  list->size = 0;
  list->capacity = capacity;
  list->element_size = element_size;

  memset(buff, 0, capacity*(sizeof(NODE)+element_size));

  return true;
}

bool LIST_Destroy(LIST list)
{
  list->buff = NULL;
  list->head = list->tail = NULL;
  list->size = 0;
  list->capacity = 0;
  list->element_size = 0;

  return true;
}

bool LIST_Clear(LIST list)
{
  list->head = list->tail = (NODE*)list->buff;
  list->size = 0;

  return true;
}

bool LIST_Traverse(const LIST list, void(*func)(void*))
{
  char *item = (char*)list->head;
  for(size_t size = 0; size < list->size; ++size)
  {
    func(((NODE*)item)->data);
    item = ((NODE*)item)->next;
  }

  return true;
}
