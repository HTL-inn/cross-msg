#!/bin/bash

make clean
make
echo ""
./build/core -D 0
