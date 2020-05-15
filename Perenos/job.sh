#!/bin/bash
out_file=./perenos.o
echo 'error; time;' > out_file.csv
for i in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
do
	$out_file $i 4 >> out_file.csv
done
