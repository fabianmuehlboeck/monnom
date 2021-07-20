#!/bin/bash
SOURCE="$( realpath "${BASH_SOURCE[0]}" )"
DIRNAME="$( dirname "$SOURCE" )"
. "$DIRNAME/setpath.sh"
rm -rf "$MONNOMBASE/experiments/monnom/sieve/.BM_"*
rm -f "$MONNOMBASE/experiments/monnom/sieve/results.csv"
rm -rf "$MONNOMBASE/experiments/monnom/sort/.BM_"*
rm -f "$MONNOMBASE/experiments/monnom/sort/results.csv"
rm -rf "$MONNOMBASE/experiments/monnom/float/.BM_"*
rm -f "$MONNOMBASE/experiments/monnom/float/results.csv"
rm -rf "$MONNOMBASE/experiments/grift/sieve/monotonic/.BM_"*
rm -f "$MONNOMBASE/experiments/grift/sieve/monotonic/results.csv"
rm -rf "$MONNOMBASE/experiments/grift/sieve/proxies/.BM_"*
rm -f "$MONNOMBASE/experiments/grift/sieve/proxies/results.csv"
rm -rf "$MONNOMBASE/experiments/racket/sieve/benchmark"
rm -rf "$MONNOMBASE/experiments/nom/sieve/benchmark"
