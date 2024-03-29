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

    sort(dispositivos.begin(), dispositivos.end());
}

// Getters
vector<DevUsage> PlatUsage::getDispositivos()
{
    return dispositivos;
}

// Asigna una serie de cores a la tarea t y devuelve las instrucciones ejecutadas
// El parámetro ht indica si se le debe aplicar HT a la tarea
int PlatUsage::asignarCores(Task t, int numPendientes, bool ht)
{
    int ejecutadasTotal = 0;

    for (vector<DevUsage>::iterator it = dispositivos.begin(); it != dispositivos.end(); ++it)
    {
        int numEjecutadas = it->asignarCores(t, numPendientes, ht);

        ejecutadasTotal += numEjecutadas;
        numPendientes -= numEjecutadas;

        if (numPendientes == 0)
        {
            // No tengo más instrucciones para ejecutar en otros dispositivos
            break;
        }
    }

    return ejecutadasTotal;
}

// Devuelve las tareas que se están ejecutando
set<Task> PlatUsage::getTareas()
{
    set<Task> tareas;
    for (auto dev : dispositivos)
    {
        set<Task> devTareas = dev.getTareas();
        tareas.insert(devTareas.begin(), devTareas.end());
    }
    return tareas;
}

// Devuelve las tareas que se están ejecutando
int PlatUsage::getNumTareas()
{
    int numTareas = 0;
    for (auto dev : dispositivos)
    {
        numTareas += dev.getTareas().size();
    }
    return numTareas;
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

// Devuelve el tiempo global que supone el uso actual de la plataforma
double PlatUsage::getTiempo()
{
    double maxTiempo = 0.0;
    set<Task> tareasEjecutadas = getTareas();

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

// Asigna una tarea para su ejecución en el dispositivo
void PlatUsage::asignarTareas(vector<Ejecucion> &tareasActuales, deque<vector<Ejecucion>> &permutaciones, vector<int> &instEjecutadas, vector<Ejecucion> &tareasPendientes, int &hermanosRestantes)
{
    for (int i = 0; i < (int)tareasActuales.size() && getCapacidad() > 0; i++)
    {
        Task t = tareasActuales[i].getTarea();
        bool ht = tareasActuales[i].isHT();
        int numPendientes = getInstRestantes(t, instEjecutadas[getId(t)]);

        // Añade a la pila la versión con HT
        if (ht && isHTAplicable(t, numPendientes))
        {
            vector<Ejecucion> combinacionSinHT = Ejecucion::getNoHTVersion(t, tareasActuales);
            permutaciones.push_front(combinacionSinHT);
            hermanosRestantes++;
        }
        else
        {
            // No se puede ejecutar con HT
            ht = false;
        }

        // Coge la tarea y actualiza las instrucciones pendientes
        int numEjecutadas = asignarCores(t, numPendientes, ht);
        instEjecutadas[getId(t)] += numEjecutadas;

        if (isFinalizada(t, instEjecutadas[getId(t)]))
        {
            Ejecucion::remove(t, tareasPendientes);
        }
    }

    // Se prueba solo con HT al tratarse de una unica tarea
    if (isHTActivado() && getNumTareas() == 1)
    {
        permutaciones.pop_front();
        hermanosRestantes--;
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
void PlatUsage::deshacerEjecucion(vector<int> &instruccionesEjecutadas, vector<Ejecucion> &tareasPendientes)
{
    for (vector<DevUsage>::iterator it = dispositivos.begin(); it != dispositivos.end(); ++it)
    {
        it->deshacerEjecucion(instruccionesEjecutadas, tareasPendientes);
    }
}

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

// Devuelve si se ejecuta la tarea t en la plataforma
bool PlatUsage::isEjecutando(Task t)
{
    for (auto dev : dispositivos)
    {
        if (dev.isEjecutando(t))
        {
            return true;
        }
    }
    return false;
}

// Devuelve si se ejecuta la tarea con identificador idTarea en la plataforma
bool PlatUsage::isEjecutando(int idTarea)
{
    Task t;
    t.id = idTarea;

    return isEjecutando(t);
}

// Recorro los dispositivos que no lo tienen activado aún y compruebo si se podría activar para el numInstrucciones
bool PlatUsage::isHTAplicable(Task t, int numInstrucciones)
{
    for (auto dev : dispositivos)
    {
        if (dev.isHTAplicable(numInstrucciones) && !isEjecutando(t))
        {
            return true;
        }
    }

    return false;
}

// Devuelve si está activado HT en algún dispositivo
bool PlatUsage::isHTActivado()
{
    for (auto dev : dispositivos)
    {
        if (dev.isHTActivado())
        {
            return true;
        }
    }
    return false;
}

// Devuelve la solución en las estructuras de salida
void PlatUsage::getSolucion(map<int, vector<Device>> &ejecuciones, vector<Task> &orden)
{
    for (auto dev : dispositivos)
    {
        set<Task> tareas = dev.getTareas();

        for (auto t : tareas)
        {
            orden.push_back(t);

            Device dispositivo;
            dispositivo.id = dev.getIdDispositivo();
            dispositivo.n_cores = dev.getCores(t);
            dispositivo.hyperthreading = dev.isHTActivado();

            vector<Device> ejs = ejecuciones[getId(t)];
            ejs.push_back(dispositivo);
            ejecuciones[getId(t)] = ejs;
        }
    }
}

// Comprueba que se pueda ejecutar con HT la tarea t
void PlatUsage::printInfo()
{
    for (vector<DevUsage>::iterator it = dispositivos.begin(); it != dispositivos.end(); ++it)
    {
        DevUsage uso = *it;
        uso.printInfo();
    }
}