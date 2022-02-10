#include "../includes/estructuras_de_datos.h"

void Construir_pila_inicial(pila_propia_t pila, ciudad_t lista_de_caminos[], int mi_contador) {
   int i;
   camino_t camino = Alocar_camino(NULL);

   for (i = mi_contador-1; i >= 0; i--) {
      Crear_camino_desde_lista(lista_de_caminos + i*(n+1), camino);
      Empujar_copia(pila, camino, NULL);
   }
   Liberar_camino(camino, NULL);
}  


void Construir_cola_inicial(ciudad_t** cola_proceso, int tam_de_cola, 
      int* num_caminos_de_proceso) {
   cola_t cola;
   int tam_actual = 0, i;
   ciudad_t nbr;
   camino_t camino = Alocar_camino(NULL);
   ciudad_t* lista_de_colas;

   Inicializar_camino(camino, 0);
   cola = Inicializar_cola(2*tam_de_cola);

   /*Busqueda en anchura */
   Encolar(cola, camino);  // Encola copia

   Liberar_camino(camino, NULL);
   tam_actual++;
   while (tam_actual < num_de_procesos) {
      camino = Sacar_primero(cola);
      tam_actual--;
      for (nbr = 1; nbr < n; nbr++)
         if (!Visitada(camino, nbr)) {
            Agregar_ciudad_al_camino(camino, nbr);
            Encolar(cola, camino);
            tam_actual++;
            Quitar_la_ultima_ciudad(camino);
         }
      Liberar_camino(camino, NULL);
   } 

   *num_caminos_de_proceso = tam_actual; 


   /* Copia lista de ciudades de la cola a lista_de_colas */
   lista_de_colas = malloc((*num_caminos_de_proceso)*(n+1)*sizeof(int));
   for (i = 0; i < *num_caminos_de_proceso; i++)
      memcpy(lista_de_colas + i*(n+1), Cola_elt(cola,i)->ciudades, 
            (n+1)*sizeof(int));
   *cola_proceso = lista_de_colas;
   Liberar_cola(cola);
}  


pila_propia_t Inicializar_pila(void) {
   int i;

   pila_propia_t pila = malloc(sizeof(struct_de_pila));
   pila->lista = malloc(n*n*sizeof(camino_t));
   for (i = 0; i < n*n; i++)
      pila->lista[i] = NULL;
   pila->tam_de_lista = 0;
   pila->lista_alloc = n*n;

   return pila;
}  


// empuja puntero
void Empujar(pila_propia_t pila, camino_t camino) {
   if (pila->tam_de_lista == pila->lista_alloc) {
      fprintf(stderr, "Empujar causó derrame de la pila!\n");
      free(camino->ciudades);
      free(camino);
   } else {
      pila->lista[pila->tam_de_lista] = camino;
      (pila->tam_de_lista)++;
   }
}  

void Empujar_copia(pila_propia_t pila, camino_t camino, pila_propia_t disponible) {
   camino_t tmp;

   if (pila->tam_de_lista == pila->lista_alloc) {
      fprintf(stderr, "Stack overflow!\n");
      exit(-1);
   }
   tmp = Alocar_camino(disponible);
   Copiar_camino(camino, tmp);
   pila->lista[pila->tam_de_lista] = tmp;
   (pila->tam_de_lista)++;
}  


camino_t Sacar_ultimo(pila_propia_t pila) {
   camino_t tmp;

   if (pila->tam_de_lista == 0) {
      fprintf(stderr, "Trying to pop empty pila!\n");
      exit(-1);
   }
   tmp = pila->lista[pila->tam_de_lista-1];
   pila->lista[pila->tam_de_lista-1] = NULL;
   (pila->tam_de_lista)--;
   return tmp;
}  


int  Pila_esta_vacia(pila_propia_t pila) {
   if (pila->tam_de_lista == 0)
      return TRUE;
   else
      return FALSE;
}  

void Liberar_pila(pila_propia_t pila) {
   int i;

   for (i = 0; i < pila->tam_de_lista; i++) {
      free(pila->lista[i]->ciudades);
      free(pila->lista[i]);
   }
   free(pila->lista);
   free(pila);
}  


cola_t Inicializar_cola(int tamanio) {
   cola_t cola_nueva = malloc(sizeof(struct_de_cola));
   cola_nueva->lista = malloc(tamanio*sizeof(camino_t));
   cola_nueva->lista_alloc = tamanio;
   cola_nueva->cabeza = cola_nueva->final = cola_nueva->llena = 0;

   return cola_nueva;
}  

camino_t Sacar_primero(cola_t cola) {
   camino_t tmp;

   if (Cola_esta_vacia(cola)) {
      fprintf(stderr, "Intentaste desencolar de una cola vacia!\n");
      exit(-1);
   }
   tmp = cola->lista[cola->cabeza];
   cola->cabeza = (cola->cabeza + 1) % cola->lista_alloc;
   return tmp;
}  

void Encolar(cola_t cola, camino_t camino) {
   camino_t tmp;

   if (cola->llena == TRUE) {
      fprintf(stderr, "Intentaste encolar de una cola llena!\n");
      fprintf(stderr, "lista_alloc = %d, cabeza = %d, final = %d\n",
            cola->lista_alloc, cola->cabeza, cola->final);
      exit(-1);
   }
   tmp = Alocar_camino(NULL);
   Copiar_camino(camino, tmp);
   cola->lista[cola->final] = tmp;
   cola->final = (cola->final + 1) % cola->lista_alloc; 
   if (cola->final == cola->cabeza)
      cola->llena = TRUE;

}  

int Cola_esta_vacia(cola_t cola) {
   if (cola->llena == TRUE)
      return FALSE;
   else if (cola->cabeza != cola->final)
      return FALSE;
   else
      return TRUE;
}  


void Liberar_cola(cola_t cola) {
   free(cola->lista);
   free(cola);
}  


int Limite_superior_para_cola(void) {
   int fact = n-1;
   int tamanio = n-1;

   while (tamanio < num_de_procesos) {
      fact++;
      tamanio *= fact;
   }

   if (tamanio > Fact(n-1)) {
      fprintf(stderr, "No necesitas tantos procesos para");
      fprintf(stderr, "un problema tan pequeño\n");
      tamanio = 0;
   }
   return tamanio;
}  


long long Fact(int k) {
   long long tmp = 1;
   int i;

   for (i = 2; i <= k; i++)
      tmp *= i;
   return tmp;
}  
