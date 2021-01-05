#include "../include/dev_usage.h"
#include <algorithm>
#include <set>

DevUsage::DevUsage(Device dispositivo)
{
    this->dispositivo = dispositivo;
}

DevUsage::~DevUsage()
{
}

// Getters
map<Task, int> DevUsage::getCores_tarea()
{
    return cores_tarea;
}

bool DevUsage::isHTActivado()
{
    return ht;
}

Device DevUsage::getDispositivo()
{
    return dispositivo;
}

vector<Task> DevUsage::getTareas()
{
    vector<Task> keys;
    for (map<Task, int>::iterator it = cores_tarea.begin(); it != cores_tarea.end(); ++it)
    {
        keys.push_back(it->first);
    }
    return keys;
}

// Devuelve si el dispositivo ejecuta alguna tarea
bool DevUsage::isOcupado()
{
    return cores_tarea.empty();
}

// Devuelve el incremento en el tiempo que supone el uso actual del dispositivo
double DevUsage::getTiempo()
{
    double maxTiempo = 0.0;
    for (map<Task, int>::iterator it = cores_tarea.begin(); it != cores_tarea.end(); ++it)
    {
        Task tarea = it->first;
        int numCores = it->second;

        double tiempo = ((double)getInstruccionesEjecutadas(tarea)) / getNumInst(tarea);
        if (tiempo > maxTiempo)
        {
            maxTiempo = tiempo;
        }
    }
    return maxTiempo;
}

// Devuelve el incremento de energía que supone el uso actual del dispositivo
double DevUsage::getEnergia()
{
    return getCoresOcupados() * getConsumptionCore(dispositivo);
}

// Devuelve si es consistente el estado del dispositivo (las dependencias de las tareas están satisfechas)
bool DevUsage::isRealizable(vector<Ejecucion> tareasPendientes)
{
    set<int> pendientes = Ejecucion::getIds(tareasPendientes);

    for (map<Task, int>::iterator it = cores_tarea.begin(); it != cores_tarea.end(); ++it)
    {
        Task tarea = it->first;
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
void DevUsage::asignarTarea(Ejecucion tarea, int numCores)
{
    cores_tarea[tarea.getTarea()] = numCores;
}

// Devuelve los cores que ocupa la tarea t
int DevUsage::getCores(Task t)
{
    return cores_tarea[t];
}

// Devuelve las instrucciones que está ejecutando la tarea t
int DevUsage::getInstruccionesEjecutadas(Task t)
{
    return getCores(t) * getInstCore(dispositivo);
}

// Limpia el mapa con la asociación de cores para cada tarea
void DevUsage::vaciar()
{
    cores_tarea.clear();
}

// Devuelve el número de cores ocupados
int DevUsage::getCoresOcupados()
{
    int numOcupados = 0;
    for (map<Task, int>::iterator it = cores_tarea.begin(); it != cores_tarea.end(); ++it)
    {
        int numCores = it->second;
        numOcupados += numCores;
    }
    return numOcupados;
}

// Devuelve el número de cores libres
int DevUsage::getCoresLibres()
{
    return getNumCores(dispositivo) - getCoresOcupados();
}

// Devuelve el número de instrucciones que aún puede ejecutar el dispositivo
int DevUsage::getCapacidad()
{
    return getCoresLibres() * getInstCore(dispositivo);
}

// Deshace las tareas que puedan estar ejecutandose dentro del cluster
void DevUsage::deshacerEjecucion(vector<int> &instruccionesEjecutadas, vector<Ejecucion> &tareasPendientes)
{
    int instCore = getInstCore(dispositivo);
    set<int> pendientes = Ejecucion::getIds(tareasPendientes);

    for (map<Task, int>::iterator it = cores_tarea.begin(); it != cores_tarea.end(); ++it)
    {
        Task tarea = it->first;
        int idTarea = getId(tarea);
        int numCoresTarea = it->second;

        int instrucciones = numCoresTarea * instCore;
        instruccionesEjecutadas[idTarea] -= instrucciones;

        if (pendientes.find(getId(tarea)) == pendientes.end())
        {
            Ejecucion ej(tarea, false);
            tareasPendientes.push_back(ej);
        }
    }

    vaciar();
}