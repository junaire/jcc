#!/bin/bash

# This will cause the shell to exit immediately if a simple command exits with a nonzero exit value.
set -e

DIR="$( cd "$( dirname "$0" )" && pwd )"

JCC=${DIR}/../../build/bin/jcc

for i in $(ls ${DIR}/*.c); do
    echo "Running" ${i}
    ${JCC} ${i} --ast-dump > ${i%.c}.out
done
