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
    // for (auto dev : dispositivos)
    // {
    //     cout << getDevId(dev.getDispositivo()) << " ";
    // }
    // cout << endl;
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

    // Comprueba que la tarea a ejecutar no depende de una que se esté ejecutando ahora
    int numDeps = getNumDeps(t);
    int *dependencias = getDependencies(t);
    for (int i = 0; i < numDeps; i++)
    {
        if (isEjecutando(dependencias[i]))
        {
            // No puede ejecutarse en este momento
            return 0;
        }
    }

    for (vector<DevUsage>::iterator it = dispositivos.begin(); it != dispositivos.end(); ++it)
    {
        int numEjecutadas = it->asignarCores(t, numPendientes, ht);
        // cout << "T" << t.id << ": " << numEjecutadas << " ocupada: " << it->isOcupado() << endl;

        ejecutadasTotal += numEjecutadas;
        numPendientes -= numEjecutadas;

        if ((ht && numEjecutadas > 0) || numPendientes == 0)
        {
            // No tengo más instrucciones para ejecutar en otros dispositivos
            break;
        }
    }

    return ejecutadasTotal;
}

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

// Devuelve el tiempo global que supone el uso actual de la plataforma
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

// Devuelve si se podría ejecutar la tarea t en la plataforma, según sus dependencias
// bool PlatUsage::isRealizable(Task t, vector<Ejecucion> tareasPendientes)
// {
//     int idTarea = getId(t);
//     int numDeps = getNumDeps(t);
//     int *deps = getDependencies(t);
//     set<int> pendientes = Ejecucion::getIds(tareasPendientes);

//     for (int i = 0; i < numDeps; i++)
//     {
//         int dep = deps[i];
//         // Comprueba que la dependencia ni esté pendiente de ejecutarse ni se esté ejecutando en este momento
//         if (dep != idTarea && (pendientes.find(dep) != pendientes.end() || isEjecutando(dep)))
//         {
//             return false;
//         }
//     }

//     return true;
// }

// Asigna una tarea para su ejecución en el dispositivo
void PlatUsage::asignarTareas(vector<Ejecucion> &tareasActuales, deque<vector<Ejecucion>> &permutaciones, vector<int> &instEjecutadas, vector<Ejecucion> &tareasPendientes, int &hermanosRestantes)
{
    int numAsignadas = 0;
    bool versionHT = false;
    Task tareaHT;

    for (int i = 0; i < (int)tareasActuales.size() && getCapacidad() > 0; i++)
    {
        Task t = tareasActuales[i].getTarea();
        bool ht = tareasActuales[i].isHT();
        int numPendientes = getInstRestantes(t, instEjecutadas[getId(t)]);

        // Añade a la pila la versión con HT
        if (!ht && isHTAplicable(t, numPendientes))
        {
            tareaHT = t;
            vector<Ejecucion> combinacionHT = Ejecucion::getHTVersion(tareaHT, tareasActuales);
            permutaciones.push_front(combinacionHT);
            hermanosRestantes++;
            versionHT = true;
        }

        // Coge la tarea y actualiza las instrucciones pendientes
        int numEjecutadas = asignarCores(t, numPendientes, ht);
        // cout << "Se han ejecutado " << numEjecutadas << endl;
        instEjecutadas[getId(t)] += numEjecutadas;

        if (isFinalizada(t, instEjecutadas[getId(t)]))
        {
            Ejecucion::remove(t, tareasPendientes);
        }

        if (numEjecutadas > 0)
        {
            numAsignadas++;
        }
    }

    if (versionHT && numAsignadas == 1)
    {
        // Borra la de HT, explorando solo esa posibilidad
        vector<Ejecucion> secuenciaHT = permutaciones.front();
        permutaciones.pop_front();
        hermanosRestantes--;

        // Deshace los cambios de la version sin HT
        deshacerEjecucion(instEjecutadas, tareasPendientes);

        int numPendientes = getInstRestantes(tareaHT, instEjecutadas[getId(tareaHT)]);
        int numEjecutadas = asignarCores(tareaHT, numPendientes, true);
        instEjecutadas[getId(tareaHT)] += numEjecutadas;

        if (isFinalizada(tareaHT, instEjecutadas[getId(tareaHT)]))
        {
            Ejecucion::remove(tareaHT, tareasPendientes);
        }
    }

    // cout << "Capacidad restante: " << getCapacidad() << endl;
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
    for (std::vector<DevUsage>::iterator it = dispositivos.begin(); it != dispositivos.end(); ++it)
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
    for (auto dev : dispositivos)
    {
        if (dev.isEjecutando(idTarea))
        {
            return true;
        }
    }
    return false;
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

// Comprueba que se pueda ejecutar con HT la tarea t
void PlatUsage::printInfo()
{
    for (std::vector<DevUsage>::iterator it = dispositivos.begin(); it != dispositivos.end(); ++it)
    {
        DevUsage uso = *it;
        uso.printInfo();
    }
}