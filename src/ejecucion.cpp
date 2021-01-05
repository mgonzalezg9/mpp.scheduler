#include "../include/ejecucion.h"

set<int> Ejecucion::getIds(vector<Ejecucion> tareas) {
    set<int> ids;
    for(auto ejecucion : tareas) {
        ids.insert(getId(ejecucion.getTarea()));
    }
    return ids;
}

Ejecucion::Ejecucion(Task tarea, bool ht)
{
    this->tarea = tarea;
    this->ht = ht;
}

Ejecucion::~Ejecucion()
{
}

Task Ejecucion::getTarea()
{
    return tarea;
}

bool Ejecucion::isHT()
{
    return ht;
}