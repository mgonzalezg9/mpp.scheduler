#include "../include/plat_usage.hpp"
#include <algorithm>
#include <set>
#include <iostream>

PlatUsage::PlatUsage(Platform *plataforma)
{
    // Construye la simulación de una plataforma sin tareas ejecutandose
    for (int i = 0; i < getNumDevices(plataforma); i++)
    {
        Device device = getDevice(plataforma, i);
        DevUsage dev(device);
        dispositivos.push_back(dev);
    }
}

// Getters
vector<DevUsage> PlatUsage::getDispositivos()
{
    return dispositivos;
}

// Asigna una serie de cores a la tarea t y devuelve las instrucciones ejecutadas
int PlatUsage::asignarCores(Task t, bool ht);

// Devuelve las tareas que se están ejecutando
vector<Task> PlatUsage::getTareas()
{
    vector<Task> tareas;
    for (auto dev : dispositivos)
    {
        vector<Task> devTareas = dev.getTareas();
        tareas.insert(tareas.end(), devTareas.begin(), devTareas.end());
    }
    return tareas;
}

// Devuelve si la plataforma ejecuta alguna tarea
bool PlatUsage::isOcupado()
{
    for (auto dev : dispositivos)
    {
        if (dev.isOcupado())
        {
            return true;
        }
    }
    return false;
}

// Devuelve el tiempo que tarda la tarea t en ejecutarse
double PlatUsage::getTiempo(Task t)
{
    return ((double)getInstruccionesEjecutadas(t)) / getNumInst(t);
}

// Devuelve el incremento en el tiempo que supone el uso actual de la plataforma
double PlatUsage::getTiempo()
{
    double maxTiempo = 0.0;
    vector<Task> tareasEjecutadas = getTareas();

    for (auto tarea : tareasEjecutadas)
    {
        double tiempo = getTiempo(tarea);

        if (tiempo > maxTiempo)
        {
            maxTiempo = tiempo;
        }
    }
    return maxTiempo;
}

// Devuelve el incremento de energía que supone el uso actual de la plataforma
double PlatUsage::getEnergia()
{
    double energia = 0.0;
    for (auto dev : dispositivos)
    {
        energia += dev.getEnergia();
    }
    return energia;
}

// Devuelve si es consistente el estado de la plataforma (las dependencias de las tareas están satisfechas)
bool PlatUsage::isRealizable(vector<Ejecucion> tareasPendientes)
{
    set<int> pendientes = Ejecucion::getIds(tareasPendientes);
    vector<Task> tareasEjecucion = getTareas();

    for (auto tarea : tareasEjecucion)
    {
        int idTarea = getId(tarea);
        int numDeps = getNumDeps(tarea);
        int *deps = getDependencies(tarea);

        // Comprueba si las dependencias estás satisfechas
        for (int i = 0; i < numDeps; i++)
        {
            int dep = deps[i];
            if (dep != idTarea && pendientes.find(dep) != pendientes.end())
            {
                return false;
            }
        }
    }

    return true;
}

// Asigna una tarea para su ejecución en el dispositivo
void PlatUsage::asignarTareas(vector<Ejecucion> tareasPendientes);

// Desaloja la plataforma por completo
void PlatUsage::vaciar()
{
    // Vacía cada uno de los dispositivos
    for (std::vector<DevUsage>::iterator it = dispositivos.begin(); it != dispositivos.end(); ++it)
    {
        it->vaciar();
    }
}

// Devuelve el número de instrucciones que aún puede ejecutar el dispositivo
int PlatUsage::getCapacidad()
{
    int capacidad = 0;
    for (auto dev : dispositivos)
    {
        capacidad += dev.getCapacidad();
    }
    return capacidad;
}

// Deshace las tareas que puedan estar ejecutandose dentro del cluster
void PlatUsage::deshacerEjecucion(vector<int> &instruccionesEjecutadas, vector<Ejecucion> &tareasPendientes);

// Devuelve las instrucciones que está ejecutando la tarea t
int PlatUsage::getInstruccionesEjecutadas(Task t)
{
    int numInst = 0;

    for (auto dev : dispositivos)
    {
        numInst += dev.getInstruccionesEjecutadas(t);
    }

    return numInst;
}

// Comprueba que se pueda ejecutar con HT la tarea t
bool PlatUsage::isHTAplicable(Task t);

// Comprueba que se pueda ejecutar con HT la tarea t
void PlatUsage::printInfo()
{
    for (std::vector<DevUsage>::iterator it = dispositivos.begin(); it != dispositivos.end(); ++it)
    {
        DevUsage uso = *it;
        uso.printInfo();
    }
}