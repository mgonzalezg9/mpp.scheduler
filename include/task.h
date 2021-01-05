#ifndef TASK_H
#define TASK_H

typedef struct Task
{
	int id;
	int n_inst;
	int n_deps;
	int *dep_tasks;

	bool operator<(const Task &other) const
	{
		return id < other.id;
	}
} Task;

int getId(Task t);
int getNumInst(Task t);
int getNumDeps(Task t);
int *getDependencies(Task t);

#endif
