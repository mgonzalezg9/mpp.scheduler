#include <stdio.h>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <deque>
#include <algorithm>

#include "../include/platform.hpp"
#include "../include/task.hpp"
#include "../include/ejecucion.hpp"
#include "../include/dev_usage.hpp"
#include "../include/plat_usage.hpp"

#define NIVEL_INEXPLORADO 0

using namespace std;

// Funciones auxiliares

// Devuelve una secuencia de tareas que se podrían ejecutar al tener las dependencias resueltas
vector<Ejecucion> validarDependencias(vector<Ejecucion> &tareas, vector<Ejecucion> tareasAnteriores)
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

void diferenciarEmpezadas(vector<Ejecucion> tareas, vector<Ejecucion> tareasAnteriores, vector<Ejecucion> &tareasEmpezadas, vector<Ejecucion> &tareasNoEmpezadas)
{
    // Las tareas ejecutadas en el nivel anterior tienen prioridad
    if (!tareasAnteriores.empty())
    {
        // Calcula la intersección de las tareas anteriores y las planificadas
        sort(tareas.begin(), tareas.end());
        sort(tareasAnteriores.begin(), tareasAnteriores.end());
        set_intersection(tareasAnteriores.begin(), tareasAnteriores.end(),
                         tareas.begin(), tareas.end(),
                         back_inserter(tareasEmpezadas));

        set_difference(tareas.begin(), tareas.end(),
                       tareasEmpezadas.begin(), tareasEmpezadas.end(),
                       inserter(tareasNoEmpezadas, tareasNoEmpezadas.end()));
    }
    else
    {
        tareasNoEmpezadas = tareas;
    }
}

vector<vector<Ejecucion>> getPermutaciones(vector<Ejecucion> tareas, PlatUsage nivelAnterior)
{
    vector<vector<Ejecucion>> perms;

    // Ordenado al principio
    vector<Ejecucion> tareasAnteriores = Ejecucion::crearTareas(nivelAnterior.getTareas());

    // Desecha las tareas cuyas dependencias están sin resolver
    tareas = validarDependencias(tareas, tareasAnteriores);

    // cout << "Tareas sin dependencias" << endl;
    // for (auto ej : tareas)
    // {
    //     cout << "T" << getId(ej.getTarea()) << (ej.isHT() ? "*" : "") << " ";
    // }
    // cout << endl;

    // Separa las tareas según si han empezado ya o no
    vector<Ejecucion> tareasEmpezadas, tareasNoEmpezadas;
    diferenciarEmpezadas(tareas, tareasAnteriores, tareasEmpezadas, tareasNoEmpezadas);

    sort(tareasEmpezadas.begin(), tareasEmpezadas.end());
    sort(tareasNoEmpezadas.begin(), tareasNoEmpezadas.end());
    do
    {
        do
        {
            vector<Ejecucion> combinacion;
            combinacion.insert(combinacion.end(), tareasEmpezadas.begin(), tareasEmpezadas.end());
            combinacion.insert(combinacion.end(), tareasNoEmpezadas.begin(), tareasNoEmpezadas.end());

            perms.push_back(combinacion);
        } while (next_permutation(tareasNoEmpezadas.begin(), tareasNoEmpezadas.end()));
    } while (next_permutation(tareasEmpezadas.begin(), tareasEmpezadas.end()));

    return perms;
}

void formatSolucion(vector<PlatUsage> s, Task *sortedTasks, int nTasks, Platform *selectedDevs, int nDevices)
{
    // Inicialización
    vector<Task> orden;

    vector<Device> listaVacia;
    map<int, vector<Device>> ejecuciones;
    for (int i = 0; i < nTasks; i++)
    {
        ejecuciones[i] = listaVacia;
    }

    // Obtención de la solución para los vectores auxiliares
    for (auto nodo : s)
    {
        nodo.getSolucion(ejecuciones, orden);
    }

    // Copiado al array de tareas ordenadas
    set<Task> tareasInsertadas;
    int cont = 0;
    for (auto it = orden.begin(); it != orden.end(); it++)
    {
        if (tareasInsertadas.find(*it) == tareasInsertadas.end())
        {
            sortedTasks[cont] = *it;
            tareasInsertadas.insert(*it);
            cont++;
        }
    }

    // Copiado al array con el uso de las plataformas
    for (int i = 0; i < nTasks; i++)
    {
        vector<Device> dispositivos = ejecuciones[i];
        selectedDevs[i].n_devices = dispositivos.size();

        for (int j = 0; j < selectedDevs[i].n_devices; j++)
        {
            selectedDevs[i].devices[j] = dispositivos[j];
        }
    }
}

// Esquema de backtracking

void generar(int nivel, vector<PlatUsage> &s, vector<int> &hermanosRestantes, vector<Ejecucion> &tareasPendientes, deque<vector<Ejecucion>> &permutaciones, double &tact, double &eact, vector<int> &instEjecutadas, PlatUsage nivelAnterior)
{
    // Genera las permutaciones
    if (hermanosRestantes[nivel] == NIVEL_INEXPLORADO)
    {
        vector<vector<Ejecucion>> nuevas = getPermutaciones(tareasPendientes, nivelAnterior);

        for (auto permutacion : nuevas)
        {
            permutaciones.push_front(permutacion);
        }
        hermanosRestantes[nivel] = nuevas.size();
    }

    // Coge la siguiente secuencia en la cola
    vector<Ejecucion> secuencia = permutaciones.front();
    permutaciones.pop_front();

    // for (auto ej : secuencia)
    // {
    //     cout << "T" << getId(ej.getTarea()) << (ej.isHT() ? "*" : "") << " ";
    // }
    // cout << endl;

    hermanosRestantes[nivel]--;

    // Limpia el valor generado anteriormente
    if (s[nivel].isOcupado())
    {
        tact -= s[nivel].getTiempo();
        eact -= s[nivel].getEnergia();
        s[nivel].deshacerEjecucion(instEjecutadas, tareasPendientes);
    }

    s[nivel].asignarTareas(secuencia, permutaciones, instEjecutadas, tareasPendientes, hermanosRestantes[nivel]);
    tact += s[nivel].getTiempo();
    eact += s[nivel].getEnergia();
}

bool solucion(vector<Ejecucion> tareasPendientes)
{
    return tareasPendientes.empty();
}

bool criterio(vector<Ejecucion> tareasPendientes)
{
    return !tareasPendientes.empty();
}

bool masHermanos(int nivel, vector<int> hermanosRestantes)
{
    return hermanosRestantes[nivel] > 0;
}

void retroceder(int &nivel, vector<PlatUsage> &s, double &tact, double &eact, vector<int> &instEjecutadas, vector<Ejecucion> &tareasPendientes)
{
    tact -= s[nivel].getTiempo();
    eact -= s[nivel].getEnergia();

    s[nivel].deshacerEjecucion(instEjecutadas, tareasPendientes);

    nivel--;
}

void get_solution(Task *tasks, int n_tasks, Platform *platform, Task *sorted_tasks, Platform *selected_devs, double &time, double &energy)
{
    // Inicialización
    int nivel = 1;

    vector<PlatUsage> s(2);
    PlatUsage inicio(platform);
    s[0] = inicio;
    s[1] = inicio;

    double tact, eact;
    tact = eact = 0.0;

    double toa, eoa;
    toa = eoa = INT_MAX;
    vector<PlatUsage> soa;

    vector<int> hermanosRestantes(2);
    hermanosRestantes[0] = NIVEL_INEXPLORADO;
    hermanosRestantes[1] = NIVEL_INEXPLORADO;

    deque<vector<Ejecucion>> permutaciones;

    vector<Ejecucion> tareasPendientes = Ejecucion::crearTareas(tasks, n_tasks);

    vector<int> instEjecutadas(n_tasks, 0);

    // Algoritmo
    do
    {
        generar(nivel, s, hermanosRestantes, tareasPendientes, permutaciones, tact, eact, instEjecutadas, s[nivel - 1]);

        // cout << "Nivel " << nivel << endl;
        // s[nivel].printInfo();
        // cout << "--------------------------------" << endl;

        if (solucion(tareasPendientes) && tact <= toa && eact <= eoa)
        {
            toa = tact;
            eoa = eact;
            soa = s;
        }

        if (criterio(tareasPendientes))
        {
            s.push_back(inicio);
            nivel++;

            // Se marca el nuevo nivel como no explorado
            if ((int)hermanosRestantes.size() == nivel)
            {
                hermanosRestantes.push_back(NIVEL_INEXPLORADO);
            }
        }
        else
        {
            while (!masHermanos(nivel, hermanosRestantes) && nivel > 0)
            {
                retroceder(nivel, s, tact, eact, instEjecutadas, tareasPendientes);
            }
        }
    } while (nivel > 0);

    // Reconstrucción de la solución
    time = toa;
    energy = eoa;
    formatSolucion(soa, sorted_tasks, n_tasks, selected_devs, platform->n_devices);
}
