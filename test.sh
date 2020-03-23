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
fi

mpic++ --prefix /usr/local/share/OpenMPI -o ots ots.cpp

if [ $test -eq 0 ]
then
  dd if=/dev/random bs=1 count=$numbers of=numbers status=none
  mpirun --prefix /usr/local/share/OpenMPI -np $numbers ots
else
  rm -f testfiles/*
  for i in {4..20}
  do
    printf $i >> output
    for j in {1..50}
    do
      if [ -f testfiles/$i ]
      then
        dd if=/dev/random bs=1 count=$i of=testfiles/$j iflag=append conv=notrunc status=none
      else
        dd if=/dev/random bs=1 count=$i of=testfiles/$j status=none
      fi
      mv testfiles/$j numbers
      printf ";" >> output
      mpirun --prefix /usr/local/share/OpenMPI -np $i ots 2&>/dev/null
      printf $? >> output
#      mv numbers testfiles/$j
    done
    printf "\n" >> output
  done
fi

rm -f ots numbers
