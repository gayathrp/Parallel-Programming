#!/bin/sh

export MASS_DIR=../..
g++ -Wall MyPlace.cpp -I$MASS_DIR/source -shared -fPIC -o MyPlace
g++ -Wall main.cpp Timer.cpp -I$MASS_DIR/source -L$MASS_DIR/ubuntu -lmass -I$MASS_DIR/ubuntu/ssh2/include -L$MASS_DIR/ubuntu/ssh2/lib -lssh2 -o main
