#!/bin/bash
export DYLD_LIBRARY_PATH=~/opt/llvm/bld/lib/:$DYLD_LIBRARY_PATH
./dist/clangDoc $@
