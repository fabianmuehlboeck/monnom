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
RACKETPKGPATH=`racket -e "(require setup/dirs) (display (path->string (find-user-console-bin-dir)))"`
GRIFTPATHADD="$RACKETPKGPATH"
if [[ "$PATH" =~ (^|:)"$GRIFTPATHADD"(|/)(:|$) ]]; then
  :
else
  export PATH="$GRIFTPATHADD:$PATH"
fi
