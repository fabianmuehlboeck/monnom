#!/bin/bash
SOURCE="$( realpath "${BASH_SOURCE[0]}" )"
DIRNAME="$( dirname "$SOURCE" )"
PATHADD="$DIRNAME/monnombuild:$DIRNAME/sourcecode/Runtime"
if [[ "$PATH" =~ (^|:)"$PATHADD"(|/)(:|$) ]]; then
  :
else
  export PATH="$PATHADD:$PATH"
fi
export MONNOMBASE="$DIRNAME"
. /opt/intel/oneapi/setvars.sh #this is needed to be able to load libtbb.so
RACKETPKGPATH=`racket -e "(require setup/dirs) (display (path->string (find-user-console-bin-dir)))"`
GRIFTPATHADD="$RACKETPKGPATH"
if [[ "$PATH" =~ (^|:)"$GRIFTPATHADD"(|/)(:|$) ]]; then
  :
else
  export PATH="$GRIFTPATHADD:$PATH"
fi
HIGGSPATHADD="$DIRNAME/experiments/higgs/higgs/source/:$DIRNAME/experiments/higgs/bin"
if [[ "$PATH" =~ (^|:)"$HIGGSPATHADD"(|/)(:|$) ]]; then
  :
else
  export PATH="$HIGGSPATHADD:$PATH"
fi
HIGGSLDLIBPATHADD="$DIRNAME/experiments/higgs/lib64"
if [[ "$LD_LIBRARY_PATH" =~ (^|:)"$HIGGSLDLIBPATHADD"(|/)(:|$) ]]; then
  :
else
  export LD_LIBRARY_PATH="$HIGGSLDLIBPATHADD:$LD_LIBRARY_PATH"
fi