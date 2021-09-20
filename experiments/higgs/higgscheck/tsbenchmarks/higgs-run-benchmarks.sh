#!/bin/sh
for b in compiled/*/*
do
    (
        cd "$b"
        ln -s ../../../runtime .
        ln -s ../../../stdlib .
        ln -s ../../../benchmark.js .
    ) 2> /dev/null
done

for i in `seq 0 10`
do
    for b in compiled/*/*
    do
        printf '"%s",' "$b"
        (
            cd "$b"
            higgs benchmark.js
        )
    done
done
