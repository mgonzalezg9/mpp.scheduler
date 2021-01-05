#include <stdio.h>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <deque>

#include "../include/platform.h"
#include "../include/task.h"
#include "../include/ejecucion.hpp"
#include "../include/dev_usage.hpp"

#define NIVEL_INEXPLORADO -1

using namespace std;

// Construye la simulación de una plataforma sin tareas ejecutandose
void buildPlataformaInicio(vector<DevUsage> &simulacion, Platform *cluster)
{
    for (int i = 0; i < getNumDevices(cluster); i++)
    {
        Device device = getDevice(cluster, i);
        DevUsage dev(device);
        simulacion.push_back(dev);
    }
}

vector<vector<Ejecucion>> getPermutaciones(vector<Ejecucion> tareas)
{
    vector<vector<Ejecucion>> perms;
    return perms;
}

// Esquema de backtracking

void generar(int &nivel, vector<vector<DevUsage>> s, vector<int> &hermanosRestantes, vector<Ejecucion> &tareasEjecutar, deque<vector<Ejecucion>> &permutaciones)
{
    if (hermanosRestantes[nivel] == NIVEL_INEXPLORADO)
    {
        vector<vector<Ejecucion>> nuevas = getPermutaciones(tareasEjecutar);

        for (auto permutacion : nuevas)
        {
            permutaciones.push_front(permutacion);
        }
        hermanosRestantes[nivel] = nuevas.size();
    }

    // TODO
}

bool solucion(int nivel, vector<vector<DevUsage>> s)
{
    return nivel == 2;
}

bool criterio(int nivel, vector<vector<DevUsage>> s)
{
    return nivel == 1;
}

bool masHermanos(int nivel, vector<vector<DevUsage>> s)
{
    return false;
}

void retroceder(int &nivel, vector<vector<DevUsage>> s)
{
    nivel--;
}

void get_solution(Task *tasks, int n_tasks, Platform *platform, Task *sorted_tasks, Platform *selected_devs, double time, double energy)
{
    // Inicialización
    int nivel = 1;

    vector<vector<DevUsage>> s(2);
    vector<DevUsage> inicio;
    buildPlataformaInicio(inicio, platform);
    s[0] = inicio;
    s[1] = inicio;

    double t, e;
    t = e = 0.0;

    double toa, eoa;
    toa = eoa = INT_MAX;
    vector<vector<DevUsage>> soa;

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
            // vector<DevUsage> inicio;
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

    // nivel = 0;
    // for (std::vector<vector<DevUsage>>::iterator it1 = soa.begin(); it1 != soa.end(); ++it1)
    // {
    //     cout << "Nuevo nivel" << endl;
    //     for (std::vector<DevUsage>::iterator it2 = it1->begin(); it2 != it1->end(); ++it2)
    //     {
    //         DevUsage uso = *it2;
    //         if (nivel == 0)
    //         {
    //             it2->activarHT();
    //         }
    //         cout << it2->isHTActivado() << endl;
    //     }
    //     nivel++;
    // }
}
