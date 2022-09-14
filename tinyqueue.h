#pragma once


#include <stddef.h>
#include <stdbool.h>


typedef struct
{
  char *buff;
  size_t capacity;
  size_t element_size;

  size_t size;
  char *head;
  char *tail;
}QUEUE_ST;

typedef QUEUE_ST* QUEUE;


bool QUEUE_Empty(const QUEUE queue);
size_t QUEUE_Size(const QUEUE queue);
size_t QUEUE_Capaticy(QUEUE queue);

void* QUEUE_Front(const QUEUE queue);
void* QUEUE_Back(const QUEUE queue);
void* QUEUE_At(const QUEUE queue, size_t pos);

bool QUEUE_PushFront(QUEUE queue, const void *data);
bool QUEUE_PushBack(QUEUE queue, const void *data);
bool QUEUE_PopFront(QUEUE queue);
bool QUEUE_PopBack(QUEUE queue);

bool QUEUE_Create(QUEUE queue, void *buff, size_t element_size, size_t capacity);
bool QUEUE_Destroy(QUEUE queue);
bool QUEUE_Clear(QUEUE queue);
bool QUEUE_Traverse(const QUEUE queue, void(*func)(void*));
