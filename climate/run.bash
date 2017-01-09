#!/bin/bash

#Below is load path for the library
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../ubuntu/ssh2/lib:../ubuntu

echo "Number of nodes? {1, 2, 4, 8, 16}: "
read NUMNODES
echo "Number of threads? {1, 2, 4}: "
read NUMTHREADS
echo "Size? "
read SIZE
echo "Port? "
read PORT
echo "Password? "
read -s PASSWORD

head -$(($NUMNODES-1)) machinefile.txt > .tempmachinefile.txt

./main $USER $PASSWORD .tempmachinefile.txt $PORT $NUMNODES $NUMTHREADS $SIZE

rm .tempmachinefile.txt
