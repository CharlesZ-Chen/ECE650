#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#ifndef DFL_S
#define DFL_S 10
#define DFL_N 5
#define DFL_L 5
#define DFL_C 20
#endif

typedef struct _point
{
	int x, y;
} Point;

typedef struct _Street
{
	int len;
	Point* pointArray;
}Street;

typedef struct _Graph
{
	int len;
	Street* stArray;
}Graph;

int str2int(char* const str);
/**generate random data using /dev/urandom*/
int randomInt(int left, int right);
void sigtermHandler(int signum);
Graph* initGraph(int len);
void destroyGraph(Graph** Graph);
void _initStreet(Graph* graph, int stId, int len);
Point _newPoint(Graph* graph, Street* st, int indexThisP);
int _isValidSeg(Graph* graph, Street* st, int indexThisP, Point prevP, Point thisP);
int isOverlap(Point* seg1Sp, Point* seg1Ep, Point* seg2Sp, Point* seg2Ep);
Point _normal_line(Point segSp, Point segEp);
void destroyGraph(Graph ** graph);
void printRmv(Graph* graph);
void printAdd(Graph* graph);
void segSort( Point** p1, Point** p2);

