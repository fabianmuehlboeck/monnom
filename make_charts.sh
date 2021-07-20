#!/bin/bash
SOURCE="$( realpath "${BASH_SOURCE[0]}" )"
DIRNAME="$( dirname "$SOURCE" )"
. "$DIRNAME/setpath.sh"
CURDIR=`pwd`
cd "$MONNOMBASE"
python3 newplot.py Sieve experiments/nom/sieve nom experiments/monnom/sieve monnom experiments/grift/sieve grift experiments/racket/sieve racket
python3 newplot.py Sort experiments/monnom/sort monnom
python3 newplot.py Float experiments/monnom/float monnom
cd "$CURDIR"
