#include "../includes/branch_and_bound.h"


ciudad_t ciudad_de_partida = 0;

void Busqueda_branch_and_bound(void) {
   ciudad_t nbr;
   pila_propia_t pila;  
   pila_propia_t disponible;  
   camino_t camino_actual;

   disponible = Inicializar_pila();
   pila = Inicializar_pila();
   Particionar_el_arbol(pila);

   while (!Pila_esta_vacia(pila)) {
      camino_actual = Sacar_ultimo(pila);
      if (Cant_ciudades(camino_actual) == n) {
         if (Mejor_camino(camino_actual)) {
            Actualizar_el_mejor_camino(camino_actual);
         }
      } else {
         for (nbr = n-1; nbr >= 1; nbr--) 
            if (Viable(camino_actual, nbr)) {
               Agregar_ciudad_al_camino(camino_actual, nbr);
               Empujar_copia(pila, camino_actual, disponible);
               Quitar_la_ultima_ciudad(camino_actual);
            }
      }
      Liberar_camino(camino_actual, disponible);
   }
   Liberar_pila(pila);
   Liberar_pila(disponible);
   MPI_Barrier(universo);
   Obtener_mejor_camino_global();
} 



void Particionar_el_arbol(pila_propia_t pila) {
   int mi_contador, ok_local = 1;
   int tam_de_cola, contador_de_caminos;
   ciudad_t *lista_de_colas = NULL;
   ciudad_t *lista_de_caminos;
   int cuantos[num_de_procesos];  
   int desplazamientos[num_de_procesos]; 

   if (mi_id_proceso == 0) {
      tam_de_cola = Limite_superior_para_cola();
      if (tam_de_cola == 0) ok_local = 0;
   }
   Chequeo_de_errores(ok_local, "Demasiados procesos", universo);

   if (mi_id_proceso == 0) 
      Construir_cola_inicial(&lista_de_colas, tam_de_cola, &contador_de_caminos);
   MPI_Bcast(&contador_de_caminos, 1, MPI_INT, 0, universo);

   Colocar_caminos_iniciales(contador_de_caminos, cuantos, desplazamientos, 
         &mi_contador, &lista_de_caminos);

   MPI_Scatterv(lista_de_colas, cuantos, desplazamientos, camino_mpi_t,
         lista_de_caminos, mi_contador, camino_mpi_t, 0, universo);
   
   Construir_pila_inicial(pila, lista_de_caminos, mi_contador);

   if (mi_id_proceso == 0) free(lista_de_colas);
   free(lista_de_caminos);

}  

int Mejor_camino(camino_t camino) {
   costo_t costo_actual = Costo_camino(camino);
   ciudad_t ultima_ciudad = Ultima_ciudad(camino);

   Buscar_mejores_caminos();

   if (costo_actual + Costo(ultima_ciudad, ciudad_de_partida) < costo_del_mejor_camino)
      return TRUE;
   else
      return FALSE;
}  

void Actualizar_el_mejor_camino(camino_t camino) {
   Copiar_camino(camino, mejor_camino_local);
   Agregar_ciudad_al_camino(mejor_camino_local, ciudad_de_partida);
   costo_del_mejor_camino = Costo_camino(mejor_camino_local);
   Bcast_costo_camino(costo_del_mejor_camino);
}  

void Copiar_camino(camino_t camino1, camino_t camino2) {
   memcpy(camino2->ciudades, camino1->ciudades, (n+1)*sizeof(ciudad_t));

   camino2->contador = camino1->contador;
   camino2->costo = camino1->costo;
}  

void Agregar_ciudad_al_camino(camino_t camino, ciudad_t nueva_ciudad) {
   ciudad_t ultima_ciudad_anterior = Ultima_ciudad(camino);
   camino->ciudades[camino->contador] = nueva_ciudad;
   (camino->contador)++;
   camino->costo += Costo(ultima_ciudad_anterior,nueva_ciudad);
}  


void Quitar_la_ultima_ciudad(camino_t camino) {
   ciudad_t ultima_ciudad_anterior = Ultima_ciudad(camino);
   ciudad_t nueva_ciudad_anterior;
   
   camino->ciudades[camino->contador-1] = VACIO;
   (camino->contador)--;
   nueva_ciudad_anterior = Ultima_ciudad(camino);
   camino->costo -= Costo(nueva_ciudad_anterior,ultima_ciudad_anterior);
}  

int Viable(camino_t camino, ciudad_t ciudad) {
   ciudad_t ultima_ciudad = Ultima_ciudad(camino);

   if (!Visitada(camino, ciudad) && 
        Costo_camino(camino) + Costo(ultima_ciudad,ciudad) < costo_del_mejor_camino)
      return TRUE;
   else
      return FALSE;
}  

int Visitada(camino_t camino, ciudad_t ciudad) {
   int i;

   for (i = 0; i < Cant_ciudades(camino); i++)
      if ( Ciudad_en_camino(camino,i) == ciudad ) return TRUE;
   return FALSE;
}  


void Inicializar_camino(camino_t camino, costo_t costo) {
   int i;

   camino->ciudades[0] = 0;
   for (i = 1; i <= n; i++) {
      camino->ciudades[i] = VACIO;
   }
   camino->costo = costo;
   camino->contador = 1;
} 

camino_t Alocar_camino(pila_propia_t disponible) {
   camino_t tmp;

   if (disponible == NULL || Pila_esta_vacia(disponible)) {
      tmp = malloc(sizeof(struct_camino));
      tmp->ciudades = malloc((n+1)*sizeof(ciudad_t));
      return tmp;
   } else {
      return Sacar_ultimo(disponible);
   }
}  

void Liberar_camino(camino_t camino, pila_propia_t disponible) {
   if (disponible == NULL) {
      free(camino->ciudades);
      free(camino);
   } else {
      Empujar(disponible, camino);
   }
}  

void Imprimir_camino(camino_t camino, char* titulo) {
   int i;
   char string[10000];

   if (mi_id_proceso >= 0)
      sprintf(string, "Proc %d > %s %p: ", mi_id_proceso, titulo, camino);
   else
      sprintf(string, "%s: ", titulo);
   for (i = 0; i < Cant_ciudades(camino); i++)
      sprintf(string + strlen(string), "%d ", Ciudad_en_camino(camino,i));
   printf("%s\n\n", string);
}

void Buscar_mejores_caminos(void) {
   int terminado = FALSE, msg_disponible, costo_camino;
   MPI_Status status;

   while(!terminado) {
      MPI_Iprobe(MPI_ANY_SOURCE, TAG_CAMINO, universo, &msg_disponible, 
            &status);
      if (msg_disponible) {
         MPI_Recv(&costo_camino, 1, MPI_INT, status.MPI_SOURCE, TAG_CAMINO,
               universo, MPI_STATUS_IGNORE);
         if (costo_camino < costo_del_mejor_camino) costo_del_mejor_camino = costo_camino;
      } else {
         terminado = TRUE;
      }
   }  
}  


void Bcast_costo_camino(int costo_camino) {
   int destino;

   for (destino = 0; destino < num_de_procesos; destino++)
      if (destino != mi_id_proceso)
         MPI_Bsend(&costo_camino, 1, MPI_INT, destino, TAG_CAMINO, universo);
}  



void Obtener_mejor_camino_global(void) {
   struct {
      int costo;
      int id;
   } data_local, data_global;
   data_local.costo = Costo_camino(mejor_camino_local);
   data_local.id = mi_id_proceso;

   /* Proceso 0 y el dueño del mejor camino necesitan data_global */
   MPI_Allreduce(&data_local, &data_global, 1, MPI_2INT, MPI_MINLOC, universo);

   if (data_global.id == 0) return;
   if (mi_id_proceso == 0) {
      MPI_Recv(mejor_camino_local->ciudades, n+1, MPI_INT, data_global.id,
            0, universo, MPI_STATUS_IGNORE);
      mejor_camino_local->costo = data_global.costo;
      mejor_camino_local->contador = n+1;
   } else if (mi_id_proceso == data_global.id) {
      MPI_Send(mejor_camino_local->ciudades, n+1, MPI_INT, 0, 0, universo);
   } 
}  



void Crear_camino_desde_lista(ciudad_t lista[], camino_t camino) {
   int cont = 1, costo = 0;
   ciudad_t city1, city2;

   memcpy(camino->ciudades, lista, (n+1)*sizeof(ciudad_t));

   city1 = 0;
   while (cont <= n && lista[cont] != VACIO) {
      city2 = lista[cont];
      cont++;
      costo += Costo(city1, city2);
      city1 = city2;
   }
   camino->contador = cont;
   camino->costo = costo;
} 

void Colocar_caminos_iniciales(int contador_de_caminos, int cuantos[], int desplazamientos[],
      int* contador_de_proceso, ciudad_t** lista_de_caminos_de_proceso) {
   int quotient, remainder, i;

   quotient = contador_de_caminos/num_de_procesos;
   remainder = contador_de_caminos % num_de_procesos;
   for (i = 0; i < remainder; i++) 
      cuantos[i] = quotient+1;
   for (i = remainder; i  < num_de_procesos; i++)
      cuantos[i] = quotient;
   *contador_de_proceso = cuantos[mi_id_proceso];
   desplazamientos[0] = 0;
   for (i = 1; i < num_de_procesos; i++)
      desplazamientos[i] = desplazamientos[i-1] + cuantos[i-1];

   *lista_de_caminos_de_proceso = malloc((*contador_de_proceso)*(n+1)*sizeof(int));
}  


void Leer_matriz_de_adyacencias(FILE* archivo_de_matriz) {
   int i, j, ok_local = 1;

   if (mi_id_proceso == 0) fscanf(archivo_de_matriz, "%d", &n);
   MPI_Bcast(&n, 1, MPI_INT, 0, universo);
   if (n <= 0) ok_local = 0;
   Chequeo_de_errores(ok_local, "Numero de vertices debe ser positivo", universo);

   matriz_de_adyacencias = malloc(n*n*sizeof(costo_t));

   if (mi_id_proceso == 0) {
      for (i = 0; i < n; i++)
         for (j = 0; j < n; j++) {
            fscanf(archivo_de_matriz, "%d", &matriz_de_adyacencias[i*n + j]);
            if (i == j && matriz_de_adyacencias[i*n + j] != 0) {
               fprintf(stderr, "Entradas diagonales deben ser cero\n");
               ok_local = 0;;
            } else if (i != j && matriz_de_adyacencias[i*n + j] <= 0) {
               fprintf(stderr, "Entrada no diagonal debe ser positiva\n");
               ok_local = 0;
            }
         }
   }
   Chequeo_de_errores(ok_local, "Error en el archivo de matriz", universo);
   MPI_Bcast(matriz_de_adyacencias, n*n, MPI_INT, 0, universo);
}  
