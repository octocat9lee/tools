#!/bin/bash
cd ${0%/*}
LD_LIBRARY_PATH=../lib ./exe "$@"

