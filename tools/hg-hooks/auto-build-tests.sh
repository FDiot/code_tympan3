#!/bin/bash

# This is only an example hook : it will NOT work out of the box
# You will have to adapt it by specifying your build directory
# as the BASE_DIR variable.

BASE_DIR=~/tmp/build/Code_TYMPAN-default
BUILD_DIR=$BASE_DIR/$1

echo "Updating to current-work..."
TIP_ID=$(hg log -r tip --template '{node|short}\n')
echo "HG_NODE: $HG_NODE - tip: $TIP_ID"
hg up --clean $TIP_ID
echo "Running make for CS $TIP_ID and $BUILD_DIR..."
LOGFILE=make-$TIP_ID.log
make -C $BUILD_DIR -j8  > $LOGFILE 2>&1 &
MAKE_PID=$!
echo "Make Process ID : $MAKE_PID"
