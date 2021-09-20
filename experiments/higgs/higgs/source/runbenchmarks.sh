#!/bin/sh

set -e

run() {
    [ "$1" ]
    local dir
    dir="$1"

    mkdir -p runs
    mkdir runs/"$dir"

    /opt/rt/bin/rt 99 ./benchmark.py \
        --bench_list=contract-benchmark-list.csv \
        --csv_file=runs/"$dir"/out.csv \
        --num_runs=8 2>&1 | tee runs/"$dir"/log.txt
    chmod -R a-w runs/"$dir"
}

make release

run with-contracts

mv benchmarks benchmarks-correct
ln -s benchmarks-nocontracts benchmarks
run no-contracts

rm benchmarks

sed 's/contractsAlwaysCanFail = false/contractsAlwaysCanFail = true/' -i jit/ops.d
make release

ln -s benchmarks-lowruns benchmarks
run naive-contracts-lowruns

rm benchmarks
ln -s benchmarks-lowruns-nocontracts benchmarks
run no-contracts-lowruns

rm benchmarks
mv benchmarks-correct benchmarks
