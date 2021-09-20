#!/bin/sh
domkbad() {
    local bm
    local prefix
    local anns
    bm="$1"
    prefix="$2"
    anns="$3"
    mkdir -p mkbad-results/$bm

    for i in `seq -w 01 10`
    do
        node mkbad.js $bm "$prefix" $anns `nproc --all` | tee mkbad-results/$bm/$i.csv
    done
}

domkbad crypto 'var CryptoVERSION;' 635
domkbad navier-stokes '' 192
domkbad raytrace '' 232
domkbad richards 'var RichardsTYPEDVERSION;' 122
domkbad splay '' 46
