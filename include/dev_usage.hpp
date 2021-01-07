#ifndef DEV_USAGE_H
#define DEV_USAGE_H

#include <map>
#include <vector>
#include "task.hpp"
#include "ejecucion.hpp"
#include "platform.hpp"

using namespace std;

class DevUsage
{
private:
    map<Task, int> cores_tarea;
    bool ht;
    Device dispositivo;

    // Asigna una tarea para su ejecución en el dispositivo
    void asignarTarea(Ejecucion tarea, int numCores);

    // Asigna una tarea para su ejecución en el dispositivo
    void asignarTarea(Task tarea, int numCores);

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

    // Asigna la mayor cantidad posible de cores a la tarea para que ejecute las instrucciones que tiene pendientes
    // Devuelve el número de instrucciones que ha podido asignarle
    int asignarCores(Task tarea, int numPendientes, bool ht);

    // Devuelve los cores que ocupa la tarea t
    int getCores(Task t);

    // Limpia el mapa con la asociación de cores para cada tarea
    void vaciar();

    // Devuelve el número de instrucciones que aún puede ejecutar el dispositivo
    int getCapacidad();

    // Devuelve el número de instrucciones que aún puede ejecutar el dispositivo si se habilita HT
    int getCapacidadHT();

    // Deshace las tareas que puedan estar ejecutandose dentro del cluster
    void deshacerEjecucion(vector<int> &instruccionesEjecutadas, vector<Ejecucion> &tareasPendientes);

    // Devuelve el número de cores ocupados
    int getCoresOcupados();

    // Devuelve el número de cores libres
    int getCoresLibres();

    // Devuelve las instrucciones que está ejecutando la tarea t
    int getInstruccionesEjecutadas(Task t);

    // Devuelve si una tarea t se está ejecutando en el dispositivo
    bool isEjecutando(Task t);

    // Devuelve si el dispositivo permitiria aplicar HT para el numero de instrucciones
    bool isHTAplicable(int numInstrucciones);

    // Imprime información sobre el dispositivo para depuración
    void printInfo();

    // Ordena los dispositivos según su consumo
    bool operator<(const DevUsage &other)
    {
        return dispositivo < other.dispositivo;
    }
};

#endif // DEV_USAGE_H