/* aidan bird 2021 */
#include "graph.h"
#include "heap.h"
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#define WHITE 0xffffffff

size_t
sumEdges(int32_t x, int32_t y, Maze *m)
{
    static int32_t vecX[] = { 0, 0, -1, 1};
    static int32_t vecY[] = { -1, 1, 0, 0};
    size_t i;
    size_t sum;

    sum = 0;
    for (i = 0; i < 4; i++) {
        if (x + vecX[i] < 0 || x + vecX[i] >= m->w
            || y + vecY[i] < 0 || y + vecY[i] >= m->h)
            continue;
        if (((uint32_t **)m->grid)[y + vecY[i]][x + vecX[i]] == WHITE)
            sum++;
    }
    return sum;
}

GraphNode *
walkPath(int x, int y, Direction d, Maze *m,
    GraphNode **freeNodes, GraphNode *parent, MazeGraph *mg) {
    static int vecMoveX[] = { 0, 0, -1, 1};
    static int vecMoveY[] = { -1, 1, 0, 0};
    int h;
    int k;

    h = x + vecMoveX[d];
    k = y + vecMoveY[d];
    while (1) {
        if (h < 0 || h >= m->w || k < 0 || k >= m->h 
        || ((uint32_t **)m->grid)[k][h] != WHITE)
            break;
        if (sumEdges(h, k, m) >= 3)
            return forkPath(h, k, d, m, freeNodes, parent, mg);
        h += vecMoveX[d];
        k += vecMoveY[d];
    } 
    h -= vecMoveX[d];
    k -= vecMoveY[d];
    if (h == x && k == y)
        return NULL;
    return forkPath(h, k, d, m, freeNodes, parent, mg);
}

GraphNode *
forkPath(int x, int y, Direction d, Maze *m,
    GraphNode **freeNodes, GraphNode *parent, MazeGraph *mg) {
    size_t i;
    Direction p;

    switch (d) {
        case UP:
            p = DOWN;
            break;
        case DOWN:
            p = UP;
            break;
        case LEFT:
            p = RIGHT;
            break;
        case RIGHT:
            p = LEFT;
            break;
    }
    /* BASE CASE */ 
    if (freeNodes[y][x].isVisited) {
        freeNodes[y][x].children[p] = parent;
        freeNodes[y][x].weight[p] = abs(freeNodes[y][x].x - parent->x
            + freeNodes[y][x].y - parent->y);
        return &freeNodes[y][x];
    }
    freeNodes[y][x].isVisited = 1;
    freeNodes[y][x].x = x;
    freeNodes[y][x].y = y;
    freeNodes[y][x].parent = parent;
    freeNodes[y][x].children[p] = parent;
    mg->nodevec[mg->nodeCount] = &freeNodes[y][x];
    mg->nodeCount++;
    for (i = 0; i < 4; i++) {
        if (i == p)
            continue;
        freeNodes[y][x].children[i] = walkPath(x, y, i, m, freeNodes, &freeNodes[y][x], mg);
        if (freeNodes[y][x].children[i] != NULL) {
            freeNodes[y][x].weight[i] = abs(freeNodes[y][x].children[i]->x - x
                + freeNodes[y][x].children[i]->y - y);
            freeNodes[y][x].children[i]->parentWeight = freeNodes[y][x].weight[i];
        }
    }
    return &freeNodes[y][x];
}

MazeGraph *
mkGraph(Maze *m, short startx, short starty)
{
    MazeGraph *mg;
    Direction startDir;
    size_t i;

    // possible segfault due to reversed dims
    if (!(mg = calloc(sizeof(MazeGraph) + sizeof(GraphNode *) * (size_t)m->h 
        + sizeof(GraphNode *) * m->h * m->w
        + sizeof(GraphNode) * m->w * m->h, 1)))
        return NULL;
    mg->nodemat = (void *)mg + sizeof(MazeGraph);
    mg->nodevec = (void *)mg + sizeof(MazeGraph) + sizeof(GraphNode *) * m->h;
    mg->nodeCount = 0;
    for (i = 0; i < m->h; i++)
        mg->nodemat[i] = (void *)mg + sizeof(MazeGraph)
        + sizeof(GraphNode *) * m->h + sizeof(GraphNode *) * m->h * m->w
        + sizeof(GraphNode) * m->w * i; 
    if (!starty && startx > 0 && startx < m->w)
        startDir = DOWN;
    else if (starty == m->h && startx > 0 && startx < m->w)
        startDir = UP;
    else if (startx == 0 && starty > 0 && starty < m->h)
        startDir = RIGHT;
    else
        startDir = LEFT;
    /* TODO: reduce parameter count */
    mg->start = forkPath(startx, starty, startDir, m, mg->nodemat, NULL, mg);
    return mg;
}

VecPath *
dijkstra(GraphNode *goal, MazeGraph *mg, Maze *m)
{
    VecPath *ret;
    BinaryMinHeap *heap;
    BinaryMinHeapNode tmp;
    BinaryMinHeapNode test;
    DijkstraNode **dist;
    DijkstraNode *previousNode;
    size_t i;
    size_t j;

    if(!(heap = newBinaryMinHeap((size_t)log2(mg->nodeCount + 1) - 1)))
        return NULL;
    if(!(ret = malloc(sizeof(DijkstraNode *) * m->h
        + sizeof(DijkstraNode) * m->h * m->w
        + sizeof(DijkstraNode *) * m->h * m->w))) {
        free(heap);
        return NULL;
    }
    ret->start = (void *)ret + sizeof(VecPath);
    dist = (void *)ret + sizeof(VecPath) + sizeof(DijkstraNode *) * m->h * m->w;
    for (i = 0; i < m->h; i++) {
        dist[i] = (void *)dist + sizeof(DijkstraNode *) * m->h
        + sizeof(DijkstraNode) * m->w * i;
        for (j = 0; j < m->w; j++) {
            dist[i][j].dist = INT_MAX;
        }
    }
    for (i = 1; i < mg->nodeCount; i++)
        mg->nodevec[i]->isVisited = 0;
    tmp.key = 0;
    mg->start->isVisited = 0;
    tmp.value = mg->start;
    dist[mg->start->y][mg->start->x].parent = NULL;
    dist[mg->start->y][mg->start->x].node = mg->start;
    dist[mg->start->y][mg->start->x].dist = 0;
    binaryMinHeapInsert(tmp, heap);
    // TODO: terminate loop when end node is detected
    while (heap->count) {
        test = binaryMinHeapPop(heap);
        for (i = 0; i < 4; i++) {
            if (!(((GraphNode *)test.value)->children[i])
                || ((GraphNode *)test.value)->children[i]->isVisited) {
                if (goal->x == ((GraphNode *)test.value)->x
                    && goal->y == ((GraphNode *)test.value)->y)
                    goto exit_success;
                continue;
            }
            tmp.key = ((GraphNode *)test.value)->weight[i] + test.key;
            tmp.value = ((GraphNode *)test.value)->children[i];
            // possible error due to mismatched dims
            // TODO: clean this up
            if (tmp.key < dist[((GraphNode *)test.value)->children[i]->y]
                [((GraphNode *)test.value)->children[i]->x].dist) {
                dist[((GraphNode *)test.value)->children[i]->y]
                    [((GraphNode *)test.value)->children[i]->x].parent
                    = test.value;
                dist[((GraphNode *)test.value)->children[i]->y]
                    [((GraphNode *)test.value)->children[i]->x].node
                    = tmp.value;
                dist[((GraphNode *)test.value)->children[i]->y]
                    [((GraphNode *)test.value)->children[i]->x].dist
                    = tmp.key;
            }
            if (goal->x == ((GraphNode *)test.value)->x
                && goal->y == ((GraphNode *)test.value)->y)
                goto exit_success;
            ((GraphNode *)test.value)->children[i]->isVisited = 1;
            binaryMinHeapInsert(tmp, heap);
        }
    }
/* exit_failure */ 
    free(heap);
    return NULL;
exit_success:;
    i = 0;
    previousNode = &dist[goal->y][goal->x];
    while (previousNode->parent) {
        ret->start[i] = previousNode;
        previousNode = &dist[previousNode->parent->y][previousNode->parent->x];
        i++;
    }
    ret->start[i] = &dist[mg->start->y][mg->start->x];
    ret->count = i + 1;
    free(heap);
    return ret;
}

