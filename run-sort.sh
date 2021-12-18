#!/bin/bash
SOURCE="$( realpath "${BASH_SOURCE[0]}" )"
DIRNAME="$( dirname "$SOURCE" )"
. "$DIRNAME/setpath.sh"
if [ -z ${RUNS+x} ]; then RUNS=1 ; else :; fi
ORIG=`pwd`
cd "$MONNOMBASE"
python3 run-csharp-sort.py "$MONNOMBASE"
python3 run-java-sort.py "$MONNOMBASE"
python3 run-node-sort.py "$MONNOMBASE"
python3 run-higgs-sort.py "$MONNOMBASE"
python3 run-retic-sort.py "$MONNOMBASE"
nombench --path "$MONNOMBASE/experiments/monnom/sort" -w 2 --project Sort --byfile -r $RUNS
$MONNOMBASE/collect.sh "monnom/sort"
cd $ORIG

