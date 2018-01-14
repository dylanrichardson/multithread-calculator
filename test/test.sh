#!/bin/bash

cd "$(dirname "$0")/.."

function runConfig
{
    echo "Configuration $1:"
    cat $1
    # run with graph
    echo ""
    echo "Running with graph/graph:"
    graph/graph $1
    # run with nblock
    echo ""
    echo "Running with nblock/nblock:"
    nblock/nblock $1
    echo ""
}

for filename in config/*; do
    runConfig $filename
done
