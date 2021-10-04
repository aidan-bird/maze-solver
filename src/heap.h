/* aidan bird 2021 */
#ifndef HEAP_H
#define HEAP_H

#include <stddef.h>

typedef struct BinaryMinHeap BinaryMinHeap;
typedef struct BinaryMinHeapNode BinaryMinHeapNode;

struct BinaryMinHeapNode
{
    int key;
    void *value;
};

struct BinaryMinHeap
{
    size_t maxHeight;
    size_t height;
    size_t count;
    BinaryMinHeapNode *arr;
};

BinaryMinHeap *newBinaryMinHeap(size_t height);

void binaryMinHeapInsert(BinaryMinHeapNode x, BinaryMinHeap *heap);

BinaryMinHeapNode binaryMinHeapPop(BinaryMinHeap *heap);

#endif
