#!/bin/bash
. /opt/intel/oneapi/setvars.sh #this is needed to be able to load libtbb.so
if [ -z ${RUNS+x} ]; then RUNS=1 ; else :; fi
SOURCE="$( realpath "${BASH_SOURCE[0]}" )"
DIRNAME="$( dirname "$SOURCE" )"
. "$DIRNAME/setpath.sh"
nombench --path "$MONNOMBASE/experiments/monnom/float" -w 2 --project Float -r $RUNS
$DIRNAME/collect.sh "monnom/float"
nombench --path "$MONNOMBASE/experiments/monnom/sort" -w 2 --project Sort --byfile -r $RUNS
$DIRNAME/collect.sh "monnom/sort"
nombench --path "$MONNOMBASE/experiments/monnom/sieve" -w 2 --project Sieve -r $RUNS
$DIRNAME/collect.sh "monnom/sieve"

