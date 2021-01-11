GCC = g++
GDB = gdb
FLAGS = -O3 -std=c++11 -ggdb3 -Wall -lm -g
MODULES = src/main.cpp src/task.cpp src/platform.cpp src/io.cpp src/sec.cpp src/dev_usage.cpp src/ejecucion.cpp src/plat_usage.cpp
IN = 01
ARG_1 = input/$(IN)/platform_$(IN)
ARG_2 = input/$(IN)/tasks_$(IN)
SEC = ./sec

sec: $(MODULES)
	$(GCC) $(FLAGS) $(MODULES) -o sec -DTIME -DDEBUG
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

clean:
	rm -f sec
