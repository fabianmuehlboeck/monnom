#!/bin/bash
SOURCE="$( realpath "${BASH_SOURCE[0]}" )"
DIRNAME="$( dirname "$SOURCE" )"
. "$DIRNAME/setpath.sh"
CURDIR=`pwd`
cd "$MONNOMBASE"
python3 newplot.py Sieve.png experiments/nom/sieve nom experiments/monnom/sieve monnom experiments/grift/sieve grift experiments/racket/sieve racket
python3 newplot.py Sort.png experiments/monnom/sort monnom
python3 newplot.py Float.png experiments/monnom/float monnom
cd "$CURDIR"
