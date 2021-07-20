#!/bin/bash
SOURCE="$( realpath "${BASH_SOURCE[0]}" )"
DIRNAME="$( dirname "$SOURCE" )"
. "$DIRNAME/setpath.sh"
if [ -z ${RUNS+x} ]; then RUNS=1 ; else :; fi
"$MONNOMBASE/nom/Benchmark.exe" -l racket -r "$MONNOMBASE/experiments/racket/sieve" -n $RUNS -w

