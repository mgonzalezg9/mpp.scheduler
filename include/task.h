#ifndef TASK_H
#define TASK_H

typedef struct {
	int id;
	int n_inst;
	int n_deps;
	int *dep_tasks;
} Task;

int getId(Task t);
int getNumInst(Task t);
int getNumDeps(Task t);
int *getDependencies(Task t);
	
#endif
