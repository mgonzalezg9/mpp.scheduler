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
map<Task, int> DevUsage::getInstTarea()
{
    return instTarea;
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

set<Task> DevUsage::getTareas()
{
    set<Task> keys;
    for (map<Task, int>::iterator it = instTarea.begin(); it != instTarea.end(); ++it)
    {
        keys.insert(it->first);
    }
    return keys;
}

// Devuelve si el dispositivo ejecuta alguna tarea
bool DevUsage::isOcupado()
{
    return !instTarea.empty();
}

// Devuelve el incremento en el tiempo que supone el uso actual del dispositivo
double DevUsage::getTiempo()
{
    double maxTiempo = 0.0;
    for (map<Task, int>::iterator it = instTarea.begin(); it != instTarea.end(); ++it)
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
void DevUsage::asignarTarea(Ejecucion tarea, int numInstrucciones)
{
    asignarTarea(tarea.getTarea(), numInstrucciones);
}

// Asigna una tarea para su ejecución en el dispositivo
void DevUsage::asignarTarea(Task tarea, int numInstrucciones)
{
    if (numInstrucciones > 0)
    {
        instTarea[tarea] = numInstrucciones;
    }
}

// Devuelve los cores que ocupa la tarea t
int DevUsage::getCores(Task t)
{
    if (ht)
    {
        return getNumCores(dispositivo);
    }
    else
    {
        return getInstruccionesEjecutadas(t) / getInstCore(dispositivo);
    }
}

// Devuelve las instrucciones que está ejecutando la tarea t
int DevUsage::getInstruccionesEjecutadas(Task t)
{
    return instTarea[t];
}

// Devuelve el número de instrucciones ejecutadas
int DevUsage::getInstruccionesEjecutadas()
{
    int instrucciones = 0;
    for (map<Task, int>::iterator it = instTarea.begin(); it != instTarea.end(); ++it)
    {
        instrucciones += it->second;
    }
    return instrucciones;
}

// Devuelve el número de cores ocupados
int DevUsage::getCoresOcupados()
{
    if (ht)
    {
        return getNumCores(dispositivo);
    }
    else
    {
        return getInstruccionesEjecutadas() / getInstCore(dispositivo);
    }
}

// Devuelve el número de cores libres
int DevUsage::getCoresLibres()
{
    return getNumCores(dispositivo) - getCoresOcupados();
}

// Devuelve el número de instrucciones que aún puede ejecutar el dispositivo
int DevUsage::getCapacidad()
{
    return getNumCores(dispositivo) * getInstCore(dispositivo) - getInstruccionesEjecutadas();
}

// Devuelve el número de instrucciones que puede ejecutar el dispositivo si se habilita HT
int DevUsage::getCapacidadHT()
{
    return getNumCores(dispositivo) * getInstCore(dispositivo) * FACTOR_HYPERTHREADING;
}

// Deshace las tareas que puedan estar ejecutandose dentro del cluster
void DevUsage::deshacerEjecucion(vector<int> &instruccionesEjecutadas, vector<Ejecucion> &tareasPendientes)
{
    set<int> pendientes = Ejecucion::getIds(tareasPendientes);

    for (map<Task, int>::iterator it = instTarea.begin(); it != instTarea.end(); ++it)
    {
        Task tarea = it->first;
        int idTarea = getId(tarea);

        int instrucciones = getInstruccionesEjecutadas(tarea);
        instruccionesEjecutadas[idTarea] -= instrucciones;

        if (pendientes.find(idTarea) == pendientes.end())
        {
            Ejecucion ej(tarea, true);
            tareasPendientes.push_back(ej);
        }
    }

    // Reset
    instTarea.clear();
    ht = false;
}

// Devuelve si una tarea t se está ejecutando en el dispositivo
bool DevUsage::isEjecutando(Task t)
{
    return instTarea.find(t) != instTarea.end();
}

// Devuelve si una tarea con identificador t se está ejecutando en el dispositivo
bool DevUsage::isEjecutando(int idTarea)
{
    Task t;
    t.id = idTarea;
    return isEjecutando(t);
}

// Devuelve si el dispositivo permitiria aplicar HT para el numero de instrucciones
bool DevUsage::isHTAplicable(int numInstrucciones)
{
    return !isHTActivado() && numInstrucciones == getCapacidadHT() && !isOcupado();
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
            instAsignadas = numPendientes;
            asignarTarea(tarea, instAsignadas);
            this->ht = true;
        }
    }
    else
    {
        // Calcula el número de cores necesarios
        int coresNecesarios = ceil(((double)numPendientes) / instCore);

        if (coresNecesarios > coresLibres)
        {
            instAsignadas = coresLibres * instCore;
        }
        else
        {
            instAsignadas = numPendientes;
        }
        asignarTarea(tarea, instAsignadas);
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

    set<Task> tareas = getTareas();
    for (auto t : tareas)
    {
        cout << "\t\tT" << getId(t) << ": {(" << getCores(t) << ") => " << getInstruccionesEjecutadas(t) << "}" << endl;
    }
}