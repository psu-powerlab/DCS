#!/bin/bash

# setup environment

# build
export SRC=dcs
make -C ../build

# run
./../build/bin/debug/$SRC
