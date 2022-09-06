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
bool QUEUE_Clear(QUEUE queue)
```

> *考虑使用下标索引，代替地址进行 队列头尾 运算，增加可读性。*

## tinycircularqueue



## tinylist



## tinycli



## 说明

- 为保持精简，各接口未作严格的数据安全防护，需要调用者自己保证。
- 使用 C99，需要添加编译选项 `-std=c99`.
