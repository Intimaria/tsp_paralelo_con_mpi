#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <mpi.h>


#define MIN             1000000
#define VACIO           -1
#define FALSE           0
#define TRUE            1
#define TAG_CAMINO      1
#define COSTO_INICIAL   100

/* VARIABLES GLOBALES */
int n;  
int mi_id_proceso;
int num_de_procesos;
MPI_Comm universo;
char* buffer_mpi;


void Chequeo_de_errores(int ok_local, char mensaje[], MPI_Comm universo);
void Limpiar_cola_mensages(void);


#endif