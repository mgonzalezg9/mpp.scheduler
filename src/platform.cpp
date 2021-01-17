#include "../include/platform.hpp"

int getDevId(Device dev)
{
	return dev.id;
}

int getNumCores(Device dev)
{
	return dev.n_cores;
}

int getInstCore(Device dev)
{
	return dev.inst_core;
}

double getConsumptionCore(Device dev)
{
	return dev.consumption;
}

double getHyperConsumption(Device dev)
{
	int nCores = getNumCores(dev);
	double consumption = getConsumptionCore(dev);
	return (nCores * (consumption + 0.1 * consumption));
}

int getNumDevices(Platform *platform)
{
	return platform->n_devices;
}

Device *getDevices(Platform *platform)
{
	return platform->devices;
}

Device getDevice(Platform *platform, int i)
{
	return platform->devices[i];
}

int getComputationalCapacity(Platform *platform)
{
	int acum = 0;
	int nDevs = getNumDevices(platform);
	Device *devs = getDevices(platform);

	for (int i = 0; i < nDevs; i++)
	{
		acum += getNumCores(devs[i]) * getInstCore(devs[i]);
	}
	return acum;
}

double getConsumption(Platform *platform)
{
	double acum = 0.0;
	int nDevs = getNumDevices(platform);
	Device *devs = getDevices(platform);

	for (int i = 0; i < nDevs; i++)
	{
		acum += getNumCores(devs[i]) * getConsumptionCore(devs[i]);
	}
	return acum;
}

void createDeviceType(MPI_Datatype *device_type)
{
	Device indata;
	int numAttrs = 5;
	int blocklen1[5] = {1, 1, 1, 1, 1};

	MPI_Datatype typelist[5] = {MPI_INT, MPI_INT, MPI_INT, MPI_DOUBLE, MPI_INT};
	MPI_Aint addresses[numAttrs];
	MPI_Aint displacements[numAttrs];

	MPI_Get_address(&indata, &addresses[0]);
	MPI_Get_address(&(indata.inst_core), &addresses[1]);
	MPI_Get_address(&(indata.n_cores), &addresses[2]);
	MPI_Get_address(&(indata.consumption), &addresses[3]);
	MPI_Get_address(&(indata.hyperthreading), &addresses[4]);

	for (int i = 0; i < numAttrs; i++)
	{
		displacements[i] = addresses[i] - addresses[0];
	}

	MPI_Type_create_struct(numAttrs, blocklen1, displacements, typelist, device_type);
	MPI_Type_commit(device_type);
}
