#include <stdio.h>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <deque>
#include <algorithm>

#include "../include/platform.h"
#include "../include/task.h"
#include "../include/ejecucion.hpp"
#include "../include/dev_usage.hpp"
#include "../include/plat_usage.hpp"

#define NIVEL_INEXPLORADO -1

using namespace std;

// Funciones auxiliares

vector<vector<Ejecucion>> getPermutaciones(vector<Ejecucion> tareas)
{
    vector<vector<Ejecucion>> perms;

    // Ordenado al principio
    sort(tareas.begin(), tareas.end());

    do
    {
        perms.push_back(tareas);
    } while (next_permutation(tareas.begin(), tareas.end()));

    return perms;
}

// Devuelve si alguien usa alguna parte de la plataforma
bool isOcupado(vector<DevUsage> plataforma)
{
    for (auto dev : plataforma)
    {
        if (dev.isOcupado())
        {
            return true;
        }
    }
    return false;
}

// Devuelve si alguien usa alguna parte de la plataforma
double getTiempo(vector<DevUsage> plataforma)
{
    for (auto dev : plataforma)
    {
        if (dev.isOcupado())
        {
            return true;
        }
    }
    return false;
}

// Esquema de backtracking

void generar(int &nivel, vector<PlatUsage> s, vector<int> &hermanosRestantes, vector<Ejecucion> &tareasEjecutar, deque<vector<Ejecucion>> &permutaciones, int &tact, int &eact, vector<int> &instEjecutadas)
{
    // Genera las permutaciones
    if (hermanosRestantes[nivel] == NIVEL_INEXPLORADO)
    {
        vector<vector<Ejecucion>> nuevas = getPermutaciones(tareasEjecutar);

        for (auto permutacion : nuevas)
        {
            permutaciones.push_front(permutacion);
        }
        hermanosRestantes[nivel] = nuevas.size();
    }

    // Coge la siguiente secuencia en la cola
    vector<Ejecucion> secuencia = permutaciones.front();
    permutaciones.pop_front();
    hermanosRestantes[nivel]--;

    // Limpia el valor generado anteriormente
    if (s[nivel].isOcupado())
    {
        tact -= s[nivel].getTiempo();
        eact -= s[nivel].getEnergia();
        s[nivel].deshacerEjecucion(instEjecutadas, tareasEjecutar);
    }

    s[nivel].asignarTareas(secuencia);
    tact += s[nivel].getTiempo();
    eact += s[nivel].getEnergia();
}

bool solucion(int nivel, vector<PlatUsage> s)
{
    return nivel == 2;
}

bool criterio(int nivel, vector<PlatUsage> s)
{
    return nivel == 1;
}

bool masHermanos(int nivel, vector<PlatUsage> s)
{
    return false;
}

void retroceder(int &nivel, vector<PlatUsage> s)
{
    nivel--;
}

void get_solution(Task *tasks, int n_tasks, Platform *platform, Task *sorted_tasks, Platform *selected_devs, double time, double energy)
{
    // Inicialización
    int nivel = 1;

    vector<PlatUsage> s(2);
    PlatUsage inicio(platform);
    s[0] = inicio;
    s[1] = inicio;

    double t, e;
    t = e = 0.0;

    double toa, eoa;
    toa = eoa = INT_MAX;
    vector<PlatUsage> soa;

    vector<int> hermanosRestantes(2);
    hermanosRestantes[0] = NIVEL_INEXPLORADO;
    hermanosRestantes[1] = NIVEL_INEXPLORADO;

    deque<vector<Ejecucion>> permutaciones;

    // Algoritmo
    do
    {
        generar(nivel, s);
        if (solucion(nivel, s) && t < toa && e <= eoa)
        {
            toa = t;
            eoa = e;
            soa = s;
        }

        if (criterio(nivel, s))
        {
            // PlatUsage inicio;
            // buildPlataformaInicio(inicio, platform);
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
            while (!masHermanos(nivel, s) && nivel > 0)
            {
                retroceder(nivel, s);
            }
        }

    } while (nivel > 0);
}
