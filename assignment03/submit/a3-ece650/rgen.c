#include "rgen.h"

int randomData;
int sVal = DFL_S, nVal = DFL_N, lVal = DFL_L, cVal = DFL_C;
Graph* graph;

#define TEST 0

#if TEST
#define MAIN main_during_test
#else
#define MAIN main
#endif

int MAIN(int argc, char **argv)
{
	opterr = 0;
	int opt;

	 while((opt = getopt(argc, argv, "s:n:l:c:")) != -1)
	 {
	 	switch(opt)
	 	{
	 		case 's':

	 			sVal = str2int(optarg);
	 			if(sVal < 2)
	 			{
	 				fprintf(stderr, "Error: s cannot lower than 2.\n");
	 				exit(EXIT_SUCCESS);
	 			}
	 			break;
	 		case 'n':
	 			nVal = str2int(optarg);
	 			if(nVal < 1)
	 			{
	 				fprintf(stderr, "Error: n cannot lower than 1.\n");
	 				exit(EXIT_SUCCESS);
	 			}
	 			break;
	 		case 'l':
	 			lVal = str2int(optarg);
	 			if(lVal < 5)
	 			{
	 				fprintf(stderr, "Error: l cannot lower than 5.\n");
	 				exit(EXIT_SUCCESS);
	 			}
	 			break;
	 		case'c':
	 			cVal = str2int(optarg);
	 			if(cVal < 1)
	 			{
	 				fprintf(stderr, "Error: c cannot lower than 1.\n");
	 				exit(EXIT_SUCCESS);
	 			}
	 			break;
	 	}
	 }
	// printf("sVal:%d\nnVal:%d\nlVal:%d\ncVal:%d\n", sVal, nVal, lVal, cVal);

	randomData = open("/dev/urandom", O_RDONLY);
	graph = NULL;
	signal(SIGTERM, sigtermHandler);
	while(1)
	{
		printRmv(graph);
		destroyGraph(&graph);
		graph = initGraph(randomInt(2, sVal));
		printAdd(graph);
		printf("g\n");
		fflush(stdout);
		sleep(randomInt(5, lVal));	
	}

	return EXIT_SUCCESS;
}

int str2int(char* const str)
{
	int i;
	sscanf(str, " %d ", &i);
	return i;
}
/**generate random data using /dev/urandom
* return an integer r from [left, right], right must > 0
* @depends: global var randomData (the File discriptor of /dev/urandom)
*/
int randomInt(int left, int right)
{
	if (left == right)
		{ return left; }
	int r;
	if(read(randomData, &r, sizeof(int)) < 0)
	{
		fprintf(stderr, "Error: cannot read /dev/urandom, return a fake randomInt.\n");
		return (left+right)/2;
	}
	return abs(r%(right-left+1)) + left;
}

Point randomPoint(int k)
{
	Point p;
	p.x = randomInt(-k, k);
	p.y = randomInt(-k, k);
	return p;
}
/*signal handler for SIGINT(ctr-c)*/
void sigtermHandler(int signum)
{
	signal(SIGTERM, SIG_DFL);
	close(randomData);
	destroyGraph(&graph);
	exit(EXIT_FAILURE);
}

Graph* initGraph(int len)
{
	Graph* graph = (Graph*) malloc(sizeof(Graph));
	graph->len = len;
	graph->stArray = (Street*) malloc(sizeof(Street)*graph->len);
	int i;
	for (i = 0; i < graph->len; i++)
	{
		graph->stArray[i].len = 0;
	}

	for (i = 0; i < graph->len; i++)
	{
		/*range of st->pointArray should be [2 ,nVal+1] => [1, nVal] segments*/
		_initStreet(graph, i, randomInt(2, nVal+1));
	}
	return graph;
}

void _initStreet(Graph* graph, int stId, int len)
{
	int i;
	Street* st = &(graph->stArray[stId]);
	st->pointArray = (Point*) malloc(sizeof(Point) * len);
	for(i = 0; i < len; i++)
	{	
		st->pointArray[i] = _newPoint(graph, st, i);
	}
	st->len = len;
}

Point _newPoint(Graph* graph, Street* st, int indexThisP)
{
	Point p = randomPoint(cVal);
	if (indexThisP == 0)
		{	return p;	}
	Point prevP = st->pointArray[indexThisP-1];
	
	int tryTime = 25;
	int i, j;
	i = 0;
	while(!_isValidSeg(graph, st, indexThisP, prevP, p))
	{
		if(i >= tryTime)
		{
			fprintf(stderr, "Error: failed to generate valid input for %d simultaneous attempts \n", tryTime);
			raise(SIGQUIT);
		}
		p = randomPoint(cVal);

		i++;
	}
	return p;
}

int _isValidSeg(Graph* graph, Street* st, int indexThisP, Point prevP, Point thisP)
{
	if (prevP.x == thisP.x && prevP.y == thisP.y)
		{	return 0;	}
	int i, j;
	//check the seg with current graph
	for(i = 0; graph->stArray[i].len > 0; i++)
	{
		Street thisSt = graph->stArray[i];
		for(j = 0; j < thisSt.len - 1; j++)
		{
			if(isOverlap(&(thisSt.pointArray[j]), &(thisSt.pointArray[j+1]), &prevP, &thisP))
				{	return 0;	}
		}
	}

	//check the seg with it's street current len
	for(j = 0; j < indexThisP - 1; j++)
	{
		if(isOverlap(&(st->pointArray[j]), &(st->pointArray[j+1]), &prevP, &thisP))
			{	return 0;	}
	}
	return 1;
}

int isOverlap(Point* seg1Sp, Point* seg1Ep, Point* seg2Sp, Point* seg2Ep)
{
	Point n1, n2;
	int denominator;
	int isNotSameLine;

	n1 = _normal_line(*seg1Sp, *seg1Ep);
	n2 = _normal_line(*seg2Sp, *seg2Ep);
	denominator = n1.x * n2.y - n1.y * n2.x;
	if(denominator != 0)
		{	
			return 0;	}
	isNotSameLine = n1.x * (seg2Sp->x - seg1Sp->x) + n1.y * (seg2Sp->y - seg1Sp->y);
	if(isNotSameLine)
		{	
			return 0;	}
	segSort(&seg1Sp, &seg1Ep);
	segSort(&seg2Sp, &seg2Ep);

	if (seg1Ep->x != seg2Ep->x)
	{
		if ( seg1Ep->x > seg2Ep->x)
		{
			if (seg2Ep->x > seg1Sp->x)
				{	return 1;	}
			else
				{	return 0;	}
		}
		else
		{
			if(seg1Ep->x > seg2Sp->x)
				{	return 1;	}
			else
				{	return 0;	}
		}
	} else 
	{
		if (seg1Ep->y > seg2Ep->y)
		{
				if(seg2Ep->y > seg1Sp->y)
					{	return 1;	}
				else
					{	return 0;	}
		}
		else if (seg2Ep->y > seg1Ep->y)
		{
			if(seg1Ep->y > seg2Sp->y)
				{	return 1;	}
			else
				{	return 0;	}
		} else {	return 1;	}
	}
}

Point _normal_line(Point segSp, Point segEp)
{
	Point np;
	np.x = segEp.y - segSp.y;
	np.y = segSp.x - segEp.x;
	return np;
}

void segSort( Point** p1, Point** p2)
{
	Point* tmp = NULL;

	if((*p1)->x != (*p2)->x)
		{
			if((*p1)->x > (*p2)->x)
			{
				tmp = *p2;
				*p2 = *p1;
				*p1 = tmp;
				return;
			}
		}
	else if ((*p1)->y != (*p2)->y)
	{
		if ((*p1)->y > (*p2)->y)
		{
			tmp = *p2;
			*p2 = *p1;
			*p1 = tmp;
			return;
		}
	}
}

void destroyGraph(Graph ** graph)
{
	if(*graph == NULL)
		{ return; }
	int i, j;
	for(i = 0; i < (*graph)->len; i++)
	{
		
		free((*graph)->stArray[i].pointArray);
	}
	free((*graph)->stArray);
	free(*graph);
	*graph = NULL;
	return;
}

void printAdd(Graph* graph)
{
	if(graph == NULL)
		{	return;	}
	int i,j;
	for(i = 0; i < graph->len; i++)
	{
		printf("a \"s%d\" ", i);
		Street* st = &(graph->stArray[i]);
		for(j = 0; j < st->len; j++)
		{
			printf("(%d,%d)", st->pointArray[j].x, st->pointArray[j].y);
		}
		printf("\n");
	}
	fflush(stdout);
	return;
}

void printRmv(Graph* graph)
{
	if(graph == NULL)
		{	return;	}
	int i;
	for(i = 0; i < graph->len; i++)
	{
		printf("r \"s%d\"\n", i);
	}
	fflush(stdout);
	return;
}
