#include "../include/platform.h"

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
