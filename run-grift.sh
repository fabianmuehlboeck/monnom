#!/bin/bash
SOURCE="$( realpath "${BASH_SOURCE[0]}" )"
DIRNAME="$( dirname "$SOURCE" )"
. "$DIRNAME/setpath.sh"
cd "$MONNOMBASE/experiments/grift/sieve"
./runtests.sh
cd "$MONNOMBASE"
$MONNOMBASE/collect.sh "grift/sieve/proxies"
$MONNOMBASE/collect.sh "grift/sieve/monotonic"
