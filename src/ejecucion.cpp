#include "../include/ejecucion.hpp"

set<int> Ejecucion::getIds(vector<Ejecucion> tareas)
{
    set<int> ids;
    for (auto ejecucion : tareas)
    {
        ids.insert(getId(ejecucion.getTarea()));
    }
    return ids;
}

vector<int> Ejecucion::getListIds(vector<Ejecucion> tareas)
{
    vector<int> ids;
    for (auto ejecucion : tareas)
    {
        ids.push_back(getId(ejecucion.getTarea()));
    }
    return ids;
}

vector<Ejecucion> Ejecucion::crearTareas(Task *tareas, int n_tasks)
{
    vector<Ejecucion> ejecuciones;

    for (int i = 0; i < n_tasks; i++)
    {
        Ejecucion ej(tareas[i], true);
        ejecuciones.push_back(ej);
    }

    return ejecuciones;
}

vector<Ejecucion> Ejecucion::crearTareas(set<Task> tareas)
{
    vector<Ejecucion> resultado;
    for (auto t : tareas)
    {
        Ejecucion ej(t, true);
        resultado.push_back(ej);
    }
    return resultado;
}

vector<Ejecucion> Ejecucion::crearTareas(vector<int> ids, Task *sortedTasks)
{
    vector<Ejecucion> resultado;

    for (auto id : ids)
    {
        Task t = sortedTasks[id];
        Ejecucion ej(t, false);

        resultado.push_back(ej);
    }

    return resultado;
}

vector<Ejecucion> Ejecucion::getNoHTVersion(Task t, vector<Ejecucion> tareas)
{
    vector<Ejecucion> htVersion;

    for (auto ejecucion : tareas)
    {
        if (ejecucion.getTarea() == t)
        {
            Ejecucion ej(t, false);
            htVersion.push_back(ej);
        }
        else
        {
            htVersion.push_back(ejecucion);
        }
    }

    return htVersion;
}

bool Ejecucion::remove(Task t, vector<Ejecucion> &tareas)
{
    for (vector<Ejecucion>::iterator it = tareas.begin(); it != tareas.end(); ++it)
    {
        if (it->getTarea() == t)
        {
            tareas.erase(it);
            return true;
        }
    }
    return false;
}

bool Ejecucion::isPresente(vector<Ejecucion> combinacion, vector<vector<Ejecucion>> combinaciones)
{
    for (auto elemento : combinaciones)
    {
        if (elemento == combinacion)
        {
            return true;
        }
    }
    return false;
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

void Ejecucion::activarHT()
{
    ht = true;
}