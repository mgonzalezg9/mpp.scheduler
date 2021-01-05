#ifndef DEV_USAGE_H
#define DEV_USAGE_H

#include <map>
#include <vector>
#include "task.h"
#include "ejecucion.h"
#include "platform.h"

using namespace std;

class DevUsage
{
public:
    // Constructores
    DevUsage(Device dispositivo);
    ~DevUsage();

    // Getters
    map<Task, int> DevUsage::getCores_tarea();
    bool DevUsage::isHTActivado();
    Device DevUsage::getDispositivo();

    // Devuelve las tareas que se están ejecutando
    vector<Task> DevUsage::getTareas();

    // Devuelve si el dispositivo ejecuta alguna tarea
    bool DevUsage::isOcupado();

    // Devuelve el incremento en el tiempo que supone el uso actual del dispositivo
    double DevUsage::getTiempo();

    // Devuelve el incremento de energía que supone el uso actual del dispositivo
    double DevUsage::getEnergia();

    // Devuelve si es consistente el estado del dispositivo (las dependencias de las tareas están satisfechas)
    bool DevUsage::isRealizable(vector<Ejecucion> tareasPendientes);

    // Asigna una tarea para su ejecución en el dispositivo
    void DevUsage::asignarTarea(Ejecucion tarea, int numCores);

    // Devuelve los cores que ocupa la tarea t
    int DevUsage::getCores(Task t);

    // Limpia el mapa con la asociación de cores para cada tarea
    void DevUsage::vaciar();

    // Devuelve el número de instrucciones que aún puede ejecutar el dispositivo
    int DevUsage::getCapacidad();

    // Deshace las tareas que puedan estar ejecutandose dentro del cluster
    void DevUsage::deshacerEjecucion(vector<int> &instruccionesEjecutadas, vector<Ejecucion> &tareasPendientes);

    // Devuelve el número de cores ocupados
    int DevUsage::getCoresOcupados();

    // Devuelve el número de cores libres
    int DevUsage::getCoresLibres();

    // Devuelve las instrucciones que está ejecutando la tarea t
    int DevUsage::getInstruccionesEjecutadas(Task t);

private:
    map<Task, int> cores_tarea;
    bool ht;
    Device dispositivo;
};

#endif // DEV_USAGE_H