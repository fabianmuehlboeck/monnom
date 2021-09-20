#!/bin/bash
if [ -z ${RUNS+x} ]; then RUNS=1 ; else :; fi
mkdir -p proxies
mkdir -p monotonic
for cft in "U" "T"
do
for maint in "U" "T"
do
for sievet in "U" "T"
do
for siftt in "U" "T"
do
for streamt in "U" "T"
do
config="$cft$maint$sievet$siftt$streamt"
mkdir -p "proxies/.BM_$config"
mkdir -p "monotonic/.BM_$config"
cat "$streamt/streams.grift" "$maint/count-from$cft.grift" "$maint/sift$siftt.grift" "$maint/sieve$sievet.grift" "$maint/main.grift" > "proxies/.BM_$config/sieve.grift"
cp "proxies/.BM_$config/sieve.grift" "monotonic/.BM_$config/sieve.grift"
cd "proxies/.BM_$config"
grift --reference-semantics Proxied --coercions "sieve.grift"
for index in $(seq 1 $RUNS)
do
echo "$config: Proxies Run $index"
echo 9999 | ./a.out > "out$index.txt"
done
cd ../..
cd "monotonic/.BM_$config"
grift --reference-semantics Monotonic --coercions "sieve.grift"
for index in $(seq 1 $RUNS)
do
echo "$config: Monotonic Run $index"
echo 9999 | ./a.out > "out$index.txt"
done
cd ../..
done
done
done
done
done
