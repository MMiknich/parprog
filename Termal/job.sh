#!/bin/bash

#PBS -l walltime=00:10:00,nodes=4:ppn=4
#PBS -N Termal_MichalM
#PBS -q batch

cd $PBS_O_WORKDIR

out_file=./temptask2
k=1
U0=1
Ue=0
dt=0.0002
OUT_TYPE=0
T=0.0001

for j in 0.0005 0.0001 
do
	for i in {1..16}
	do
		
		mpirun --hostfile $PBS_NODEFILE -np $i $out_file $U0 $Ue $k $j $dt $T $OUT_TYPE
	done
done
T=0.00001
for j in 0.00002
do
	for i in {1..16}
	do
                mpirun --hostfile $PBS_NODEFILE -np $i $out_file $U0 $Ue $k $j $dt $T $OUT_TYPE
        done
done
