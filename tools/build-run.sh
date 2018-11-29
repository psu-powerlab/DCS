#!/bin/bash

# setup environment
export CPU=x86_64
export OS=linux
export VARIANT=debug
export AJ_ROOT=$HOME/src/core-alljoyn/build/$OS/$CPU/$VARIANT/dist/cpp
export AJ_LIB=$AJ_ROOT/lib
export AJ_INC=$AJ_ROOT/inc
export LD_LIBRARY_PATH=$AJ_LIB:$LD_LIBRARY_PATH
export CTA_ROOT=~/src/CTA-2045-UCM-CPP-Library
export CTA_LIB=$CTA_ROOT/build/debug
export CTA_INC=$CTA_ROOT/cea2045
export LD_LIBRARY_PATH=$CTA_LIB:$LD_LIBRARY_PATH

# build
export SRC=dcs
make -C ../build

# run
./../build/bin/debug/$SRC -c /home/tylor/dev/powerlab/DCS/data/config.ini -o y
