#!/bin/bash

FILENAME="MachineCode"
TESTFILE=$1
OUTNAME="out.asm"
  
if test -f ./bin/$FILENAME; then
    ./bin/$FILENAME < ./inputs/$TESTFILE
    spim -file ./outputs/$OUTNAME
    printf "\n"
else
  echo "File not founded"
fi
