#include "tinyqueue.h"

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

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

bool PushBackTest(QUEUE queue, int test_data_buff[])
{
  int idx;
  for(idx = 0 ; idx < queue->capacity; ++idx)
  {
    QUEUE_PushBack(queue, &test_data_buff[idx]);
  }

  for(idx = 0; idx < queue->capacity; ++idx)
  {
    if(*(int*)QUEUE_At(queue, idx) != test_data_buff[idx])
      return true;
  }

  for(idx = 0; idx < queue->capacity; ++idx)
  {
    if(*(int*)QUEUE_Back(queue) != test_data_buff[queue->capacity - idx - 1])
      return true;

    if(*(int*)QUEUE_Front(queue) != test_data_buff[0])
      return true;

    QUEUE_PopBack(queue);
  }

  return false;
}

bool PushFrontTest(QUEUE queue, int test_data_buff[])
{
  int idx;
  for(idx = 0 ; idx < queue->capacity; ++idx)
  {
    QUEUE_PushFront(queue, &test_data_buff[idx]);
  }

  for(idx = 0; idx < queue->capacity; ++idx)
  {
    if(*(int*)QUEUE_At(queue, idx) != test_data_buff[queue->capacity - idx - 1])
      return true;
  }

  for(idx = 0; idx < queue->capacity; ++idx)
  {
    if(*(int*)QUEUE_Front(queue) != test_data_buff[queue->capacity - idx - 1])
      return true;

    if(*(int*)QUEUE_Back(queue) != test_data_buff[0])
      return true;

    QUEUE_PopFront(queue);
  }
  
  return false;
}

int main(int argc, char* argv[])
{
  printf("------- queue test -------\r\n");
  FILE *fp;
  char *file;

  if(argc == 1)
  {
    file = "./queue_test_data.txt";
  }
  else if(argc == 2)
  {
    file = argv[1];
  }
  else
  {
    printf("Usage: %s filename\r\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  if((fp = fopen(file, "r")) == NULL)
  {
    printf("Can't open %s\r\n", file);
    exit(EXIT_FAILURE);
  }

  int test_number;
  size_t capacity;
  size_t element_size = sizeof(int);
  fscanf(fp, "%d %zd", &test_number, &capacity);

  QUEUE_ST queue;
  int queue_buff[capacity];
  QUEUE_Create(&queue, queue_buff, element_size, capacity);

  for(int i = 0; i < test_number; ++i)
  {
    int test_data_buff[queue.capacity];
    for(int idx = 0; idx < queue.capacity; ++idx)
      fscanf(fp, "%d", &test_data_buff[idx]);

    if(PushBackTest(&queue, test_data_buff) ||
       PushFrontTest(&queue, test_data_buff))
      goto test_failed;
  }

  printf("test passed!\n");
  return 0;

test_failed:
  printf("test failed!\n");
  return 1;
}
