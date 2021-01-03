#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../include/task.h"
#include "../include/platform.h"

Task *load_tasks(const char *file, int *n_tasks)
{
	char c;
	int id, n_inst, td, n, i, j;
	
	FILE *fd = fopen(file, "r");
	
	int nElem = fscanf(fd, "%d\n", &n);
	printf("Reading File: %s\n", file);
	
	Task *tasks = (Task *) malloc(n*sizeof(Task));
	assert(tasks);
	
	i = 0;
	while(i < n) {
		nElem = fscanf(fd, "%d %d %d%c", &id, &n_inst, &td, &c);
		assert(nElem == 4);
		
		tasks[i].id = id;
		tasks[i].n_inst = n_inst;
		tasks[i].dep_tasks = (int *) malloc(sizeof(int));
		tasks[i].dep_tasks[0] = td;
		
		j=1;
		do {
			if(c == ',') {
				j++;
				nElem = fscanf(fd, "%d%c", &td, &c);
				tasks[i].dep_tasks = (int *) realloc(tasks[i].dep_tasks, j*sizeof(int));
				tasks[i].dep_tasks[j-1] = td;
			}
			else { break; }
		} while (c != '\n');
		tasks[i].n_deps = j;
		i++;
	}
	
	fclose(fd);
	*n_tasks = n;
	return tasks;
}

Platform *load_platform(const char *file)
{
	int i;
	int n_devs;
	int id, n_cores, inst_core;
	double consumption;
	
	FILE *fd = fopen(file, "r");
	
	int nElem = fscanf(fd, "%d\n", &n_devs);
	printf("Reading File: %s\n", file);
	
	Platform *platform = (Platform *) malloc(sizeof(Platform));
	platform->n_devices = n_devs;
	assert(platform);
	
	platform->devices = (Device *) malloc(n_devs*sizeof(Device));
	assert(platform->devices);
	
	i = 0;
	while(i < n_devs) {
		nElem = fscanf(fd, "%d %d %d %lf\n", &id, &n_cores, &inst_core, &consumption);
		assert(nElem == 4);
		
		platform->devices[i].id = id;
		platform->devices[i].n_cores = n_cores;
		platform->devices[i].inst_core = inst_core;
		platform->devices[i].consumption = consumption;
		platform->devices[i].hyperthreading = 0;
		i++;
	}
	
	fclose(fd);
	return platform;
}

void print_solution(Task *sorted_tasks, int n_tasks, Platform *selected_devs, double time, double energy)
{
	int i, j, ht, n_devs;
	printf("Best Solution: ");
	
	printf("{");
	for(i=0; i<n_tasks-1; i++) {
		printf("T%d, ", sorted_tasks[i].id);
	}
	printf("T%d}: ", sorted_tasks[i].id);
	printf("(%.2lf, %.2lf)\n", time, energy);
	
	for(i=0; i<n_tasks; i++) {
		n_devs = selected_devs[i].n_devices;
		printf("     T%d: {", sorted_tasks[i].id);
		for(j=0; j<n_devs; j++) {
			printf("%d(%d", selected_devs[i].devices[j].id, selected_devs[i].devices[j].n_cores);
			ht = selected_devs[i].devices[j].hyperthreading;
			if(ht == 1) {
				printf("x2)");
			}
			else {
				printf(")");
			}
			if(j < n_devs-1) { printf(" + "); }
		}
		printf("}\n");
	}
	printf("\n");
}
