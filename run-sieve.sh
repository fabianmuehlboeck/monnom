#!/bin/bash
SOURCE="$( realpath "${BASH_SOURCE[0]}" )"
DIRNAME="$( dirname "$SOURCE" )"
. "$DIRNAME/setpath.sh"
if [ -z ${RUNS+x} ]; then RUNS=1 ; else :; fi
ORIG=`pwd`
cd "$MONNOMBASE"
python3 run-csharp-sieve.py "$MONNOMBASE"
python3 run-java-sieve.py "$MONNOMBASE"
python3 run-node-sieve.py "$MONNOMBASE"
"$MONNOMBASE/nom/Benchmark.exe" -l racket -r "$MONNOMBASE/experiments/racket/sieve" -n $RUNS -w
cd "$MONNOMBASE/experiments/grift/sieve"
./runtests.sh
cd "$MONNOMBASE"
$MONNOMBASE/collect.sh "grift/sieve/proxies"
$MONNOMBASE/collect.sh "grift/sieve/monotonic"
cd "$MONNOMBASE/nom"
"./Benchmark.exe" -l ng-bash -r "$MONNOMBASE/experiments/nom/sieve" -n $RUNS -w
cd "$MONNOMBASE"
nombench --path "$MONNOMBASE/experiments/monnom/sieve" -w 2 --omituntypedinterfaces --project Sieve -r $RUNS
$MONNOMBASE/collect.sh "monnom/sieve"
cd "$ORIG"

