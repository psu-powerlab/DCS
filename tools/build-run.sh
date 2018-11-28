#!/bin/bash

# setup environment
export CPU=x86_64

# build
export SRC=dcs
make -C ../build

# run
./../build/bin/debug/$SRC -c /home/tylor/dev/powerlab/DCS/data/config.ini -o y
