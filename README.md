# Paralelismo de solución a TSP con Branch and Bound en sistemas distribuidos con MPI

Clonar el repositorio localmente y entrar en el mismo: 
``` 
$ git clone git@github.com:Intimaria/tsp_paralelo_con_mpi.git
$ cd tsp_paralelo_con_mpi
```
Correr el makefile 
```
$ make
```
## Script sbatch para slurm scheduler

Si esta utilizando slurm scheduler, puede usar el script.sh provisto para poder administrar jobs. 
Darle permisos al script.sh
```
$ chmod +x script.sh
```
Correr el script con bash de la siguiente manera:
```
$ bash script.sh [numero de nodos] [numero de procesos por nodo] main [archivo de entrada con matriz de adyacencias]
```
Se pueden encontrar una serie de entradas en la carpeta `./data`. Allí encontrara tres casos de grafos, cada uno con Ns de 5, 8, 10, 12 y 15.
Para casos de un mejor camino de principio a fin entre en la carpeta `uno`, para el peor caso (sin mejor camino), entre en la carpeta `sin`, y para casos intermedios con camino mejor parcial, entre en la carpeta `parcial`.

## Correr normalmente

Si desea correr las pruebas directamente, sin scehduler, corra el siguinte comando luego de hacer `make`. N es el numero de procesos
```
$ mpirun -np N main [archivo de entrada]
```


