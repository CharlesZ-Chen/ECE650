#include "graph.h"
#include "SAT.h"
#include <unistd.h>
#include <math.h>

#ifdef _AS_MODULE_VC_ALGS
#define MAIN_VC_ALGS main_vc_algs
#else
#define MAIN_VC_ALGS main
#endif

int cnf_SAT_VC(Graph* graph, int* &solveSet);
int approx_VC_1(Graph* graph, int* &solveSet);
int approx_VC_2(Graph* graph, int* &solveSet);
void printSolveSet(int* solveSet, int len);
int _cmp(const void* a, const void* b);
