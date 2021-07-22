#!/bin/bash
SOURCE="$( realpath "${BASH_SOURCE[0]}" )"
DIRNAME="$( dirname "$SOURCE" )"

sudo apt-get install -y mono-runtime

raco pkg install grift

sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang++-12 60 #alternatively, ensure some modern version of clang is installed

cd "$DIRNAME/experiments/racket"
raco link benchmark-util

sudo apt-get install -y python3 python3-pip
pip3 install -U plotly
pip3 install -U pandas
pip3 install -U kaleido

$DIRNAME/build.sh

