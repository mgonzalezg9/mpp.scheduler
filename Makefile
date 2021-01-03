GCC = g++
FLAGS = -O3 -std=c++11 -ggdb3 -Wall -lm
ARG_1 = input/01/platform_01
ARG_2 = input/01/tasks_01
SEC = ./sec

sec: src/main.c src/task.c src/platform.c src/io.c src/sec.c
	$(GCC) $(FLAGS) src/main.c src/task.c src/platform.c src/io.c src/sec.c -o sec -DTIME
#	Add -DDEBUG to display the solution.

valgrind:
	valgrind --tool=memcheck --leak-check=full $(SEC) $(ARG_1) $(ARG_2)

callgrind:
	valgrind --tool=callgrind $(SEC) $(ARG_1) $(ARG_2)
#	kcachegrind callgrind.out.<number>
 
test_sec:
	$(SEC) $(ARG_1) $(ARG_2)

clean:
	rm -f sec
