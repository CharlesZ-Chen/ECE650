CC = g++
LIB_DIR = .
HEAD_DIR = .
LIB_DIR2 = ../zchaff64
HEAD_DIR2 = ../zchaff64
LIB_DIR3 = ../zchaff
HEAD_DIR3 = ../zchaff
LIB_DIR4 = ./zchaff64
HEAD_DIR4 = ./zchaff64
LIB_DIR5 = ./zchaff
HEAD_DIR5 = ./zchaff
LIB_FLAGS = -L$(LIB_DIR) -L$(LIB_DIR2) -L$(LIB_DIR3) -L$(LIB_DIR4) -L$(LIB_DIR5) -I$(HEAD_DIR) -I$(HEAD_DIR2) -I$(HEAD_DIR3) -I$(HEAD_DIR4) -I$(HEAD_DIR5) -l$(SAT_LIB)
FLAGS = -Wno-deprecated $(LIB_FLAGS)
SOURCE_FILES = a4-ece650.c graph.c
EXE = a4-ece650
SAT_LIB = sat
TEST_DIR = ../../testInput/
TEST_INPUT = input.txt

all:$(SOURCE_FILES)
	$(CC) $(SOURCE_FILES) $(FLAGS) -o $(EXE)

clean:
	rm -f $(EXE)

test:clean all
	cat $(TEST_DIR)$(TEST_INPUT) | ./$(EXE)

wd_test%:
	/home/wdietl/graphGen/graphGen $* | tee testGraph.txt | ./$(EXE)
	cat testGraph.txt
with_zchaff:clean
	make -C ../zchaff64 clean
	make -C ../zchaff zchaff
	make all
with_zchaff64:clean
	make -C ../zchaff clean
	make -C ../zchaff64 zchaff
	make all
	
