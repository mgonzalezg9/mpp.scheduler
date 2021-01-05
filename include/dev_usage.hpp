#ifndef DEV_USAGE_H
#define DEV_USAGE_H

#include <map>
#include <vector>
#include "task.h"
#include "ejecucion.hpp"
#include "platform.h"

using namespace std;

class DevUsage
{
private:
    map<Task, int> cores_tarea;
    bool ht;
    Device dispositivo;

public:
    // Constructores
    DevUsage(Device dispositivo);

    // Getters
    map<Task, int> getCores_tarea();
    bool isHTActivado();
    Device getDispositivo();

    // Devuelve las tareas que se están ejecutando
    vector<Task> getTareas();

    // Devuelve si el dispositivo ejecuta alguna tarea
    bool isOcupado();

    // Devuelve el incremento en el tiempo que supone el uso actual del dispositivo
    double getTiempo();

    // Devuelve el incremento de energía que supone el uso actual del dispositivo
    double getEnergia();

    // Devuelve si es consistente el estado del dispositivo (las dependencias de las tareas están satisfechas)
    bool isRealizable(vector<Ejecucion> tareasPendientes);

    // Asigna una tarea para su ejecución en el dispositivo
    void asignarTarea(Ejecucion tarea, int numCores);

    // Devuelve los cores que ocupa la tarea t
    int getCores(Task t);

    // Limpia el mapa con la asociación de cores para cada tarea
    void vaciar();

    // Devuelve el número de instrucciones que aún puede ejecutar el dispositivo
    int getCapacidad();

    // Deshace las tareas que puedan estar ejecutandose dentro del cluster
    void deshacerEjecucion(vector<int> &instruccionesEjecutadas, vector<Ejecucion> &tareasPendientes);

    // Devuelve el número de cores ocupados
    int getCoresOcupados();

    // Devuelve el número de cores libres
    int getCoresLibres();

    // Devuelve las instrucciones que está ejecutando la tarea t
    int getInstruccionesEjecutadas(Task t);

    void activarHT()
    {
        ht = true;
    }
};

#endif // DEV_USAGE_H