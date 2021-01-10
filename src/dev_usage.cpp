#include "../include/dev_usage.hpp"
#include <algorithm>
#include <set>
#include <iostream>
#include <cmath>

#define FACTOR_HYPERTHREADING 2

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

int DevUsage::getIdDispositivo()
{
    return getDevId(dispositivo);
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
    return !cores_tarea.empty();
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
    if (ht)
    {
        return getHyperConsumption(dispositivo);
    }
    else
    {
        return getCoresOcupados() * getConsumptionCore(dispositivo);
    }
}

// Asigna una tarea para su ejecución en el dispositivo
void DevUsage::asignarTarea(Ejecucion tarea, int numCores)
{
    asignarTarea(tarea.getTarea(), numCores);
}

// Asigna una tarea para su ejecución en el dispositivo
void DevUsage::asignarTarea(Task tarea, int numCores)
{
    if (numCores > 0)
    {
        cores_tarea[tarea] = numCores;
    }

    // cout << numCores << " " << cores_tarea[tarea] << endl;
}

// Devuelve los cores que ocupa la tarea t
int DevUsage::getCores(Task t)
{
    return cores_tarea[t];
}

// Devuelve las instrucciones que está ejecutando la tarea t
int DevUsage::getInstruccionesEjecutadas(Task t)
{
    int numInstrucciones = getCores(t) * getInstCore(dispositivo);
    if (ht)
    {
        numInstrucciones *= FACTOR_HYPERTHREADING;
    }

    return numInstrucciones;
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

// Devuelve el número de instrucciones que aún puede ejecutar el dispositivo si se habilita HT
int DevUsage::getCapacidadHT()
{
    return getCapacidad() * FACTOR_HYPERTHREADING;
}

// Deshace las tareas que puedan estar ejecutandose dentro del cluster
void DevUsage::deshacerEjecucion(vector<int> &instruccionesEjecutadas, vector<Ejecucion> &tareasPendientes)
{
    set<int> pendientes = Ejecucion::getIds(tareasPendientes);

    for (map<Task, int>::iterator it = cores_tarea.begin(); it != cores_tarea.end(); ++it)
    {
        Task tarea = it->first;
        int idTarea = getId(tarea);

        int instrucciones = getInstruccionesEjecutadas(tarea);
        instruccionesEjecutadas[idTarea] -= instrucciones;

        if (pendientes.find(idTarea) == pendientes.end())
        {
            Ejecucion ej(tarea, false);
            tareasPendientes.push_back(ej);
        }
    }

    // Reset del mapa
    cores_tarea.clear();
    ht = false;
}

// Devuelve si una tarea t se está ejecutando en el dispositivo
bool DevUsage::isEjecutando(Task t)
{
    auto it = cores_tarea.find(t);
    return it != cores_tarea.end();
}

// Devuelve si una tarea con identiicador t se está ejecutando en el dispositivo
bool DevUsage::isEjecutando(int idTarea)
{
    for (map<Task, int>::iterator it = cores_tarea.begin(); it != cores_tarea.end(); ++it)
    {
        Task tarea = it->first;
        if (getId(tarea) == idTarea)
        {
            return true;
        }
    }
    return false;
}

// Devuelve si el dispositivo permitiria aplicar HT para el numero de instrucciones
bool DevUsage::isHTAplicable(int numInstrucciones)
{
    return !isOcupado() && numInstrucciones == getCapacidadHT();
}

// Asigna la mayor cantidad posible de cores a la tarea para que ejecute las instrucciones que tiene pendientes
// Devuelve el número de instrucciones que ha podido asignarle
// El parámetro ht indica si la tarea en cuestión está planificada para ejecutarse con HT
int DevUsage::asignarCores(Task tarea, int numPendientes, bool ht)
{
    int instCore = getInstCore(dispositivo);
    int coresLibres = getCoresLibres();
    int instAsignadas = 0;

    if (ht)
    {
        if (isHTAplicable(numPendientes))
        {
            asignarTarea(tarea, coresLibres);
            instAsignadas = coresLibres * instCore * FACTOR_HYPERTHREADING;
            this->ht = true;
        }
        return instAsignadas;
    }
    else
    {
        // Calcula el número de cores necesarios
        int coresNecesarios = ceil(((double)numPendientes) / instCore);
        // cout << "Necesita " << ((double)numPendientes) / instCore << " cores" << endl;

        if (coresNecesarios > coresLibres)
        {
            asignarTarea(tarea, coresLibres);
            instAsignadas = coresLibres * instCore;
        }
        else
        {
            asignarTarea(tarea, coresNecesarios);
            instAsignadas = numPendientes;
        }
    }

    return instAsignadas;
}

// Imprime información sobre el dispositivo para depuración
void DevUsage::printInfo()
{
    cout << "Dev " << getDevId(dispositivo) << endl;
    cout << "\tNúm. cores: " << getNumCores(dispositivo) << endl;
    if (ht)
    {
        cout << "\t*HT*" << endl;
    }

    vector<Task> tareas = getTareas();
    for (auto t : tareas)
    {
        cout << "\t\tT" << getId(t) << ": {(" << getCores(t) << ") => " << getInstruccionesEjecutadas(t) << "}" << endl;
    }
}