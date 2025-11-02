#!/bin/bash

# Check if the correct number of arguments is provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <path>"
    exit 1
fi

# Get the input path
INPUT_PATH="$1"

# Check if the input path exists and is a directory
if [ ! -d "$INPUT_PATH" ]; then
    echo "Error: Directory $INPUT_PATH does not exist."
    exit 1
fi

# Loop through subdirectories 20, 21, 22
for dir in 20 21 22; do
    SUBDIR="$INPUT_PATH/$dir"
    
    if [ -d "$SUBDIR" ]; then
        # Loop through all sub-subdirectories
        for SUBSUBDIR in "$SUBDIR"/*/; do
            if [ -d "$SUBSUBDIR" ]; then
                echo "Processing $SUBSUBDIR"
                
                # Navigate to the sub-subdirectory
                cd "$SUBSUBDIR" || continue
                
                # Execute the Python script
                python3 combineCSVs.py
                
                # Find the generated CSV file and move it to the original input path
                CSV_FILE=$(find "$SUBSUBDIR" -maxdepth 1 -type f -name "*.csv")
                if [ -n "$CSV_FILE" ]; then
                    mv "$CSV_FILE" "$INPUT_PATH"
                    echo "Moved $CSV_FILE to $INPUT_PATH"
                else
                    echo "No CSV file found in $SUBSUBDIR"
                fi
            fi
        done
    else
        echo "Warning: Directory $SUBDIR does not exist. Skipping."
    fi
done

echo "Script completed."

