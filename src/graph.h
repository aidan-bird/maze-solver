/* aidan bird 2021 */
#ifndef GRAPH_H
#define GRAPH_H

#include "png_utils.h"
#include <stdint.h>
#include <png.h>

typedef struct GraphNode GraphNode;
typedef Img Maze;
typedef struct MazeGraph MazeGraph;
typedef struct VecPath VecPath;
typedef struct DijkstraNode DijkstraNode;

struct VecPath
{
    size_t count;
    DijkstraNode **start;
};

struct DijkstraNode
{
    int dist;
    GraphNode *parent;
    GraphNode *node;
};

typedef enum Direction {
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3
} Direction;

struct GraphNode
{
    int isVisited;
    int parentWeight;
    GraphNode *parent;
    int32_t x;
    int32_t y;
    GraphNode *children[4];
    int weight[4];
};

struct MazeGraph
{
    GraphNode *start;
    GraphNode **nodevec;
    GraphNode **nodemat;
    size_t nodeCount;
};

size_t sumEdges(int32_t x, int32_t y, Maze *m);
GraphNode *walkPath(int x, int y, Direction d, Maze *m, GraphNode **freeNodes, GraphNode *parent, MazeGraph *mg);
MazeGraph *mkGraph(Maze *m, short startx, short starty);
GraphNode *forkPath(int x, int y, Direction d, Maze *m, GraphNode **freeNodes, GraphNode *parent, MazeGraph *mg);
VecPath *dijkstra(GraphNode *goal, MazeGraph *mg, Maze *m);

#endif
