#include <stdio.h>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <deque>
#include <algorithm>
#include <omp.h>

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
    int numHilos = atoi(getenv("OMP_NUM_THREADS"));
    int nivel[numHilos];

    vector<PlatUsage> nivelesIniciales(2);
    PlatUsage inicio(platform);
    nivelesIniciales[0] = inicio;
    nivelesIniciales[1] = inicio;

    vector<PlatUsage> s[numHilos];

    double tact[numHilos];
    double eact[numHilos];

    double toa[numHilos];
    double eoa[numHilos];

    vector<PlatUsage> soa[numHilos];

    vector<int> hermanosIniciales(2);
    hermanosIniciales[0] = NIVEL_INEXPLORADO;
    hermanosIniciales[1] = NIVEL_INEXPLORADO;

    vector<int> hermanosRestantes[numHilos];

    vector<int> instEjecutadasInicio(n_tasks, 0);
    vector<int> instEjecutadas[numHilos];

    deque<vector<Ejecucion>> permutaciones[numHilos];

    vector<Ejecucion> tareas = Ejecucion::crearTareas(tasks, n_tasks);
    vector<Ejecucion> tareasPendientes[numHilos];

    // Genera el primer nivel para cada uno de los hilos
    vector<vector<Ejecucion>> hermanosPrimerNivel = getPermutaciones(tareas, inicio);
    int numHermanos = hermanosPrimerNivel.size();

#pragma omp parallel num_threads(numHilos)
    {
        int tid = omp_get_thread_num();

        // Inicialización de los datos
        nivel[tid] = 1;
        s[tid] = nivelesIniciales;
        tact[tid] = eact[tid] = 0.0;
        toa[tid] = eoa[tid] = INT_MAX;
        hermanosRestantes[tid] = hermanosIniciales;
        instEjecutadas[tid] = instEjecutadasInicio;
        tareasPendientes[tid] = tareas;

        // Asigna subárboles a cada hilo
        int hermanoInicio = tid * numHermanos / numHilos;
        int hermanoFin = (tid + 1) * numHermanos / numHilos;

        permutaciones[tid].insert(permutaciones[tid].end(), &hermanosPrimerNivel[hermanoInicio], &hermanosPrimerNivel[hermanoFin]);
        hermanosRestantes[tid][1] = hermanoFin - hermanoInicio;

        // Algoritmo
        do
        {
            generar(nivel[tid], s[tid], hermanosRestantes[tid], tareasPendientes[tid], permutaciones[tid], tact[tid], eact[tid], instEjecutadas[tid], s[tid][nivel[tid] - 1]);

            // cout << "Nivel " << nivel << endl;
            // s[nivel].printInfo();
            // cout << "--------------------------------" << endl;

            if (solucion(tareasPendientes[tid]) && tact[tid] <= toa[tid] && eact[tid] <= eoa[tid])
            {
                toa[tid] = tact[tid];
                eoa[tid] = eact[tid];
                soa[tid] = s[tid];
            }

            if (criterio(tareasPendientes[tid]))
            {
                s[tid].push_back(inicio);
                nivel[tid]++;

                // Se marca el nuevo nivel como no explorado
                if ((int)hermanosRestantes[tid].size() == nivel[tid])
                {
                    hermanosRestantes[tid].push_back(NIVEL_INEXPLORADO);
                }
            }
            else
            {
                while (!masHermanos(nivel[tid], hermanosRestantes[tid]) && nivel[tid] > 0)
                {
                    retroceder(nivel[tid], s[tid], tact[tid], eact[tid], instEjecutadas[tid], tareasPendientes[tid]);
                }
            }
        } while (nivel[tid] > 0);

        // #pragma omp critical
        //         cout << "Soy el thread " << tid << " y obtengo (t = " << toa[tid] << ", e = " << eoa[tid] << ")" << endl;
    }

    // Búsqueda del mejor resultado obtenido
    int hiloSolucion = 0;

    for (int i = 1; i < numHilos; i++)
    {
        if (toa[i] <= toa[hiloSolucion] && eoa[i] <= eoa[hiloSolucion])
        {
            hiloSolucion = i;
        }
    }

    // Reconstrucción de la solución
    time = toa[hiloSolucion];
    energy = eoa[hiloSolucion];
    formatSolucion(soa[hiloSolucion], sorted_tasks, n_tasks, selected_devs, platform->n_devices);
}
