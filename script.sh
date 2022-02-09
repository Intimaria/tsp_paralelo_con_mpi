#!/bin/bash
sbatch <<EOT
#!/bin/sh
#SBATCH -N "$1"
#SBATCH --exclusive
#SBATCH --tasks-per-node="$2"
#SBATCH -o ./res/salida.txt
#SBATCH -e ./res/errores.txt
mpirun ./"$3" "$4"
EOT
