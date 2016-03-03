#include "graph.h"
#include "SAT.h"
#include <unistd.h>
#include <math.h>

int solveVertexCover_SAT(Graph* graph, int* &solveSet);
void printSolveSet(int* solveSet, int len);
int cmp(const void* a, const void* b);

int main(int argc, char const *argv[])
{
	/* code */
	char* line = NULL;
	size_t bytes = 0;
	int* solveSet = NULL, k = 0;
	Graph* graph = NULL;
	while(getline(&line, &bytes, stdin) != EOF)
	{
		graph = readGraph(line);
		k = solveVertexCover_SAT(graph, solveSet);
		printSolveSet(solveSet, k);
		free(solveSet);
		solveSet = NULL;
		destroyGraph(&graph);
	}

	return 0;
}

/*return the solve vertex set
*return NULL if unsolvable or any error occurs
*/
int solveVertexCover_SAT(Graph* graph, int* &solveSet)
{
	if(graph == NULL || graph->V == 0)
	{
		return 0;
	}
	SAT_Manager mgr;
	mgr = SAT_InitManager();

	int bakP[2], bak;

    pipe(bakP);
    bak = dup(STDOUT_FILENO);
    dup2(bakP[1], STDOUT_FILENO);
   
   	int k, i, j, m, p, q, cIter, solveResult = UNSATISFIABLE;
   	
   	for(k = 1; k <= graph->V; k++)
   	{
   		int groupId = SAT_AllocClauseGroupID(mgr);
   		if(k == 1)
   			SAT_SetNumVariables(mgr, 1*graph->V);
   		else{//add V Variable each loop since k is increment by 1 each time 
   			int temp = 0;
   			while(temp < graph->V){
   				SAT_AddVariable(mgr);
   				temp++;
   			}
   		}
   		int* c = NULL;
   		/* pre alloc c as maximum cluase size */
   		if(2*k > graph->V)
   			c = (int*) malloc(sizeof(int) * (2*k));
   		else
   			c = (int*) malloc(sizeof(int) * graph->V);
   		/* for k*V atoms matrix: Xrc => k*(r-1)+c (var index in SAT) */

   		/* for i:[1,k] add clause: X1i v X2i v .. v XVi (r:[1,V] c:i)*/
   		for(i = 1; i <= k; i++)
   		{
   			for(cIter = 0; cIter < graph->V; cIter++)
   				c[cIter] = ((k*cIter + i) << 1);
   			 SAT_AddClause(mgr, c, graph->V, groupId);
   		}

   		/* for m:[1,n] p,q:[1,k] p<q add clause: ~Xmp v ~Xmq */
   		for(m = 1; m <= graph->V; m++)
   			for(p = 1; p <= k; p++)
   				for(q = p+1; q <= k; q++)
   				{
   					c[0] = ((k*(m-1)+p) << 1) + 1;
   					c[1] = ((k*(m-1)+q) << 1) + 1;
   					SAT_AddClause(mgr, c, 2, groupId);
   				}

   		/* for m:[1,k] p,q:[1,n] p<q add clause: ~Xpm v ~Xqm */
   		for(m = 1; m <= k; m++)
   			for(p = 1; p <= graph->V; p++)
   				for(q = p+1; q <= graph->V; q++)
   				{
   					c[0] = ((k*(p-1)+m) << 1) + 1;
   					c[1] = ((k*(q-1)+m) << 1) + 1;
   					SAT_AddClause(mgr, c, 2, groupId);
   				}
   		/* for each edge <i,j> in graph add clause: 
   			Xi1 v Xi2 v ... v Xik v Xj1 v Xj2 v ... v Xjk */
   		int num_cluase = SAT_NumClauses(mgr);
   		for(i = 0; i < graph->V; i++)
   		{
   			Node* nCrawl = graph->array[i].head;
   			while(nCrawl != NULL)
   			{
   				int dest = nCrawl->d;
   				/*only add clause for edge<i,dest> which dest > i
   				 for add each edge only once in an undirect graph*/
   				if(dest > i){
   					for(cIter = 0; cIter < k; cIter++)
   						c[cIter] = ((k*i+cIter+1) << 1);
   					for(cIter = k; cIter < 2*k; cIter++)
   						c[cIter] = ((k*dest+cIter-k+1) << 1);
   					SAT_AddClause(mgr, c, 2*k, groupId); 
   				}
   				nCrawl = nCrawl->next;
   			}
   		}
   		if(num_cluase == SAT_NumClauses(mgr)){
            break;
   		}
   		solveResult = SAT_Solve(mgr);
   		if(solveResult == SATISFIABLE)
   			break;
   		free(c);
   		SAT_DeleteClauseGroup(mgr, groupId);
   		SAT_Reset(mgr);
   	}

   	dup2(bak, STDOUT_FILENO);
   	if(solveResult == UNSATISFIABLE){
   		SAT_ReleaseManager(mgr);
   		return 0;
   	}
	int n = SAT_NumVariables(mgr);

	solveSet = (int*) malloc(sizeof(int) * k);

	for(i = 0, j = 1; j <= n && i < k; j++) {
	   	int a = SAT_GetVarAsgnment(mgr, j);
	   	if(a == 1) {
	   		solveSet[i] = (int) ceil(((double)j/(double)k)) - 1;
			i++;
	    }
	   	else if(a == 0) {}
	   	else 
	   	{
	   		fprintf(stderr, "Error: SAT error.\n");
	   		SAT_ReleaseManager(mgr); 
			return 0; 
	   	}
	}
	SAT_ReleaseManager(mgr);
	return k;
}

void printSolveSet(int* solveSet, int len)
{
	if (len < 1){
		return;
	}
	qsort(solveSet, len, sizeof(int), cmp);
	int i;
	for(i = 0; i < len; i++)
	{
		printf("%d ",solveSet[i]);
	}
	printf("\n");
	fflush(stdout);
}

int cmp(const void* a, const void* b)
{
	return *((int*)a) > *((int*)b) ? 1 : -1;
}
