MPICC = mpic++
GCC = g++
GDB = gdb
FLAGS = -O3 -std=c++11 -ggdb3 -Wall -lm -g -fpermissive
MODULES = src/task.cpp src/platform.cpp src/io.cpp src/dev_usage.cpp src/ejecucion.cpp src/plat_usage.cpp
NUM_UNITS = 3
IN = 01
ARG_1 = input/$(IN)/platform_$(IN)
ARG_2 = input/$(IN)/tasks_$(IN)
SEC = ./sec
OMP = ./omp
MPI = ./mpi

sec: src/main.cpp src/sec.cpp $(MODULES) 
	$(GCC) $(FLAGS) $(MODULES) src/main.cpp src/sec.cpp -o $(SEC) -DTIME -DDEBUG
#	Add -DDEBUG to display the solution.

mpi: src/main_mpi.cpp src/mpi.cpp $(MODULES)
	$(MPICC) $(FLAGS) $(MODULES) src/main_mpi.cpp src/mpi.cpp -o $(MPI) -DTIME -DDEBUG
#	Add -DDEBUG to display the solution.

omp: src/main.cpp src/omp.cpp $(MODULES)
	$(GCC) $(FLAGS) -fopenmp $(MODULES) src/main.cpp src/omp.cpp -o $(OMP) -DTIME -DDEBUG
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
	OMP_NUM_THREADS=$(NUM_UNITS) $(OMP) $(ARG_1) $(ARG_2)

test_mpi: mpi
	mpirun -np $(NUM_UNITS) $(MPI) $(ARG_1) $(ARG_2)

clean:
	rm -f sec omp mpi
