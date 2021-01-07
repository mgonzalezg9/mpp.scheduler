#ifndef TASK_H
#define TASK_H

#include <vector>

using namespace std;

struct Task
{
	int id;
	int n_inst;
	int n_deps;
	int *dep_tasks;

	bool operator<(const Task &other) const
	{
		return id < other.id;
	}

	bool operator==(const Task &other) const
	{
		return id == other.id;
	}
};

int getId(Task t);
int getNumInst(Task t);
int getNumDeps(Task t);
int *getDependencies(Task t);
bool isFinalizada(Task t, int numInstEjecutadas);
int getInstRestantes(Task t, int numInstEjecutadas);

#endif
