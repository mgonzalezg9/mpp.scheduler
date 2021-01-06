#ifndef EJECUCION_H
#define EJECUCION_H

#include "task.h"
#include <set>
#include <vector>

using namespace std;

class Ejecucion
{
private:
    Task tarea;
    bool ht;

public:
    static set<int> getIds(vector<Ejecucion> tareas);

    Ejecucion(Task tarea, bool ht);
    ~Ejecucion();

    Task getTarea();
    bool isHT();

    bool operator<(const Ejecucion &other) const
    {
        return tarea < other.tarea;
    }
};

#endif // EJECUCION_H