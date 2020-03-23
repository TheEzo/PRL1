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

mpic++ --prefix /usr/local/share/OpenMPI -o oets odd-even.cpp

if [ $test -eq 0 ]
then
  dd if=/dev/random bs=1 count=$numbers of=numbers status=none
  mpirun --prefix /usr/local/share/OpenMPI -np $numbers oets
else
  rm testfiles/*
  for i in {4..30}
  do
    printf $i >> output
    for j in {1..50}
    do
      dd if=/dev/random bs=1 count=$i of=testfiles/$i iflag=append conv=notrunc status=none
      mv testfiles/$i numbers
      printf ";" >> output
      mpirun --prefix /usr/local/share/OpenMPI -np $i oets
      printf $? >> output
      mv numbers testfiles/$i
    done
    printf "\n" >> output
  done
fi


#spusteni
mpirun --prefix /usr/local/share/OpenMPI -np $numbers oets

#uklid
rm -f oets numbers
