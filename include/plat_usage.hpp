#ifndef PLAT_USAGE_H
#define PLAT_USAGE_H

#include <vector>
#include "dev_usage.hpp"

using namespace std;

class PlatUsage
{
private:
    vector<DevUsage> dispositivos;

public:
    // Constructores
    PlatUsage(Platform *plataforma);

    // Getters
    vector<DevUsage> getDispositivos();

    // Asigna una serie de cores a la tarea t y devuelve las instrucciones ejecutadas
    int asignarCores(Task t, bool ht);

    // Devuelve las tareas que se están ejecutando
    vector<Task> getTareas();

    // Devuelve si la plataforma ejecuta alguna tarea
    bool isOcupado();

    // Devuelve el tiempo que tarda la tarea t en ejecutarse
    double getTiempo(Task t);

    // Devuelve el incremento en el tiempo que supone el uso actual de la plataforma
    double getTiempo();

    // Devuelve el incremento de energía que supone el uso actual de la plataforma
    double getEnergia();

    // Devuelve si es consistente el estado de la plataforma (las dependencias de las tareas están satisfechas)
    bool isRealizable(vector<Ejecucion> tareasPendientes);

    // Asigna una tarea para su ejecución en el dispositivo
    void asignarTareas(vector<Ejecucion> tareasPendientes);

    // Desaloja la plataforma por completo
    void vaciar();

    // Devuelve el número de instrucciones que aún puede ejecutar el dispositivo
    int getCapacidad();

    // Deshace las tareas que puedan estar ejecutandose dentro del cluster
    void deshacerEjecucion(vector<int> &instruccionesEjecutadas, vector<Ejecucion> &tareasPendientes);

    // Devuelve las instrucciones que está ejecutando la tarea t
    int getInstruccionesEjecutadas(Task t);

    // Comprueba que se pueda ejecutar con HT la tarea t
    bool isHTAplicable(Task t);

    // Imprime información sobre la plataforma para depuración
    void printInfo();
};

#endif // PLAT_USAGE_H