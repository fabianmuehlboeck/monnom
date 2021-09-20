#!/bin/sh
dorun() {
    local bm
    local prefix
    bm="$1"
    prefix="$2"

    node ../../built/local/tsc.js $bm-typed.ts
    (
        echo "$prefix"
        cat $bm-typed.js
        cat benchmark.js
    ) > $bm-typed.unchecked.js
    node ../../built/local/tsc.js --generateContracts $bm-typed.ts
    (
        echo "$prefix"
        cat $bm-typed.js
        cat benchmark.js
    ) > $bm-typed.safe.js

    printf '"%s","unchecked"' "$bm"
    for i in `seq 1 10`
    do
        printf ','
        higgs $bm-typed.unchecked.js | tr -d '\n'
    done
    printf '\n'

    printf '"%s","safe"' "$bm"
    for i in `seq 1 10`
    do
        printf ','
        higgs $bm-typed.safe.js | tr -d '\n'
    done
    printf '\n'
}

(
    dorun crypto 'var CryptoVERSION;'
    dorun navier-stokes ''
    dorun raytrace ''
    dorun richards 'var RichardsTYPEDVERSION;'
    dorun splay ''
) > benchmark-results.csv
