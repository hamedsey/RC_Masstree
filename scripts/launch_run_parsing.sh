#!/bin/bash

# Check if directory is provided
if [ -z "$1" ]; then
    echo "Usage: $0 <directory>"
    exit 1
fi

# Assign the first argument to a variable
BASE_DIR="$1"

# Check if the directory exists
if [ ! -d "$BASE_DIR" ]; then
    echo "Error: Directory '$BASE_DIR' does not exist."
    exit 1
fi

# Iterate through subdirectories
for dir in "$BASE_DIR"/*/; do
    if [ -d "$dir" ]; then
        echo "Entering directory: $dir"
        
        # Check if collect_latency.py exists in the directory
        if [ -f "$dir/collect_latency.py" ]; then
            cd $dir
            echo "Executing collect_latency.py in $dir"
            python3 collect_latency.py &
        else
            echo "No collect_latency.py found in $dir"
        fi
    fi
done
