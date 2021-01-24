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
    /* Operaciones sobre vectores de Ejecucion */
    // Devuelve el conjunto de identificadores presentes en las tareas pasadas como parámetro
    static set<int> getIds(vector<Ejecucion> tareas);

    // Crea un vector de tareas para la simulación de su ejecución
    static vector<Ejecucion> crearTareas(Task *tareas, int n_tasks);

    // Crea un vector de tareas para la simulación de su ejecución
    static vector<Ejecucion> crearTareas(set<Task> tareas);

    // Devuelve una versión de tareas en la que la tarea t no tiene la opción de usar HT
    static vector<Ejecucion> getNoHTVersion(Task t, vector<Ejecucion> tareas);

    // Borra la tarea t del vector de tareas
    static bool remove(Task t, vector<Ejecucion> &tareas);

    // Comprueba si la combinación se encuentra entre la lista de combinaciones
    static bool isPresente(vector<Ejecucion> combinacion, vector<vector<Ejecucion>> combinaciones);

    Ejecucion(Task tarea, bool ht);
    ~Ejecucion();

    Task getTarea();
    bool isHT();
    void activarHT();

    bool operator<(const Ejecucion &other) const
    {
        return tarea < other.tarea;
    }

    bool operator==(const Ejecucion &other) const
    {
        return tarea == other.tarea;
    }
};

#endif // EJECUCION_H