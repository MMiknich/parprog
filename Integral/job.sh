#!/bin/bash

#PBS -l walltime=00:10:00,nodes=4:ppn=4
#PBS -N Integral_MichalM
#PBS -q batch

cd $PBS_O_WORKDIR

N=100000000
out_file=./a.out
a=0.0
b=1.0

for j in 10 100 1000 10000 100000 1000000 10000000 100000000
do
	for i in {1..16}
	do
		echo $i
		mpirun --hostfile $PBS_NODEFILE -np $i $out_file $j $a $b 0
	done
done
