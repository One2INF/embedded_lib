#include "tinyqueue.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>


bool QUEUE_Empty(QUEUE queue)
{
  return 0 == queue->size;
}

bool QUEUE_Full(QUEUE queue)
{
  return queue->size == queue->capacity;
}

size_t QUEUE_Size(QUEUE queue)
{
  return queue->size;
}

void* QUEUE_Front(QUEUE queue)
{
  return queue->head;
}

void* QUEUE_Back(QUEUE queue)
{
  return queue->tail;
}

void* QUEUE_At(QUEUE queue, size_t pos)
{
  void *item;
  if(queue->head + pos * queue->element_size > queue->buff + queue->element_size * (queue->capacity - 1))
    item = queue->head - queue->element_size * (queue->capacity - pos);
  else
    item = queue->head + pos * queue->element_size;

  return item;
}

bool QUEUE_PushFront(QUEUE queue, void *data)
{
  if(QUEUE_Full(queue))
    return false;

  if(queue->head == queue->buff)
    queue->head = queue->buff + queue->element_size * (queue->capacity - 1);
  else
    queue->head = queue->head - queue->element_size;

  if(queue->size == 0)
    queue->tail = queue->head;

  memcpy(queue->head, data, queue->element_size);
  ++queue->size;

  return true;
}

bool QUEUE_PushBack(QUEUE queue, void *data)
{
  if(QUEUE_Full(queue))
    return false;

  if(queue->tail == queue->buff + queue->element_size * (queue->capacity - 1))
    queue->tail = queue->buff;
  else
    queue->tail = queue->tail + queue->element_size;

  if(queue->size == 0)
    queue->head = queue->tail;

  memcpy(queue->tail, data, queue->element_size);
  ++queue->size;

  return true;
}

bool QUEUE_PopFront(QUEUE queue)
{
  if(QUEUE_Empty(queue))
    return false;

  if(queue->head == queue->buff + queue->element_size * (queue->capacity - 1))
    queue->head = queue->buff;
  else
    queue->head = queue->head + queue->element_size;

  --queue->size;

  return true;   
}

bool QUEUE_PopBack(QUEUE queue)
{
  if(QUEUE_Empty(queue))
    return false;

  if(queue->tail == queue->buff)
    queue->tail = queue->buff + queue->element_size * (queue->capacity - 1);
  else
    queue->tail = queue->tail - queue->element_size;

  --queue->size;

  return true;   
}

bool QUEUE_Create(QUEUE queue, void *buff, size_t capacity, size_t element_size)
{
  if(buff == NULL || capacity == 0 || element_size == 0)
    return false;

  queue->buff = queue->head = queue->tail = buff;
  queue->size = 0;
  queue->capacity = capacity;
  queue->element_size = element_size;

  return true;
}

bool QUEUE_Destroy(QUEUE queue)
{
  return true;
}

bool QUEUE_Traverse(QUEUE queue, void(*func)(void*))
{
  char *item = queue->head;
  for(size_t size = 0; size < queue->size; ++size)
  {
    func(item);

    item += queue->element_size;
    if(item == queue->buff + queue->element_size * queue->capacity)
      item = queue->buff;
  }

  return true;
}

bool QUEUE_Clear(QUEUE queue)
{
  queue->head = queue->tail = queue->buff;
  queue->size = 0;

  return true;
}
