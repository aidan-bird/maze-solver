/* aidan bird 2021 */
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>

#include "heap.h"

#define binaryMinHeapTop(X) X->arr[0];

static inline void
swap(BinaryMinHeapNode *x, BinaryMinHeapNode *y)
{
    BinaryMinHeapNode z = *x;
    *x = *y;
    *y = z;
}

static inline size_t
min(BinaryMinHeapNode *x, size_t xi, BinaryMinHeapNode *y, size_t yi)
{
    return x->key < y->key ? xi : yi;
}

BinaryMinHeap *
newBinaryMinHeap(size_t h) 
{
    BinaryMinHeap *ret;

    if (!(ret = malloc(sizeof(BinaryMinHeap)
        + sizeof(BinaryMinHeapNode) * ((size_t)pow(2, h + 1) - 1)))) {
        return NULL;
    }
    memset((void *)ret + sizeof(BinaryMinHeap), INT_MAX,
        sizeof(BinaryMinHeapNode) * ((size_t)pow(2, h + 1) - 1));
    ret->height = h;
    ret->count = 0;
    ret->arr = (void *)ret + sizeof(BinaryMinHeap);
    return ret;
}

void
binaryMinHeapInsert(BinaryMinHeapNode x, BinaryMinHeap *heap) {
    size_t lastPos;
    size_t nextPos;

    lastPos = heap->count;
    heap->arr[heap->count] = x;
    nextPos = heap->count;
    heap->count++;
    while (1) {
        nextPos /= 2;
        if (x.key < heap->arr[nextPos].key) {
            swap(&(heap->arr[nextPos]),
                &(heap->arr[lastPos]));
            lastPos = nextPos;
        }
        else
            break;
    }
}

BinaryMinHeapNode
binaryMinHeapPop(BinaryMinHeap *heap)
{
    BinaryMinHeapNode ret;
    BinaryMinHeapNode back;
    size_t test;
    size_t i;
    size_t lastPos;

    ret = heap->arr[0];
    swap(heap->arr, heap->arr + heap->count - 1);
    heap->count--;
    i = 1;
    lastPos = 0;
    back = heap->arr[0];
    while (i < heap->count) {
        if (i + 1 < heap->count)
            test = min(heap->arr + i, i, heap->arr + i + 1, i + 1);
        else
            test = i;
        if (back.key > heap->arr[test].key) {
            swap(heap->arr + lastPos, heap->arr + test);
            i = (1 + test) * 2 - 1;
            lastPos = test;
        }
        else
            break;
    }
    return ret;
}

