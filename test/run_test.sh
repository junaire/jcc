#!/bin/bash

DIR="$( cd "$( dirname "$0" )" && pwd )"

for i in $(ls ${DIR}/*.c); do
    ${DIR}/../build/bin/jcc ${i}
done
