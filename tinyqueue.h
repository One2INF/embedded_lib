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


bool QUEUE_Empty(QUEUE queue);
size_t QUEUE_Size(QUEUE queue);
size_t QUEUE_Capaticy(QUEUE queue);

void* QUEUE_Front(QUEUE queue);
void* QUEUE_Back(QUEUE queue);
void* QUEUE_At(QUEUE queue, size_t pos);

bool QUEUE_PushFront(QUEUE queue, void *data);
bool QUEUE_PushBack(QUEUE queue, void *data);
bool QUEUE_PopFront(QUEUE queue);
bool QUEUE_PopBack(QUEUE queue);

bool QUEUE_Create(QUEUE queue, void *buff, size_t capacity, size_t element_size);
bool QUEUE_Destroy(QUEUE queue);
bool QUEUE_Traverse(QUEUE queue, void(*func)(void*));
bool QUEUE_Clear(QUEUE queue);
