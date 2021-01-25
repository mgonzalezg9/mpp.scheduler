GCC = g++
GDB = gdb
FLAGS = -O3 -std=c++11 -ggdb3 -Wall -lm -g -fpermissive
MODULES = src/main.cpp src/task.cpp src/platform.cpp src/io.cpp src/dev_usage.cpp src/ejecucion.cpp src/plat_usage.cpp
NUM_THREADS = 3
IN = 01
ARG_1 = input/$(IN)/platform_$(IN)
ARG_2 = input/$(IN)/tasks_$(IN)
SEC = ./sec
OMP = ./omp

omp: $(MODULES) src/omp.cpp
	$(GCC) $(FLAGS) -fopenmp $(MODULES) src/omp.cpp -o $(OMP) -DTIME -DDEBUG
#	Add -DDEBUG to display the solution.

sec: $(MODULES) src/sec.cpp
	$(GCC) $(FLAGS) $(MODULES) src/sec.cpp -o $(SEC) -DTIME -DDEBUG
#	Add -DDEBUG to display the solution.

valgrind:
	valgrind --tool=memcheck --leak-check=full $(SEC) $(ARG_1) $(ARG_2)

callgrind:
	valgrind --tool=callgrind $(SEC) $(ARG_1) $(ARG_2)
#	kcachegrind callgrind.out.<number>
 
debug:
	$(GDB) -ex=r --args $(SEC) $(ARG_1) $(ARG_2)
	
test_sec: sec
	$(SEC) $(ARG_1) $(ARG_2)
	
test_omp: omp
	OMP_NUM_THREADS=$(NUM_THREADS) $(OMP) $(ARG_1) $(ARG_2)

clean:
	rm -f sec omp
