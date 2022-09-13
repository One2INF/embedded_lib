# embedded_lib
为 嵌入式设备 设计的库，包含常用的 ADT。

## 特点

- 精简
- 不使用堆

## tinyprintf

> 引用自 [mpaland](https://github.com/mpaland) 的 [ptintf](https://github.com/mpaland/printf)

## tinylog



## tinyqueue

```C
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
```

> *考虑使用 `下标索引` 代替 `地址` 进行 `队列头，尾` 运算，增加可读性。*

## tinycircularqueue



## tinylist

```c
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
```

> *注意：提供 `buff` 时，每个元素需要额外增加 `sizeof(NODE)` 大小。如：*
>
> 想要存储 `100` 个 `int` 元素，则需要 `buff` 大小 ```100 * (sizeof(NODE) + sizeof(int))```。
>
> 目前只需额外占用一个指针大小，用以指示下一个节点。

## tinycli



## 说明

- 为保持精简，各接口未作严格的数据安全防护（比如 传参合法性，线程安全），需要调用者自己保证；
- 使用 C99，需要添加编译选项 `-std=c99`。
