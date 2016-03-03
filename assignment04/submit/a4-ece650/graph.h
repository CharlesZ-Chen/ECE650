#include <stdio.h>
#include <stdlib.h>

typedef struct _SpNode
{
	int d;
	int predecessor;
} SpNode;

typedef struct _Node
{
	int d;
	struct _Node* next;
} Node;

typedef struct _NodeList
{
	Node* head;
} NodeList;

typedef struct _Graph
{
	int V;
	NodeList* array;
} Graph;

Graph* readGraph(char* line);
Graph* initGraph(int V);
void destroyGraph(Graph** graph);
int addEdge(Graph* graph, int src, int dest);
void printGraph(Graph* graph);
