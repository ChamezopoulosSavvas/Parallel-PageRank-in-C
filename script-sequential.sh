#!/bin/bash

#PBS -N sequential-p2p
#PBS -q auth
#PBS -j oe
#PBS -l mem=8gb

cd $PBS_O_WORKDIR

module load gcc

gcc sequential.c -O3 -lm -std=gnu99 -o sequential.out

./sequential.out p2p-Gnutella08.txt

./sequential.out p2p-Gnutella04.txt

./sequential.out p2p-Gnutella24.txt

./sequential.out p2p-Gnutella31.txt

rm sequential.out