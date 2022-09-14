#include "tinylist.h"

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define LIST_CAPACITY    4

void print_node(void *data)
{
  printf("%d ", *(int*)data);
}

void print_list_info(LIST list)
{
  printf("\r\n");
  printf("buff address: %p\r\n", list->buff);
  printf("capacity: %zd\r\n", list->capacity);
  printf("element size: %zd\r\n", list->element_size);
  printf("size: %zd\r\n", list->size);
  printf("head address: %p\r\n", list->head);
  printf("tail address: %p\r\n", list->tail);
  printf("\r\n");
}

bool PushBackTest(LIST list, int test_data_buff[])
{
  int idx;
  for(idx = 0 ; idx < list->capacity; ++idx)
  {
    LIST_PushBack(list, &test_data_buff[idx]);
  }

  for(idx = 0; idx < list->capacity; ++idx)
  {
    if(*(int*)LIST_At(list, idx) != test_data_buff[idx])
      return true;
  }

  for(idx = 0; idx < list->capacity; ++idx)
  {
    if(*(int*)LIST_Back(list) != test_data_buff[list->capacity - idx - 1])
      return true;

    if(*(int*)LIST_Front(list) != test_data_buff[0])
      return true;

    LIST_PopBack(list);
  }

  return false;
}

bool PushFrontTest(LIST list, int test_data_buff[])
{
  int idx;
  for(idx = 0 ; idx < list->capacity; ++idx)
  {
    LIST_PushFront(list, &test_data_buff[idx]);
  }

  for(idx = 0; idx < list->capacity; ++idx)
  {
    if(*(int*)LIST_At(list, idx) != test_data_buff[list->capacity - idx - 1])
      return true;
  }

  for(idx = 0; idx < list->capacity; ++idx)
  {
    if(*(int*)LIST_Front(list) != test_data_buff[list->capacity - idx - 1])
      return true;

    if(*(int*)LIST_Back(list) != test_data_buff[0])
      return true;

    LIST_PopFront(list);
  }
  
  return false;
}

bool InsertTest(LIST list, int test_data_buff[])
{
  int idx;
  LIST_PushFront(list, &test_data_buff[0]);
  for(idx = 1 ; idx < list->capacity; ++idx)
  {
    LIST_Insert(list, list->size - 1, &test_data_buff[idx]);
  }

  for(idx = 0; idx < list->capacity; ++idx)
  {
    if(*(int*)LIST_At(list, idx) != test_data_buff[idx])
      return true;
  }

  for(idx = 0; idx < list->capacity; ++idx)
  {
    if(*(int*)LIST_Back(list) != test_data_buff[list->capacity - idx - 1])
      return true;

    if(*(int*)LIST_Front(list) != test_data_buff[0])
      return true;

    LIST_Erase(list, list->size - 1);
  }
  
  return false;
}

int main(int argc, char* argv[])
{
  printf("------- list test -------\r\n");
  FILE *fp;
  char *file;

  if(argc == 1)
  {
    file = "./list_test_data.txt";
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

  LIST_ST list;
  char list_buff[capacity * (sizeof(NODE) + element_size)];
  LIST_Create(&list, list_buff, element_size, capacity);

  for(int i = 0; i < test_number; ++i)
  {
    int test_data_buff[list.capacity];
    for(int idx = 0; idx < list.capacity; ++idx)
      fscanf(fp, "%d", &test_data_buff[idx]);

    if(PushBackTest(&list, test_data_buff) ||
       PushFrontTest(&list, test_data_buff) ||
       InsertTest(&list, test_data_buff))
      goto test_failed;
  }

  printf("test passed!\n");
  return 0;

test_failed:
  printf("test failed!\n");
  return 1;
}
