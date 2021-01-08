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

vector<Ejecucion> Ejecucion::crearTareas(Task *tareas, int n_tasks)
{
    vector<Ejecucion> ejecuciones;

    for (int i = 0; i < n_tasks; i++)
    {
        Ejecucion ej(tareas[i], false);
        ejecuciones.push_back(ej);
    }

    return ejecuciones;
}

vector<Ejecucion> Ejecucion::getHTVersion(Task t, vector<Ejecucion> tareas)
{
    vector<Ejecucion> htVersion = tareas;

    for (std::vector<Ejecucion>::iterator it = htVersion.begin(); it != htVersion.end(); ++it)
    {
        if (it->getTarea() == t)
        {
            it->activarHT();
            return htVersion;
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

vector<Ejecucion> Ejecucion::validar(vector<Ejecucion> tareas)
{
    vector<Ejecucion> resultado;
    set<int> pendientes = Ejecucion::getIds(tareas);

    for (vector<Ejecucion>::iterator it = tareas.begin(); it != tareas.end(); ++it)
    {
        Task t = it->getTarea();
        int idTarea = getId(t);
        int numDeps = getNumDeps(t);
        int *deps = getDependencies(t);

        bool valida = true;
        for (int i = 0; i < numDeps; i++)
        {
            int dep = deps[i];
            // Comprueba que la dependencia no esté pendiente de ejecutarse
            if (dep != idTarea && pendientes.find(dep) != pendientes.end())
            {
                valida = false;
                break;
            }
        }

        if (valida)
        {
            resultado.push_back(*it);
        }
    }

    return resultado;
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