#!/bin/bash
SOURCE="$( realpath "${BASH_SOURCE[0]}" )"
DIRNAME="$( dirname "$SOURCE" )"
. "$DIRNAME/setpath.sh"
racket -e "(require \"collect.rkt\") (run \"$MONNOMBASE/experiments/$1\")"
