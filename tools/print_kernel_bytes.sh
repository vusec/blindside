#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

if [ "$3" == "" ]
then
  $DIR/lib/print_kernel_bytes $*
elif [ "$3" == "beautify" ]
then
  $DIR/lib/print_kernel_bytes $1 $2 | $DIR/lib/beautify.py
else
  $DIR/lib/print_kernel_bytes
fi
