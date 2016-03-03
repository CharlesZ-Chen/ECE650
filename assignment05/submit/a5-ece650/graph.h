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

/*read graph from indicated stream*/
Graph* readGraph(FILE* stream);
void printGraph(Graph* graph);
Graph* initGraph(int V);
void destroyGraph(Graph** graph);
Graph* copyGraph(Graph* srcG);

int addEdge(Graph* graph, int src, int dest);
void deleteEdge(Graph* graph, int src, int dest);
void _deleteNode(NodeList &nl, int key);

int getHstDgrVid(Graph* graph);
int _getVidDgr(NodeList nl);
//return the next edge by increasing order
int getNextEdge(Graph* graph);
void deleteVidEdges(Graph* graph, int vId);
