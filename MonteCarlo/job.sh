#!/bin/bash

#PBS -l walltime=00:10:00,nodes=4:ppn=4
#PBS -N Termal_MichalM
#PBS -q batch

cd $PBS_O_WORKDIR

out_file=./MK.o
D = 9
for i in {1..16}
do
        $out_file $i $D
done

