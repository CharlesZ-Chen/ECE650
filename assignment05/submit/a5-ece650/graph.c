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

Graph* readGraph(FILE* stream)
{
	Graph* graph = NULL;
	char c;
	int V;
	char* line = NULL;
	size_t bytes = 0;
	int getResult = getline(&line, &bytes, stream);
	if (getResult == EOF){
		return NULL;
	}

	sscanf(line," %c %d ", &c, &V);
	if (c != 'V' || V < 0)
	{
		fprintf(stderr, "Error: error input of V.\n");
		return NULL;
	}
	int leftN, rightN;
	graph = initGraph(V);
	
	getResult = scanf( " E {");
	while(1)
	{
		getResult = scanf("<%d,%d>,", &leftN, &rightN);
		if (getResult < 2)
			break;
		if (!addEdge(graph, leftN, rightN))
		{
			graph->V = -1;
			// destroyGraph(&graph);
			break;
		}
			
	}
	
	getline(&line, &bytes, stdin);
	return graph;
}

//retrun -1 if there no edge exist, otherwise return vid with highest degree
int getHstDgrVid(Graph* graph)
{
	int Vid = -1, hstDgr = 0, i;
	for(i = 0; i < graph->V; i++)
	{
		int temp = _getVidDgr(graph->array[i]);
		if(hstDgr < temp){
			hstDgr = temp;
			Vid = i;
		}
	}
	return Vid;
}

int _getVidDgr(NodeList nl)
{
	if (nl.head == NULL)
		return 0;
	int len = 0;
	Node* nCrawl = nl.head;
	while(nCrawl != NULL)
	{
		len++;
		nCrawl = nCrawl->next;
	}
	return len;
}

void deleteEdge(Graph* graph, int src, int dest)
{
	if(src < 0 || src > graph->V || dest < 0 || dest > graph-> V)
		return;
	
	_deleteNode(graph->array[src], dest);
	_deleteNode(graph->array[dest], src);

}

void _deleteNode(NodeList &nl, int key)
{
	Node *temp = nl.head, *prev;

	if(temp != NULL && temp->d == key)
	{
		nl.head = temp->next;
		free(temp);
		return;
	}

	while(temp != NULL && temp->d != key)
	{
		prev = temp;
		temp = temp->next;
	}
	if(temp == NULL)
	{//key doesn't find in this list
		return;
	}
	prev->next = temp->next;
	free(temp);
}

Graph* copyGraph(Graph* srcG)
{
	Graph* graph = initGraph(srcG->V);
	int i;
	for(i = 0; i < srcG->V; i++)
	{
		Node* srcHead = srcG->array[i].head;
		// Node* destHead = graph->array[i].head;
		while(srcHead != NULL)
		{
			Node *nd = (Node *) malloc(sizeof(Node));
			nd->d = srcHead->d;
			nd->next = graph->array[i].head;
			graph->array[i].head = nd;
			srcHead = srcHead->next;
		}	
	}
	return graph;
}

int getNextEdge(Graph* graph)
{
	int vId;
	for(vId = 0; vId < graph->V; vId++)
	{
		if(graph->array[vId].head != NULL)
			break;
	}
	if(vId >= graph->V)
		return -1;
	return vId;
}

void deleteVidEdges(Graph* graph, int vId)
{
	 Node* head = graph->array[vId].head;
      while(head != NULL)
      {
         Node* deleteNode = head;
         head = head->next;
         deleteEdge(graph, vId, deleteNode->d); 
      }
}
