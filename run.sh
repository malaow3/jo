#!/bin/bash

# Default to Release mode
BUILD_TYPE=${BUILD_TYPE:-Release}

# Create build directory if it doesn't exist
if [ ! -d build ]; then
    cmake --preset=default
fi

# Check if CMakeCache.txt exists
if [ ! -f build/CMakeCache.txt ]; then
    # Configure CMake only if CMakeCache.txt doesn't exist
    cmake -S . -B build -DCMAKE_BUILD_TYPE=$BUILD_TYPE
fi

# Write the output to a temp file and print it if the command fails
temp=$(mktemp)
# if on mac, we can't use -j$(nproc) so we'll use something else
if [ "$(uname)" == "Darwin" ]; then
    cmake --build build -j$(sysctl -n hw.logicalcpu) > "$temp" 2>&1
else
    cmake --build build -j$(nproc) > "$temp" 2>&1
fi

if [ $? -ne 0 ]; then
    echo "Build failed:"
    cat "$temp"
    exit 1
fi
rm "$temp"

# Run the executable
./build/jo "$@"
