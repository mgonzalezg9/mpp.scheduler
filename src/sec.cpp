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

vector<vector<Ejecucion>> getPermutaciones(vector<Ejecucion> tareas)
{
    vector<vector<Ejecucion>> perms;

    // Ordenado al principio
    sort(tareas.begin(), tareas.end());

    // for (auto ej : tareas)
    // {
    //     cout << "T" << getId(ej.getTarea()) << (ej.isHT() ? "*" : "") << " ";
    // }
    // cout << endl;

    do
    {
        vector<Ejecucion> combinacion = Ejecucion::validar(tareas);

        if (!Ejecucion::isPresente(combinacion, perms))
        {
            perms.push_back(combinacion);
        }
    } while (next_permutation(tareas.begin(), tareas.end()));

    return perms;
}

// Esquema de backtracking

void generar(int nivel, vector<PlatUsage> &s, vector<int> &hermanosRestantes, vector<Ejecucion> &tareasPendientes, deque<vector<Ejecucion>> &permutaciones, double &tact, double &eact, vector<int> &instEjecutadas)
{
    // Genera las permutaciones
    if (hermanosRestantes[nivel] == NIVEL_INEXPLORADO)
    {
        vector<vector<Ejecucion>> nuevas = getPermutaciones(tareasPendientes);

        for (auto permutacion : nuevas)
        {
            permutaciones.push_front(permutacion);
        }
        hermanosRestantes[nivel] = nuevas.size();
    }

    // Coge la siguiente secuencia en la cola
    vector<Ejecucion> secuencia = permutaciones.front();
    permutaciones.pop_front();

    for (auto ej : secuencia)
    {
        cout << "T" << getId(ej.getTarea()) << (ej.isHT() ? "*" : "") << " ";
    }
    cout << endl;

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

    // cout << "s[" << nivel << "].isOcupado(): " << s[nivel].isOcupado() << endl;
}

bool solucion(vector<Ejecucion> tareasPendientes)
{
    return tareasPendientes.empty();
}

bool criterio(int nivel, vector<PlatUsage> s, vector<Ejecucion> tareasPendientes)
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

void get_solution(Task *tasks, int n_tasks, Platform *platform, Task *sorted_tasks, Platform *selected_devs, double time, double energy)
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

    int sols = 0;
    // Algoritmo
    do
    {
        generar(nivel, s, hermanosRestantes, tareasPendientes, permutaciones, tact, eact, instEjecutadas);

        // cout << "Permutaciones: " << endl;
        // for (auto combi : permutaciones)
        // {
        //     cout << "\t";
        //     for (auto ej : combi)
        //     {
        //         cout << "T" << getId(ej.getTarea()) << (ej.isHT() ? "*" : "") << " ";
        //     }
        //     cout << endl;
        // }

        // cout << "Hermanos restantes: " << endl;
        // for (auto num : hermanosRestantes)
        // {
        //     cout << num << " ";
        // }
        // cout << endl;

        cout << "Nivel " << nivel << endl;
        s[nivel].printInfo();
        cout << "--------------------------------" << endl;

        if (solucion(tareasPendientes))
        {
            if (tact < toa && eact <= eoa)
            {
                toa = tact;
                eoa = eact;
                soa = s;
            }

            sols++;
            cout << "****************** SOLUCION (t = " << tact << ", e = " << eact << ") ******************" << endl;
        }

        if (criterio(nivel, s, tareasPendientes))
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

    cout << "Soluciones: " << sols << "\tTiempo óptimo: " << toa << "\tEnergía óptima: " << eoa << endl;
}
