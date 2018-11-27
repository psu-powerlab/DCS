#!/bin/bash

# setup environment
export CPU=x86_64

# build
export SRC=dcs
make -C ../build

# run
./../build/bin/debug/$SRC
