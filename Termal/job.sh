#!/bin/bash

#PBS -l walltime=00:10:00,nodes=4:ppn=4
#PBS -N Termal_MichalM
#PBS -q batch

#cd $PBS_O_WORKDIR

out_file=./temptask2
k=1
U_0=1
U_e=0
dt= 0.0002
OUT_TYPE=3
T=0.1

for j in 0.0005 #0.0001 0.00002
do
	for i in 1 2 3
	do
		mpirun -np $i $out_file $U_0 $U_e $k $j $dt $T $OUT_TYPE
	done
done
