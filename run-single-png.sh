#!/bin/bash
export RUNS=1
./run-nom.sh
./run-racket.sh
./run-monnom.sh
./run-grift.sh
./make_charts_png.sh

