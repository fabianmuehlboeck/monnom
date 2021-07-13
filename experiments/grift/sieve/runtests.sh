#!/bin/bash

mkdir combined
for maint in "U" "T"
do
for streamt in "U" "T"
do
for cft in "U" "T"
do
for siftt in "U" "T"
do
for sievet in "U" "T"
do
config="$streamt$cft$siftt$sievet$maint"
mkdir "combined/.BM_$config"
cat "$streamt/streams.grift" "$maint/count-from$cft.grift" "$maint/sift$siftt.grift" "$maint/sieve$sievet.grift" "$maint/main.grift" > "combined/.BM_$config/sieve.grift"
cd "combined/.BM_$config"
grift --reference-semantics Monotonic "sieve.grift"
for index in 1 
#2 3 4 5 6 7 8 9 10
do
echo "$config: Run $index"
echo 9999 | ./a.out > "out$index.txt"
done
cd ../..
done
done
done
done
done
