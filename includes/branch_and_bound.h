#ifndef BB_H
#define BB_H



#include "./estructuras_de_datos.h"
#include "./utils.h"


costo_t* matriz_de_adyacencias;
camino_t mejor_camino_local;
costo_t costo_del_mejor_camino;
MPI_Datatype camino_mpi_t;  


#define Cant_ciudades(camino) (camino->contador)
#define Costo_camino(camino) (camino->costo)
#define Ultima_ciudad(camino) (camino->ciudades[(camino->contador)-1])
#define Ciudad_en_camino(camino,i) (camino->ciudades[(i)])
#define Costo(ciudad1, ciudad2) (matriz_de_adyacencias[ciudad1*n + ciudad2])


void Leer_matriz_de_adyacencias(FILE* archivo_matriz);
void Busqueda_branch_and_bound(void);
void Particionar_el_arbol(pila_propia_t pila);


int  Mejor_camino(camino_t camino); 
void Actualizar_el_mejor_camino(camino_t camino);
void Copiar_camino(camino_t camino1, camino_t camino2);
void Agregar_ciudad_al_camino(camino_t camino, ciudad_t ciudad);
void Quitar_la_ultima_ciudad(camino_t camino);
int  Viable(camino_t camino, ciudad_t ciudad);
int  Visitada(camino_t camino, ciudad_t ciudad);
void Inicializar_camino(camino_t camino, costo_t costo);
camino_t Alocar_camino(pila_propia_t disponible);
void Liberar_camino(camino_t camino, pila_propia_t disponible);
void Imprimir_camino(camino_t camino, char* titulo);

void Buscar_mejores_caminos(void);
void Bcast_costo_camino(costo_t costo);

void Obtener_mejor_camino_global(void);
void Crear_camino_desde_lista(ciudad_t lista[], camino_t camino);
void Colocar_caminos_iniciales(int contador_de_caminos, int cuantos[], int desplazamientos[],
      int* contador_de_proceso, int** lista_de_caminos_de_proceso);


#endif