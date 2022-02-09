#include "../includes/utils.h"





void Limpiar_cola_mensages(void) {
   int msg_recibido;
   MPI_Status status;
   int cuantos[2] = {0,0};
   char buf_trabajo[100000];

   MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, universo, &msg_recibido, &status);
   while (msg_recibido) {
      MPI_Recv(buf_trabajo, 100000, MPI_BYTE, status.MPI_SOURCE, 
            status.MPI_TAG, universo, MPI_STATUS_IGNORE);
      if (status.MPI_TAG == TAG_CAMINO)
         cuantos[1]++;
      else  
         cuantos[0]++;
      MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, universo, &msg_recibido, &status);
   }
}  

void Chequeo_de_errores(
      int       ok_local  , 
      char      mensaje[] , 
      MPI_Comm  universo  ) {
   int ok;

   MPI_Allreduce(&ok_local, &ok, 1, MPI_INT, MPI_MIN, universo);
   if (ok == 0) {
      int mi_id_proceso;
      MPI_Comm_rank(universo, &mi_id_proceso);
      if (mi_id_proceso == 0) {
         fprintf(stderr, "Proc %d > %s\n", mi_id_proceso, mensaje);
         fflush(stderr);
      }
      MPI_Finalize();
      exit(-1);
   }
} 


