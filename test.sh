#!/bin/bash

test=1
if [ $# -lt 1 ]
then
  if [ $test -eq 0 ]
  then
    echo "Vyzadovan parametr pocet_cisel"
    exit 1
  fi
else
    numbers=$1
fi;

mpic++ --prefix /usr/local/share/OpenMPI -o oets odd-even.cpp

if [ $test -eq 0 ]
then
 dd if=/dev/random bs=1 count=$numbers of=numbers
else

fi

#vyrobeni souboru s random cisly
dd if=/dev/random bs=1 count=$numbers of=numbers

#spusteni
mpirun --prefix /usr/local/share/OpenMPI -np $numbers oets

#uklid
rm -f oets numbers
