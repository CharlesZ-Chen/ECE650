#include <pthread.h>
#include <stdio.h>
#include "vc_algs.h"

#ifndef _TIME_ANALYSIS
#define PRINT_RESULT printResult
#define PRINTF 0&&printf
#else
#define PRINT_RESULT print4plotTime
#define PRINTF printf
#endif

enum VC_Alg {
	CNF_SAT_VC,
	APPROX_VC_1,
	APPROX_VC_2
};

static char * VC_Alg_String[] = {
	"CNF_SAT_VC", "APPROX_VC_1", "APPROX_VC_2",
};

typedef struct _vc_params{
	Graph *graph;
	VC_Alg alg_name;
} VC_params;

typedef struct _solveSet{
	int* vIdArray;
	int len;
	struct timespec ts;
} SolveSet;

void* vcAlg_thread_wrapper (void* vc_params);
void printResult(VC_params *prm, SolveSet *solveSet);
void print4plotTime(VC_params *prm, SolveSet *solveSet);
void print4plotAr(VC_params *prm, SolveSet *solveSet);

int 
main(int argc, char const *argv[])
{
	/* code */
	Graph* graph = NULL;
	pthread_t ptid_sat, ptid_apr1, ptid_apr2;
	VC_params prms_sat, prms_apr1, prms_apr2;
	SolveSet *solveSet_sat, *solveSet_apr1, *solveSet_apr2;

	prms_sat.alg_name = CNF_SAT_VC;
	prms_apr1.alg_name = APPROX_VC_1;
	prms_apr2.alg_name = APPROX_VC_2;

	while( (graph = readGraph(stdin)) != NULL)
	{
		if(graph->V == -1)//read an invalid graph
		{
			destroyGraph(&graph);
			continue;
		}
		//do muti-thread things here
		//initialize
		prms_sat.graph = graph;
		prms_apr1.graph = copyGraph(graph);
		prms_apr2.graph = copyGraph(graph);
		//compute
		pthread_create(&ptid_sat, NULL, &vcAlg_thread_wrapper, (void*) &prms_sat);
		pthread_create(&ptid_apr1, NULL, &vcAlg_thread_wrapper, (void*) &prms_apr1);
		pthread_create(&ptid_apr2, NULL, &vcAlg_thread_wrapper, (void*) &prms_apr2);

		//get result and output
		pthread_join(ptid_sat, (void**) &solveSet_sat);
		pthread_join(ptid_apr1, (void**) &solveSet_apr1);
		pthread_join(ptid_apr2, (void**) &solveSet_apr2);

		PRINT_RESULT(&prms_sat, solveSet_sat);
		PRINT_RESULT(&prms_apr1, solveSet_apr1);
		PRINT_RESULT(&prms_apr2, solveSet_apr2);
		PRINTF("\n");

		free(solveSet_sat);
		free(solveSet_apr1);
		free(solveSet_apr2);
		destroyGraph(&graph);
		destroyGraph(&(prms_apr1.graph));
		destroyGraph(&(prms_apr2.graph));
		
	}
	return 0;
}

void 
print4plotTime(VC_params *prm, SolveSet *solveSet)
{
	//unit is microseconds
	long totaltime = (solveSet->ts.tv_sec*1000000000+ solveSet->ts.tv_nsec)/1000;
	// fprintf(stdout, "%d.%09d\t", solveSet->ts.tv_sec, solveSet->ts.tv_nsec);
	fprintf(stdout, "%09ld\t", totaltime);
}

void 
print4plotAr(VC_params *prm, SolveSet *solveSet)
{
	fprintf(stdout, "%d\t", solveSet->len);
}

void 
printResult(VC_params *prm, SolveSet *solveSet)
{
	// printf("%s CPU time is: %d.%09d\n",VC_Alg_String[prm->alg_name], solveSet->ts.tv_sec, solveSet->ts.tv_nsec);
	printf("%s: ", VC_Alg_String[prm->alg_name]);
	printSolveSet(solveSet->vIdArray, solveSet->len);
	fflush(stdout);
}
void* 
vcAlg_thread_wrapper(void* vc_params)
{
	VC_params vp = *((VC_params*) vc_params);
	SolveSet* solveSet = (SolveSet*) malloc(sizeof(SolveSet));
	solveSet->len = -1, solveSet->vIdArray = NULL;
	switch(vp.alg_name){
		case CNF_SAT_VC:
			// printf("sat graph addr: %p\n", &(vp.graph));
			solveSet->len = cnf_SAT_VC(vp.graph, solveSet->vIdArray);
			break;
		case APPROX_VC_1:
			// printf("apr1 graph addr: %p\n", &(vp.graph));
			solveSet->len = approx_VC_1(vp.graph, solveSet->vIdArray);
			break;
		case APPROX_VC_2:
			solveSet->len = approx_VC_2(vp.graph, solveSet->vIdArray);
			break;
	}
	// clockid_t cid;
	// pthread_getcpuclockid(pthread_self(), &cid);
	// clock_gettime(cid, &(solveSet->ts));
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &(solveSet->ts));
	return  (void*) solveSet;
}
