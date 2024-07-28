#!/bin/bash

# Default to Release mode
BUILD_TYPE=${BUILD_TYPE:-Release}

# Create build directory if it doesn't exist
mkdir -p build

# Check if CMakeCache.txt exists
if [ ! -f build/CMakeCache.txt ]; then
    # Configure CMake only if CMakeCache.txt doesn't exist
    cmake -S . -B build -DCMAKE_BUILD_TYPE=$BUILD_TYPE
fi

# Write the output to a temp file and print it if the command fails
temp=$(mktemp)
cmake --build build -j$(nproc) > "$temp" 2>&1

if [ $? -ne 0 ]; then
    echo "Build failed:"
    cat "$temp"
    exit 1
fi
rm "$temp"

# Run the executable
./build/jo "$@"
