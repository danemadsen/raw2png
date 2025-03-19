#!/bin/bash

EXTENSION="raw16"
RAW_DIR="./raw"
OUTPUT_DIR="./output"

# Ensure output directory exists
mkdir -p "$OUTPUT_DIR"

# Loop through all .raw files in RAW_DIR
for raw_file in "$RAW_DIR"/*.$EXTENSION; do
    # Extract filename without path and extension
    filename=$(basename "$raw_file" .$EXTENSION)

    # Construct output filename
    output_file="$OUTPUT_DIR/$filename.png"

    echo "Converting $raw_file to $output_file"

    # Run your conversion program
    ./raw2png "$raw_file" "$output_file"
done