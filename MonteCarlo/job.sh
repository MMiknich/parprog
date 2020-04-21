#!/bin/bash
out_file=./MK.o
D=8
echo 'time; error;'
for i in 9 10 11 12 13 14 15 16
do
	$out_file $i $D
done
