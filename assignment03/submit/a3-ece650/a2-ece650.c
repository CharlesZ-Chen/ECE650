/***first completed version,  implement single-source-SP by using BellmanFord Algorithm
*special cases:
* 1. V 0 is vaild, in which only E = {} is vaild edge set
* 2. self shortest path finding is vaild, e.g.  input s 0 0 will output 0-0
*/
#include <stdio.h>
#include <stdlib.h>

#ifndef INFINITE
#define INFINITE 999999999
#endif

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

Graph* initGraph(int V)
{
	Graph* graph = (Graph*) malloc(sizeof(Graph));
	graph->V = V;
	if( V == 0)
	{
		graph->array = NULL;
		return graph;
	}
	graph->array = (NodeList*) malloc(V*sizeof(NodeList));
	int i;
	for(i = 0; i < V; i++) {
		graph->array[i].head = NULL;
	}
	return graph;
}

void destroyGraph(Graph** graph)
{
	if (*graph == NULL)
		return;
	int i;
	for(i = 0; i < (*graph)->V; i++)
	{
		Node* head = (*graph)->array[i].head;
		while(head != NULL)
		{
			Node* next = head->next;
			free(head);
			head = next;
		}
	}
	free((*graph)->array);
	free(*graph);
	*graph = NULL;
}

int addEdge(Graph* graph, int src, int dest)
{
	if(src >= graph->V || dest >= graph->V)
	{
		fprintf(stderr, "Error: Unexisted Vertex in edge<%d,%d>\n", src, dest);
		return 0;
	}
	if (src == dest)
		return 1;
	//add src->dest
	Node* newNode = (Node*) malloc(sizeof(Node));
	newNode->d = dest;
	newNode->next = graph->array[src].head;
	graph->array[src].head = newNode;

	//add dest->src
	newNode = (Node*) malloc(sizeof(Node));
	newNode->d = src;
	newNode->next = graph->array[dest].head;
	graph->array[dest].head = newNode;
	return 1;
}

SpNode* initSingleSource(int V, int src)
{
	int v;
	//shortest path list for Node src
	SpNode* spArray = (SpNode*) malloc(V * sizeof(SpNode));
	for (v = 0; v < V; v++) {

		spArray[v].d = INFINITE;
		spArray[v].predecessor = -1;
	}
	spArray[src].d = 0;

	return spArray;
}

//relax edge<u,v>
void relax(int uIndex, SpNode* u, SpNode* v)
{
	if(v->d > (u->d + 1)){
		v->d = u->d + 1;
		v->predecessor = uIndex;
	}
}

SpNode* bellmanFord(Graph* graph, int src)
{
	SpNode* spArray = initSingleSource(graph->V, src);
	int i;
	int v;
	//iterate this loop in V - 1 times
	for(i = 0; i < graph->V; i++) {
		//traverse edges in the graph
		for(v = 0; v < graph->V; v++) {
			Node* pCrawl = graph->array[v].head;
			while(pCrawl) {
				//pCrawl->d means destination of edge<v,d>
				relax(v, &spArray[v], &spArray[pCrawl->d]);
				pCrawl = pCrawl->next;
			}
		}
	}
	return spArray;
}

void printSPath(SpNode* spArray ,int src, int dest)
{
	if(spArray[dest].predecessor == -1)
	{
		printf("%d", src);
		return;
	}
	printSPath(spArray, src, spArray[dest].predecessor);
	printf("-%d", dest);
}

//deal with the special case and error procession
void printSpathWrapper( Graph* graph, int src, int dest)
{
	if (src >= graph->V || dest >= graph->V || src < 0 || dest < 0)
	{
		fprintf(stderr, "Error: specified not existed Vertex in \'s\' command.\n");
		return;	
	}
	
	if(src == dest)
	{
		printf("%d-%d\n", src, dest);
		return;
	}
	SpNode* spArray = bellmanFord(graph, src);
		
	if(spArray[dest].d == INFINITE)
		fprintf(stderr, "Error: Path not existed between %d and %d\n", src, dest);
	else
	{
		printSPath(spArray, src, dest);
		printf("\n");
	}
	free(spArray);
}

int main(int argc, char const *argv[])
{
	Graph *graph = NULL;
	SpNode *spArray = NULL;
	int scanResult;
	/* code */  
	while(1)
	{
		char command;
		int V;
		int leftN;
		int rightN;
		scanResult = scanf(" %c", &command);
		if (scanResult == EOF)
			break;
		if (scanResult < 1)
		{
			fprintf(stderr, "Error:Unexpected input while waiting next command.\n");
			continue;
		}
		if (command == 'V')
		{
			scanf(" %d", &V);
			if( V < 0)
			{
				fprintf(stderr, "Error: invalid number of Vertex.\n");
				continue;
			}
			if(graph != NULL)
				destroyGraph(&graph);
			graph = initGraph(V);

			//read E
			scanResult = scanf( " E {");
			while(1)
			{
				scanResult = scanf("<%d,%d>,", &leftN, &rightN);
				if (scanResult < 2)
					break;
				if (!addEdge(graph, leftN, rightN))
					break;
				// scanf(",");
			}
			scanResult = scanf("}");
		} else if( command == 's')
		{
			int src, dest;
			scanResult = scanf(" %d %d", &src, &dest);
			if (graph == NULL) {
				fprintf(stderr, "Error: please input V,E before input s %%d %%d.\n");
				continue;
			}
			if (scanResult == 2)
				printSpathWrapper(graph, src, dest);
			else {
				fprintf(stderr, "Error: Inccort input during s command.\n");
				continue;
			}
		}
	}
	return 0;
}

