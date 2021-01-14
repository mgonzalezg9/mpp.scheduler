#include <sys/time.h>
#include "mpi.h"
#include "../include/io.hpp"

static double mseconds()
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return (t.tv_sec * 1000 + t.tv_usec / 1000);
}

int main(int argc, char **argv)
{
	// Check Input Args
	if (argc < 3)
	{
		printf("Usage: %s <platform_file> <tasks_file>\n", argv[0]);
		return (-1);
	}
	printf("\n");

	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	double time, energy;
	MPI_Datatype device_type;
	createDeviceType(&device_type);

	int n_tasks, n_devs;
	Task *tasks, *sorted_tasks;
	tasks = sorted_tasks = NULL;
	Platform *platform, *selected_dev;
	platform = selected_dev = NULL;
	if (rank == 0)
	{
		// Read Input Data
		tasks = load_tasks(argv[2], &n_tasks);
		platform = load_platform(argv[1]);
		n_devs = getNumDevices(platform);

		// Envío de las tareas
		MPI_Bcast(&n_tasks, 1, MPI_INT, 0, MPI_COMM_WORLD);

		for (int i = 0; i < n_tasks; i++)
		{
			MPI_Bcast(&tasks[i].id, 1, MPI_INT, 0, MPI_COMM_WORLD);
			MPI_Bcast(&tasks[i].n_inst, 1, MPI_INT, 0, MPI_COMM_WORLD);
			MPI_Bcast(&tasks[i].n_deps, 1, MPI_INT, 0, MPI_COMM_WORLD);
			MPI_Bcast(tasks[i].dep_tasks, tasks[i].n_deps, MPI_INT, 0, MPI_COMM_WORLD);
		}

		// Envío de la plataforma
		MPI_Bcast(&n_devs, 1, MPI_INT, 0, MPI_COMM_WORLD);

		Device *devices = getDevices(platform);
		MPI_Bcast(devices, n_devs, device_type, 0, MPI_COMM_WORLD);

		// Array for Store the Solution
		sorted_tasks = (Task *)malloc(n_tasks * sizeof(Task));
		assert(sorted_tasks);

		// Each position corresponds to the sub-platform (set of computing units) used for each task
		selected_dev = (Platform *)malloc(n_tasks * sizeof(Platform));
		assert(selected_dev);

		for (int i = 0; i < n_tasks; i++)
		{
			selected_dev[i].devices = (Device *)malloc((2 * n_devs) * sizeof(Device));
		}
	}
	else
	{
		// Recepción de las tareas
		MPI_Bcast(&n_tasks, 1, MPI_INT, 0, MPI_COMM_WORLD);

		tasks = (Task *)malloc(n_tasks * sizeof(Task));
		for (int i = 0; i < n_tasks; i++)
		{
			Task t;

			MPI_Bcast(&t.id, 1, MPI_INT, 0, MPI_COMM_WORLD);
			MPI_Bcast(&t.n_inst, 1, MPI_INT, 0, MPI_COMM_WORLD);
			MPI_Bcast(&t.n_deps, 1, MPI_INT, 0, MPI_COMM_WORLD);

			t.dep_tasks = (int *)malloc(t.n_deps * sizeof(int));
			MPI_Bcast(t.dep_tasks, t.n_deps, MPI_INT, 0, MPI_COMM_WORLD);

			tasks[i] = t;
		}

		// Recepción de la plataforma
		platform = (Platform *)malloc(sizeof(Platform));

		MPI_Bcast(&n_devs, 1, MPI_INT, 0, MPI_COMM_WORLD);
		platform->n_devices = n_devs;
		assert(platform);

		platform->devices = (Device *)malloc(n_devs * sizeof(Device));
		assert(platform->devices);
		MPI_Bcast(platform->devices, n_devs, device_type, 0, MPI_COMM_WORLD);
	}

	double ti, tf;
	ti = tf = 0.0;
	if (rank == 0)
	{
#ifdef TIME
		ti = mseconds();
#endif

		// Solve the Problem
		time = energy = 0.0;
	}

	// if (rank == 2)
	// {
	// 	for (int i = 0; i < n_tasks; i++)
	// 	{
	// 		Task t = tasks[i];
	// 		cout << "T" << t.id << ": NI=" << t.n_inst << endl;
	// 	}

	// 	cout << "La plataforma tiene " << platform->n_devices << " dispositivos" << endl;

	// 	for (int i = 0; i < platform->n_devices; i++)
	// 	{
	// 		Device d = platform->devices[i];
	// 		cout << "D" << d.id << ": " << d.n_cores << " cores con " << d.inst_core << " cada uno" << endl;
	// 	}
	// }

	// get_solution(tasks, n_tasks, platform, sorted_tasks, selected_dev, time, energy, rank, size);

	MPI_Barrier(MPI_COMM_WORLD);

	if (rank == 0)
	{
#ifdef TIME
		tf = mseconds();
		printf("\nTime = %.2lf miliseconds\n", (tf - ti));
#endif

#ifdef DEBUG
		// Print Result
		print_solution(sorted_tasks, n_tasks, selected_dev, time, energy);
#endif
	}

	// Free Allocated Memory
	for (int i = 0; i < n_tasks; i++)
	{
		free(tasks[i].dep_tasks);
	}
	free(tasks);

	free(platform->devices);
	free(platform);

	if (rank == 0)
	{
		for (int i = 0; i < n_tasks; i++)
		{
			free(selected_dev[i].devices);
		}
		free(selected_dev);
		free(sorted_tasks);
	}

	MPI_Finalize();

	return 0;
}
