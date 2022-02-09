#ifndef DATOS_H
#define DATOS_H

#include "./utils.h"

typedef int ciudad_t;
typedef int costo_t;

typedef struct {
   ciudad_t* ciudades; 
   int contador;      
   costo_t costo;    
} struct_camino;
typedef struct_camino* camino_t;


typedef struct {
   camino_t* lista;
   int tam_de_lista;
   int lista_alloc;
}  struct_de_pila;
typedef struct_de_pila* pila_propia_t;


typedef struct {
   camino_t* lista;
   int lista_alloc;
   int cabeza;
   int final;
   int llena;
}  struct_de_cola;
typedef struct_de_cola* cola_t;

#include "./branch_and_bound.h"

#define Cola_elt(cola,i) \
   (cola->lista[(cola->cabeza + (i)) % cola->lista_alloc])


void Construir_pila_inicial(pila_propia_t pila, ciudad_t lista_de_caminos[], int mi_contador);
void Construir_cola_inicial(int** cola_proceso, int tam_de_cola,
      int *num_caminos_de_proceso);

pila_propia_t Inicializar_pila(void);
void Empujar(pila_propia_t pila, camino_t camino);  // Empujar pointer
void Empujar_copia(pila_propia_t pila, camino_t camino, pila_propia_t disponible); 
camino_t Sacar_ultimo(pila_propia_t pila);
int  Pila_esta_vacia(pila_propia_t pila);
void Liberar_pila(pila_propia_t pila);

/* colacircular  */
cola_t Inicializar_cola(int tamanio);
camino_t Sacar_primero(cola_t cola);
void Encolar(cola_t cola, camino_t camino);
int Cola_esta_vacia(cola_t cola);
void Liberar_cola(cola_t cola);
int Limite_superior_para_cola(void);
long long Fact(int k);


#endif