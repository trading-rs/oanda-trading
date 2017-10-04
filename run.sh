#!/bin/bash
set +e
set -x

make clean
make all
./main
