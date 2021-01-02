#include "../include/task.h"

int getId(Task t) {
	return t.id;
}

int getNumInst(Task t) {
	return t.n_inst;
}

int getNumDeps(Task t) {
	return t.n_deps;
}

int *getDependencies(Task t) {
	return t.dep_tasks;
}
