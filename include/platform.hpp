#ifndef PLATFORM_H
#define PLATFORM_H

struct Device
{
	int id;
	int n_cores;
	int inst_core;
	double consumption;
	int hyperthreading;

	// Ordena los dispositivos según su consumo por instrucción
	bool operator<(const Device &other)
	{
		return (consumption / inst_core) < (other.consumption / other.inst_core);
	}
};

struct Platform
{
	int n_devices;
	Device *devices;
};

int getDevId(Device dev);
int getNumCores(Device dev);
int getInstCore(Device dev);
double getConsumptionCore(Device dev);
double getHyperConsumption(Device dev);

int getNumDevices(Platform *);
Device *getDevices(Platform *);
Device getDevice(Platform *, int);
int getComputationalCapacity(Platform *);
double getConsumption(Platform *);

#endif
