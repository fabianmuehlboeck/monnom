#!/bin/bash
SOURCE="$( realpath "${BASH_SOURCE[0]}" )"
DIRNAME="$( dirname "$SOURCE" )"
. "$DIRNAME/setpath.sh"
if [ -z ${RUNS+x} ]; then RUNS=1 ; else :; fi
ORIG=`pwd`
cd $MONNOMBASE/nom
"./Benchmark.exe" -l ng-bash -r "$MONNOMBASE/experiments/nom/sieve" -n $RUNS -w
cd "$ORIG"

