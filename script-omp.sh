#!/bin/bash

#PBS -N omp-4-1
#PBS -q pdlab
#PBS -j oe
#PBS -l nodes=1:ppn=4

cd $PBS_O_WORKDIR

module load gcc

gcc omp.c -O3 -fopenmp -lm -std=gnu99 -o omp.out

export OMP_NUM_THREADS=$PBS_NP

./omp.out data1.txt 4

#./omp.out data2.txt 4

./omp.out data3.txt 4

#./omp.out data4.txt 4

#rm sequential.out
