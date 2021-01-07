#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../include/task.hpp"
#include "../include/platform.hpp"

Task *load_tasks(const char *, int *);
Platform *load_platform(const char *);
void print_solution(Task *, int, Platform *, double, double);
void get_solution(Task *, int, Platform *, Task *, Platform *, double, double);
