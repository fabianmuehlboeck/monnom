#!/bin/bash
if [ -z ${RUNS+x} ]; then RUNS=1 ; else :; fi
mkdir -p proxies
mkdir -p monotonic
for maint in "U" "T"
do
for pointt in "U" "T"
do
for pmft in "U" "T"
do
config="$maint$pointt$pmft"
mkdir -p "proxies/.BM_$config"
mkdir -p "monotonic/.BM_$config"
cat "$pointt/point.grift" "$pmft/pointmapfun.grift" "$maint/main.grift" > "proxies/.BM_$config/float.grift"
cp "proxies/.BM_$config/float.grift" "monotonic/.BM_$config/float.grift"
cd "proxies/.BM_$config"
grift --reference-semantics Proxied --coercions "float.grift"
for index in $(seq 1 $RUNS)
do
echo "$config: Proxies Run $index/$RUNS"
./a.out > "out$index.txt"
done
cd ../..
cd "monotonic/.BM_$config"
grift --reference-semantics Monotonic --coercions "float.grift"
for index in $(seq 1 $RUNS)
do
echo "$config: Monotonic Run $index/$RUNS"
./a.out > "out$index.txt"
done
cd ../..
done
done
done
