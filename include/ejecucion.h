#ifndef EJECUCION_H
#define EJECUCION_H

#include "task.h"
#include <set>
#include <vector>

using namespace std;

class Ejecucion
{
public:
    static set<int> Ejecucion::getIds(vector<Ejecucion> tareas);

    Ejecucion(Task tarea, bool ht);
    ~Ejecucion();

    Task Ejecucion::getTarea();
    bool Ejecucion::isHT();

private:
    Task tarea;
    bool ht;
};

#endif // EJECUCION_H