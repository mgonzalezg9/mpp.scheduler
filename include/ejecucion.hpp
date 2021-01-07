#ifndef EJECUCION_H
#define EJECUCION_H

#include "task.hpp"
#include <set>
#include <vector>

using namespace std;

class Ejecucion
{
private:
    Task tarea;
    bool ht;

public:
    // Devuelve el conjunto de identificadores presentes en las tareas pasadas como parámetro
    static set<int> getIds(vector<Ejecucion> tareas);

    // Crea un vector de tareas para la simulación de su ejecución
    static vector<Ejecucion> crearTareas(Task *tareas, int n_tasks);

    // Devuelve una versión de tareas en la que la tarea t tiene activado HT
    static vector<Ejecucion> getHTVersion(Task t, vector<Ejecucion> tareas);

    // Borra la tarea t del vector de tareas
    static bool remove(Task t, vector<Ejecucion> &tareas);

    Ejecucion(Task tarea, bool ht);
    ~Ejecucion();

    Task getTarea();
    bool isHT();
    void activarHT();

    bool operator<(const Ejecucion &other) const
    {
        return tarea < other.tarea;
    }
};

#endif // EJECUCION_H