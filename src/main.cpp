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

	// Read Input Data
	Task *tasks = load_tasks(argv[2], &n_tasks);
	Platform *platform = load_platform(argv[1]);
	n_devs = getNumDevices(platform);

	// Array for Store the Solution
	Task *sorted_tasks = (Task *)malloc(n_tasks * sizeof(Task));
	assert(sorted_tasks);

	// Each position corresponds to the sub-platform (set of computing units) used for each task
	Platform *selected_dev = (Platform *)malloc(n_tasks * sizeof(Platform));
	assert(selected_dev);

	for (int i = 0; i < n_tasks; i++)
	{
		selected_dev[i].devices = (Device *)malloc((2 * n_devs) * sizeof(Device));
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

	// if (rank == 1)
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

	get_solution(tasks, n_tasks, platform, sorted_tasks, selected_dev, time, energy, device_type, rank, size);

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
