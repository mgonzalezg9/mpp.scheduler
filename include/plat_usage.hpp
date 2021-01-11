#ifndef PLAT_USAGE_H
#define PLAT_USAGE_H

#include <vector>
#include <deque>
#include "dev_usage.hpp"

using namespace std;

class PlatUsage
{
private:
    vector<DevUsage> dispositivos;

public:
    // Constructores
    PlatUsage(Platform *plataforma);
    PlatUsage() = default;

    // Getters
    vector<DevUsage> getDispositivos();

    // Asigna una serie de cores a la tarea t y devuelve las instrucciones ejecutadas
    // El parámetro ht indica si se le debe aplicar HT a la tarea
    int asignarCores(Task t, int numPendientes, bool ht);

    // Devuelve las tareas que se están ejecutando
    set<Task> getTareas();

    // Devuelve si la plataforma ejecuta alguna tarea
    bool isOcupado();

    // Devuelve el tiempo que tarda la tarea t en ejecutarse
    double getTiempo(Task t);

    // Devuelve el tiempo global que supone el uso actual de la plataforma
    double getTiempo();

    // Devuelve el incremento de energía que supone el uso actual de la plataforma
    double getEnergia();

    // Devuelve si se podría ejecutar la tarea t en la plataforma, según sus dependencias
    // bool isRealizable(Task t, vector<Ejecucion> tareasPendientes);

    // Asigna una tarea para su ejecución en el dispositivo
    void asignarTareas(vector<Ejecucion> &tareasActuales, deque<vector<Ejecucion>> &permutaciones, vector<int> &instEjecutadas, vector<Ejecucion> &tareasPendientes, int &hermanosRestantes);

    // Devuelve el número de instrucciones que aún puede ejecutar el dispositivo
    int getCapacidad();

    // Deshace las tareas que puedan estar ejecutandose dentro del cluster
    void deshacerEjecucion(vector<int> &instruccionesEjecutadas, vector<Ejecucion> &tareasPendientes);

    // Devuelve las instrucciones que está ejecutando la tarea t
    int getInstruccionesEjecutadas(Task t);

    // Devuelve si se ejecuta la tarea t en la plataforma
    bool isEjecutando(Task t);

    // Devuelve si se ejecuta la tarea con identificador idTarea en la plataforma
    bool isEjecutando(int idTarea);

    // Comprueba que se pueda ejecutar con HT la tarea t
    // Devuelve el id del dispositivo donde se ejecutaría, -1 si no lo puede aplicar
    bool isHTAplicable(Task t, int numInstrucciones);

    // Devuelve la solución en las estructuras de salida
    void getSolucion(map<int, vector<Device>> &ejecuciones, vector<Task> &orden);

    // Imprime información sobre la plataforma para depuración
    void printInfo();
};

#endif // PLAT_USAGE_H