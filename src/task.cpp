#include "../include/task.hpp"

int getId(Task t)
{
	return t.id;
}

int getNumInst(Task t)
{
	return t.n_inst;
}

int getNumDeps(Task t)
{
	return t.n_deps;
}

int *getDependencies(Task t)
{
	return t.dep_tasks;
}

bool isFinalizada(Task t, int numInstEjecutadas)
{
	return getNumInst(t) == numInstEjecutadas;
}

int getInstRestantes(Task t, int numInstEjecutadas)
{
	return getNumInst(t) - numInstEjecutadas;
}
