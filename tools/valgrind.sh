#!/bin/bash

SRC_ROOT_DIR=$(readlink -m "$(dirname $0)/../")
echo "LD_LIBRARY_PATH=$SRC_ROOT_DIR/lib"
LD_LIBRARY_PATH="$SRC_ROOT_DIR/lib" valgrind --tool=memcheck --trace-children=yes --tool=drd --trace-alloc=yes --log-file=valgrind-%p.log bin/libOrionDecoder
