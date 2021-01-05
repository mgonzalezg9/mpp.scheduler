#ifndef PLATFORM_H
#define PLATFORM_H

typedef struct {
	int id;
	int n_cores;
	int inst_core;
	double consumption;
	int hyperthreading;
} Device;

typedef struct {
	int n_devices;
	Device *devices;
} Platform;

int getDevId(Device dev);
int getNumCores(Device dev);
int getInstCore(Device dev);
double getConsumptionCore (Device dev);
double getHyperConsumption(Device dev);

int getNumDevices(Platform  *);
Device *getDevices(Platform *);
Device getDevice(Platform *, int);
int getComputationalCapacity(Platform *);
double getConsumption(Platform *);

#endif
