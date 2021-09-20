#!/bin/bash
SOURCE="$( realpath "${BASH_SOURCE[0]}" )"
DIRNAME="$( dirname "$SOURCE" )"
. "$DIRNAME/setpath.sh"
CURDIR=`pwd`
cd "$MONNOMBASE"
python3 newplot.py Sieve experiments/nom/sieve nom experiments/monnom/sieve monnom experiments/grift/sieve grift experiments/racket/sieve racket experiments/csharp/sieve csharp experiments/java/sieve java experiments/node/sieve node
python3 newplot.py Sort experiments/monnom/sort monnom experiments/java/sort java experiments/csharp/sort csharp experiments/node/sort node experiments/higgs/sort higgs experiments/reticulated/sort retic
python3 newplot.py Float experiments/monnom/float monnom experiments/java/float java experiments/csharp/float csharp experiments/node/float node experiments/grift/float griftnew experiments/reticulated/float retic
cd "$CURDIR"
