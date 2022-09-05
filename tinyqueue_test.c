#include "tinyqueue.h"

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>


#define QUEUE_CAPACITY    4

void print_node(void *data)
{
  printf("%d ", *(int*)data);
}

void print_queue_info(QUEUE queue)
{
  printf("\r\n");
  printf("buff address: %p\r\n", queue->buff);
  printf("capacity: %zd\r\n", queue->capacity);
  printf("element size: %zd\r\n", queue->element_size);
  printf("size: %zd\r\n", queue->size);
  printf("head address: %p\r\n", queue->head);
  printf("tail address: %p\r\n", queue->tail);
  printf("\r\n");
}

int main(int argc, char* argv[])
{
  printf("------- queue test -------\r\n");
  int buff[QUEUE_CAPACITY];
  QUEUE_ST queue;

  QUEUE_Create(&queue, buff, QUEUE_CAPACITY, sizeof(int));
  print_queue_info(&queue);

  int a = 1;
  QUEUE_PushBack(&queue, &a);
  a = 2;
  QUEUE_PushBack(&queue, &a);
  a = 3;
  QUEUE_PushBack(&queue, &a);
  a = 4;
  QUEUE_PushBack(&queue, &a);
  a = 5;
  QUEUE_PushBack(&queue, &a);

  QUEUE_Traverse(&queue, print_node);
  printf("\r\n");
  for(int i = 0; i < QUEUE_CAPACITY; ++i)
  {
    if(i % 2) 
      QUEUE_PopBack(&queue);
    else
      QUEUE_PopFront(&queue);

    printf("size %zd: ", QUEUE_Size(&queue));
    QUEUE_Traverse(&queue, print_node);
    printf("\r\n");
  }

  QUEUE_PopFront(&queue);

  a = 6;
  QUEUE_PushFront(&queue, &a);
  a = 7;
  QUEUE_PushFront(&queue, &a);
  a = 8;
  QUEUE_PushFront(&queue, &a);
  a = 9;
  QUEUE_PushFront(&queue, &a);
  a = 0;
  QUEUE_PushFront(&queue, &a);

  QUEUE_Traverse(&queue, print_node);
  printf("\r\n");
  for(int i = 0; i < QUEUE_CAPACITY; ++i)
  {
    QUEUE_PopBack(&queue);
    printf("size %zd: ", QUEUE_Size(&queue));
    QUEUE_Traverse(&queue, print_node);
    printf("\r\n");
  }

  a = 11;
  QUEUE_PushBack(&queue, &a);
  a = 12;
  QUEUE_PushBack(&queue, &a);
  a = 13;
  QUEUE_PushBack(&queue, &a);
  a = 14;
  QUEUE_PushBack(&queue, &a);
  a = 15;
  QUEUE_PushBack(&queue, &a);

  QUEUE_Traverse(&queue, print_node);
  printf("\r\n");

  for(int i = 0; i < QUEUE_CAPACITY; ++i)
  {
    printf("%d ", *(int*)QUEUE_At(&queue, i));
  }
  printf("\r\n");

  return 0;
}
