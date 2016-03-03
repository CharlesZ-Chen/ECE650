#include "graph.h"

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

void printGraph(Graph* graph)
{
	if(graph == NULL)
	{
		fprintf(stderr, "Error: no graph.\n");
		return;
	}
	int i;
	for(i = 0; i < graph->V; i++)
	{
		printf("node %d", i);
		Node* nCrawl = graph->array[i].head;
		while(nCrawl != NULL)
		{
			printf("->%d",nCrawl->d);
			nCrawl = nCrawl->next;
		}
		printf("\n");
	}

}

Graph* readGraph(char* line)
{
	Graph* graph = NULL;
	char c;
	int V;
	sscanf(line," %c %d ", &c, &V);
	if (c != 'V' || V < 0)
	{
		fprintf(stderr, "Error: error input of V.\n");
		return NULL;
	}
	int leftN, rightN;
	graph = initGraph(V);
	int scanResult;
	scanResult = scanf( " E {");
	while(1)
	{
		scanResult = scanf("<%d,%d>,", &leftN, &rightN);
		if (scanResult < 2)
			break;
		if (!addEdge(graph, leftN, rightN))
		{
			destroyGraph(&graph);
			break;
		}
			
	}
	size_t bytes = 0;
	getline(&line, &bytes, stdin);
	return graph;
}
