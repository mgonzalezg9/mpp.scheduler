#include <stdio.h>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <deque>
#include <algorithm>
#include "mpi.h"

#include "../include/platform.hpp"
#include "../include/task.hpp"
#include "../include/ejecucion.hpp"
#include "../include/dev_usage.hpp"
#include "../include/plat_usage.hpp"

#define NIVEL_INEXPLORADO 0
#define TAG 99

using namespace std;

// Funciones auxiliares

// Crea un tipo de datos Device para su envío con MPI
void createDeviceType(MPI_Datatype *device_type)
{
	Device indata;
	int numAttrs = 5;
	int blocklen1[5] = {1, 1, 1, 1, 1};

	MPI_Datatype typelist[5] = {MPI_INT, MPI_INT, MPI_INT, MPI_DOUBLE, MPI_INT};
	MPI_Aint addresses[numAttrs];
	MPI_Aint displacements[numAttrs];

	MPI_Get_address(&indata, &addresses[0]);
	MPI_Get_address(&(indata.inst_core), &addresses[1]);
	MPI_Get_address(&(indata.n_cores), &addresses[2]);
	MPI_Get_address(&(indata.consumption), &addresses[3]);
	MPI_Get_address(&(indata.hyperthreading), &addresses[4]);

	for (int i = 0; i < numAttrs; i++)
	{
		displacements[i] = addresses[i] - addresses[0];
	}

	MPI_Type_create_struct(numAttrs, blocklen1, displacements, typelist, device_type);
	MPI_Type_commit(device_type);
}

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
    //     cout << "T" << getId(ej.getTarea()) << (ej.isHT() ? "+" : "-") << " ";
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
        permutaciones.insert(permutaciones.begin(), nuevas.begin(), nuevas.end());
        hermanosRestantes[nivel] = nuevas.size();
    }

    // Coge la siguiente secuencia en la cola
    vector<Ejecucion> secuencia = permutaciones.front();
    permutaciones.pop_front();

    // for (auto ej : secuencia)
    // {
    //     cout << "T" << getId(ej.getTarea()) << (ej.isHT() ? "+" : "-") << " ";
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
void get_solution(Task *tasks, int n_tasks, Platform *platform, Task *sorted_tasks, Platform *selected_devs, double &time, double &energy, int rank, int size)
{
    // Inicialización
    int nivel = 1;

    // Tipo de datos Device
    MPI_Datatype device_type;
	createDeviceType(&device_type);

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
    vector<int> instEjecutadas(n_tasks, 0);
    vector<Ejecucion> tareasPendientes = Ejecucion::crearTareas(tasks, n_tasks);

    // Reparto del primer nivel entre los procesos
    int trozo, resto;
    int numProcesos = size;
    int tamDivision = sizeof(int) * 2;
    char *workshare = (char *)malloc(tamDivision);

    if (rank == 0)
    {
        // Genera el primer nivel para cada uno de los procesos
        vector<vector<Ejecucion>> hermanosPrimerNivel = getPermutaciones(tareasPendientes, inicio);
        int numHermanos = hermanosPrimerNivel.size();

        // Avisa a cada proceso de los hermanos que le van a llegar
        trozo = numHermanos / size;
        resto = numHermanos % size;

        int position = 0;
        MPI_Pack(&trozo, 1, MPI_INT, workshare, tamDivision, &position, MPI_COMM_WORLD);
        MPI_Pack(&resto, 1, MPI_INT, workshare, tamDivision, &position, MPI_COMM_WORLD);
        MPI_Bcast(workshare, position, MPI_PACKED, 0, MPI_COMM_WORLD);

        // Si hay más procesos que nodos trabajaran solo aquellos procesos con nodo
        if (trozo == 0)
        {
            numProcesos = resto;
        }

        // Master se queda con su parte
        permutaciones.insert(permutaciones.end(), hermanosPrimerNivel.begin(), hermanosPrimerNivel.begin() + trozo);

        // Manda a cada proceso las permutaciones que le corresponden
        for (int p = 1; p < size; p++)
        {
            for (int i = 0; i < trozo; i++)
            {
                vector<int> idTareas = Ejecucion::getListIds(hermanosPrimerNivel[p * trozo + i]);

                int numIds = idTareas.size();
                MPI_Send(&numIds, 1, MPI_INT, p, TAG, MPI_COMM_WORLD);

                MPI_Send(&idTareas[0], numIds, MPI_INT, p, TAG, MPI_COMM_WORLD);
            }
        }

        // Distribuye el resto entre los procesos
        if (resto > 0)
        {
            // Para el master
            permutaciones.push_back(hermanosPrimerNivel[(size - 1) * trozo]);

            // Al resto de procesos
            int p = 1;
            for (int i = 1; i < resto; i++)
            {
                vector<int> idTareas = Ejecucion::getListIds(hermanosPrimerNivel[(size - 1) * trozo + i]);

                int numIds = idTareas.size();
                MPI_Send(&numIds, 1, MPI_INT, p, TAG, MPI_COMM_WORLD);

                MPI_Send(&idTareas[0], numIds, MPI_INT, p, TAG, MPI_COMM_WORLD);
                p++;
            }

            // El trozo de master ahora es mayor
            trozo++;
        }
    }
    else
    {
        MPI_Bcast(workshare, tamDivision, MPI_PACKED, 0, MPI_COMM_WORLD);

        int position = 0;
        MPI_Unpack(workshare, tamDivision, &position, &trozo, 1, MPI_INT, MPI_COMM_WORLD);
        MPI_Unpack(workshare, tamDivision, &position, &resto, 1, MPI_INT, MPI_COMM_WORLD);

        // Calcula si le toca recibir resto
        if (resto > rank)
        {
            trozo++;
        }

        for (int i = 0; i < trozo; i++)
        {
            int numIds;
            MPI_Recv(&numIds, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            vector<int> idsPermutacion(numIds);
            MPI_Recv(&idsPermutacion[0], numIds, MPI_INT, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // for (int id : idsPermutacion)
            // {
            //     cout << "T" << id << " ";
            // }
            // cout << endl;
            permutaciones.push_back(Ejecucion::crearTareas(idsPermutacion, tasks));
        }
    }

    // Actualiza las variables auxiliares
    hermanosRestantes[nivel] = trozo;

    if (hermanosRestantes[nivel] == 0)
    {
        // No se le ha asignado carga al proceso, por lo que termina
        return;
    }

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

    // cout << "P" << rank << ": (" << toa << ", " << eoa << ")" << endl;

    // Master recoge la mejor solución
    int tamPaquete = 2 * sizeof(double);
    char *paquete = (char *)malloc(tamPaquete);

    if (rank == 0)
    {
        int procesoSolucion = 0;
        time = toa;
        energy = eoa;

        for (int i = 1; i < numProcesos; i++)
        {
            double t, e;
            MPI_Status status;

            int position = 0;
            MPI_Recv(paquete, tamPaquete, MPI_PACKED, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &status);

            MPI_Unpack(paquete, tamPaquete, &position, &t, 1, MPI_DOUBLE, MPI_COMM_WORLD);
            MPI_Unpack(paquete, tamPaquete, &position, &e, 1, MPI_DOUBLE, MPI_COMM_WORLD);

            // Comprueba si es mejor que la solución de master
            if (t <= time && e <= energy)
            {
                procesoSolucion = status.MPI_SOURCE;
                time = t;
                energy = e;
            }
        }

        // Se indica a todos los procesos que pueden finalizar menos al que tiene la solución
        for (int i = 1; i < numProcesos; i++)
        {
            int solucion = 0;
            if (i == procesoSolucion)
            {
                solucion = 1;
            }

            MPI_Send(&solucion, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
        }

        if (procesoSolucion != 0)
        {
            // Se reciben los id de las tareas en el orden de ejecución
            vector<int> idTareas(n_tasks);
            MPI_Recv(&idTareas[0], n_tasks, MPI_INT, procesoSolucion, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int i = 0;
            for (auto id : idTareas)
            {
                sorted_tasks[i].id = id;
                i++;
            }

            // Se reciben los array con los dispositivos
            for (int i = 0; i < n_tasks; i++)
            {
                MPI_Recv(&selected_devs[i].n_devices, 1, MPI_INT, procesoSolucion, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(selected_devs[i].devices, selected_devs[i].n_devices, device_type, procesoSolucion, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
        else
        {
            formatSolucion(soa, sorted_tasks, n_tasks, selected_devs, platform->n_devices);
        }
    }
    else
    {
        // Empaquetado de los datos
        int position = 0;
        MPI_Pack(&toa, 1, MPI_DOUBLE, paquete, tamPaquete, &position, MPI_COMM_WORLD);
        MPI_Pack(&eoa, 1, MPI_DOUBLE, paquete, tamPaquete, &position, MPI_COMM_WORLD);
        MPI_Send(paquete, position, MPI_PACKED, 0, TAG, MPI_COMM_WORLD);

        // Comprueba si tiene la solución o no
        int solucion;
        MPI_Recv(&solucion, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if (solucion)
        {
            formatSolucion(soa, sorted_tasks, n_tasks, selected_devs, platform->n_devices);

            vector<int> idTareas;
            for (int i = 0; i < n_tasks; i++)
            {
                idTareas.push_back(sorted_tasks[i].id);
            }

            MPI_Send(&idTareas[0], n_tasks, MPI_INT, 0, TAG, MPI_COMM_WORLD);

            for (int i = 0; i < n_tasks; i++)
            {
                MPI_Send(&selected_devs[i].n_devices, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);
                MPI_Send(selected_devs[i].devices, selected_devs[i].n_devices, device_type, 0, TAG, MPI_COMM_WORLD);
            }
        }
    }
}
