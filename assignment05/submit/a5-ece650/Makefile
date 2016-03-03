CC = g++
LIB_DIR = .
HEAD_DIR = .

LIB_FLAGS = -L$(LIB_DIR) -I$(HEAD_DIR) -l$(SAT_LIB)
A5_FLAGS = -D_AS_MODULE_VC_ALGS
FLAGS = -Wno-deprecated -lpthread -lrt $(LIB_FLAGS) $(A5_FLAGS)

SAT_LIB = sat
ALG_SOURCE_FILES = vc_algs.c graph.c
SOURCE_FILES = a5-ece650.c $(ALG_SOURCE_FILES)
EXE = a5-ece650
ALG_EXE = vc_alg
TEST_DIR = ./testInput/
TEST_INPUT = input.txt

all:$(SOURCE_FILES)
	$(CC) $(SOURCE_FILES) $(FLAGS) -o $(EXE)

ar:a5-ece650-ar.c $(ALG_SOURCE_FILES)
	$(CC) -D_TIME_ANALYSIS  a5-ece650-ar.c $(ALG_SOURCE_FILES) $(FLAGS) -o a5-ece650.ar
12:a5-ece650-12.c $(ALG_SOURCE_FILES)
	$(CC) -D_TIME_ANALYSIS a5-ece650-12.c $(ALG_SOURCE_FILES) $(FLAGS) -o a5-ece650.12

clean:
	rm -f $(EXE)

test:clean all
	cat $(TEST_DIR)$(TEST_INPUT) | ./$(EXE)

wd_test%:
	/home/wdietl/graphGen/graphGen $* | tee testGraph.txt | ./$(EXE)
	cat testGraph.txt

vc_alg:$(ALG_SOURCE_FILES)
	$(CC) $(ALG_SOURCE_FILES) $(LIB_FLAGS) -o $(ALG_EXE)

with_zchaff:clean
	make -C ../zchaff64 clean
	make -C ../zchaff zchaff
	make all
with_zchaff64:clean
	make -C ../zchaff clean
	make -C ../zchaff64 zchaff
	make all
	
