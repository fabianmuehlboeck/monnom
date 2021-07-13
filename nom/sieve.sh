#!/bin/bash

BMNAME="Sieve"

echo "Running Benchmark $BMNAME"

mkdir ./NG_sieve/fullbackups
mv ./NG_sieve/benchmark/*_full.csv ./NG_sieve/fullbackups/
./Benchmark.exe -l ng-bash -r NG_sieve -n 10 -w
cp -f ./NG_sieve/benchmark/*_full.csv ./ng_sieve.csv

mkdir ./racket_sieve/fullbackups
mv ./racket_sieve/benchmark/*_full.csv ./racket_sieve/fullbackups/
./Benchmark.exe -l racket -r racket_sieve -n 10 -w
cp -f ./racket_sieve/benchmark/*_full.csv ./racket_sieve.csv

python3 plot.py sieve.png ng_sieve.csv Nom racket_sieve.csv Racket

echo "Benchmark $BMNAME complete"
