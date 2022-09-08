#pragma once


#include <stdlib.h>


typedef struct
{
  char *buff;
  size_t capacity;
  size_t element_size;

  size_t size;
  char *head;
  char *tail;
}LIST_ST;

typedef LIST_ST* LIST;


bool LIST_Empty(const LIST list);
size_t LIST_Size(const LIST list);
size_t LIST_Capaticy(LIST list);

void* LIST_Front(const LIST list);
void* LIST_Back(const LIST list);
void* LIST_At(const LIST list, size_t pos);

bool LIST_PushFront(LIST list, const void *data);
bool LIST_PushBack(LIST list, const void *data);
bool LIST_PopFront(LIST list);
bool LIST_PopBack(LIST list);
bool LIST_Insert(LIST list, size_t pos, const void *data);
bool LIST_Erase(LIST list, size_t pos);

bool LIST_Create(LIST list, void *buff, size_t capacity, size_t element_size);
bool LIST_Destroy(LIST list);
bool LIST_Clear(LIST list);
bool LIST_Traverse(const LIST list, void(*func)(void*));
