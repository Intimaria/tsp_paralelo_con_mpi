#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "./includes/utils.h"
#include "./includes/estructuras_de_datos.h"
#include "./includes/branch_and_bound.h"

int main(int argc, char* argv[]) {
   FILE* archivo_de_matriz = NULL;
   double comienzo, fin, duracion_local, duracion;
   int ok_local = 1, tam_un_msg;
   char uso[256];
   char* buf;

   MPI_Init(&argc, &argv);
   universo = MPI_COMM_WORLD;
   MPI_Comm_size(universo, &num_de_procesos);
   MPI_Comm_rank(universo, &mi_id_proceso);

   if (mi_id_proceso == 0 && argc != 2) ok_local = 0;
   Chequeo_de_errores(ok_local, uso, universo);
   if (mi_id_proceso == 0) {
      archivo_de_matriz = fopen(argv[1], "r");
      if (archivo_de_matriz == NULL) ok_local = 0;
   }
   Chequeo_de_errores(ok_local, "No se puede abrir el archivo de la matriz", universo);
   Leer_matriz_de_adyacencias(archivo_de_matriz);
   if (mi_id_proceso == 0) fclose(archivo_de_matriz);


   mejor_camino_local = Alocar_camino(NULL);
   Inicializar_camino(mejor_camino_local, MIN);
   costo_del_mejor_camino = MIN;

   MPI_Type_contiguous(n+1, MPI_INT, &camino_mpi_t);
   MPI_Type_commit(&camino_mpi_t);

   MPI_Pack_size(1, MPI_INT, universo, &tam_un_msg);
   buffer_mpi = 
      malloc(100*num_de_procesos*(tam_un_msg + MPI_BSEND_OVERHEAD)*sizeof(char));
   MPI_Buffer_attach(buffer_mpi,
         100*num_de_procesos*(tam_un_msg + MPI_BSEND_OVERHEAD));

   MPI_Barrier(universo);
   comienzo = MPI_Wtime();
   Busqueda_branch_and_bound();
   fin = MPI_Wtime();

   duracion_local = fin - comienzo;
   MPI_Reduce(&duracion_local, &duracion, 1, MPI_DOUBLE, MPI_MAX, 0, universo);
   Limpiar_cola_mensages();
   MPI_Barrier(universo);
   MPI_Buffer_detach(&buf, &tam_un_msg);
   
   if (mi_id_proceso == 0) {
      Imprimir_camino(mejor_camino_local, "Mejor camino");
      printf("Costo = %d\n", mejor_camino_local->costo);
      printf("Tiempo = %f segundos\n", duracion);
   }

   MPI_Type_free(&camino_mpi_t);
   free(mejor_camino_local->ciudades);
   free(mejor_camino_local);
   free(matriz_de_adyacencias);

   MPI_Finalize();
   return 0;
} 


 







 

 





















