# Paralelismo de solución a TSP con Branch and Bound en sistemas distribuidos con MPI

Resolución del **Problema del Viajante (TSP)** con **Branch & Bound** paralelizado en sistemas
distribuidos con **MPI**. Trabajo final de **Concurrencia y Paralelismo** (UNLP), 2022.

## Cómo paraleliza
- **Partición del árbol (no master-worker)**: el proceso 0 genera el árbol inicial por **BFS** y
  arma la lista de subproblemas; se reparten entre los procesos con `MPI_Scatterv`.
- Cada proceso corre su propio **DFS con branch & bound** sobre una pila, podando con la cota
  superior actual.
- **Cota compartida**: al mejorar el incumbent, un proceso hace `Bcast` del nuevo costo; los
  demás lo levantan y podan más.
- **Cierre**: `MPI_Allreduce` con `MPI_MINLOC` elige el mejor costo y su dueño, que envía el
  camino al proceso 0.

## Casos y resultados
Tres casos, N = 5…15 y distinto número de procesos. Speedup = tiempo secuencial / tiempo
paralelo (sublineal si S < P).

| Caso | Qué es | Speedup observado |
|---|---|---|
| **Sin camino** (peor caso) | hay que explorar todos los subárboles | hasta **~6** (N=13, 14 procesos) |
| **Un camino** | uno solo, hallado al inicio | **~1** — el secuencial ya es eficiente |
| **Camino parcial** | solución intermedia | mejor caso **~10** (N=15), eficiencia ~0.7 |

- El **camino parcial** es donde más rinde: con N=15 y más de 8 procesos se sostiene S > 5.
- El "sin camino" con N=15 es **inviable** por tiempo (horas) → se analizó con N=13.
- **Superlineal solo en el experimento de múltiples soluciones** (N=10, 5 caminos, P=6 → S ≈ 10.05),
  consistente con la teoría de Rao & Kumar para búsquedas con varias soluciones.

## Por qué no escala más
- **Ley de Amdahl**: hay una fracción secuencial inevitable (leer la matriz, calcular distancias,
  armar las listas); con muchos procesos, la comunicación y la distribución de carga pesan más
  que el beneficio (S < 1).
- **Partición estática** (BFS inicial), sin robo de trabajo dinámico → desbalance.
- La estructura irregular del TSP (caminos parciales) → carga impredecible.

## Cómo correr

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

## Correr normalmente

Si desea correr las pruebas directamente, sin scehduler, corra el siguinte comando luego de hacer `make`. N es el numero de procesos
```
$ mpirun -np N main [archivo de entrada]
```

## Informe completo
📄 [Paralelismo de solución a TSP (2022)](Tidball-Paralelismo-de-solucion-a-TSP-2022.pdf)

---

## English

Solving the **Travelling Salesman Problem (TSP)** with **Branch & Bound** parallelized across
**MPI** distributed systems. Final project for the **Concurrency and Parallelism** course (UNLP),
2022.

**Parallelization:** tree partitioning (not master-worker) — process 0 builds the initial tree by
**BFS** and splits the subproblems across processes with `MPI_Scatterv`. Each process runs its own
**DFS branch & bound** with a stack, pruning against the current upper bound. When a process
improves the incumbent it broadcasts the new cost, and everyone prunes harder. The final best path
is chosen with `MPI_Allreduce` / `MPI_MINLOC`.

**Results** (speedup = sequential / parallel time), across N = 5…15 and varying process counts:
- **No path** (worst case, full exploration): up to **~6×** (N=13, 14 processes).
- **Single path**: **~1×** — the sequential algorithm is already efficient.
- **Partial path**: best case **~10×** (N=15), efficiency ~0.7.

Speedup stays sublinear; the only superlinear case came from a multi-solution instance (N=10, 5
paths, P=6 → S ≈ 10.05), consistent with Rao & Kumar's analysis. Amdahl's law and the static
partition (no work stealing) explain why it doesn't scale further.

📄 [Full report (Spanish)](Tidball-Paralelismo-de-solucion-a-TSP-2022.pdf)
