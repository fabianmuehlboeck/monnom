#!/bin/bash
SOURCE="$( realpath "${BASH_SOURCE[0]}" )"
DIRNAME="$( dirname "$SOURCE" )"
. "$DIRNAME/setpath.sh"
if [ -z ${RUNS+x} ]; then RUNS=1 ; else :; fi
ORIG=`pwd`
cd "$MONNOMBASE"
python3 run-csharp-float.py "$MONNOMBASE"
python3 run-java-float.py "$MONNOMBASE"
python3 run-node-float.py "$MONNOMBASE"
python3 run-retic-float.py "$MONNOMBASE"
python3 run-grift-float.py "$MONNOMBASE"
nombench --path "$MONNOMBASE/experiments/monnom/float" -w 2 --project Float -r $RUNS
$DIRNAME/collect.sh "monnom/float"
cd $ORIG

