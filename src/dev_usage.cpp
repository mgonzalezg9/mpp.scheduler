#include "../include/dev_usage.hpp"
#include <algorithm>
#include <set>
#include <iostream>

DevUsage::DevUsage(Device dispositivo)
{
    this->dispositivo = dispositivo;
    this->ht = false;
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

// Imprime información sobre el dispositivo para depuración
void DevUsage::printInfo()
{
    cout << "Dev " << dispositivo.id << endl;
    cout << "Núm. cores: " << getNumCores(dispositivo) << endl;

    for (auto t : getTareas())
    {
        cout << "T" << getId(t) << ": {(" << getCores(t) << ") => " << getInstruccionesEjecutadas(t) << endl;
    }
}