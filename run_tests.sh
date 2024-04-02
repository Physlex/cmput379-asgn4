#!/bin/sh

rm -rf logs/
mkdir logs/

make tests
./bin/a4p1_Test $1
