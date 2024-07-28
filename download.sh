#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <platform>"
    echo "Where <platform> is either 'mac' or 'linux'"
    exit 1
fi

platform=$1
repo="malaow3/jo"  # Replace with your actual GitHub username and repo name

if [ "$platform" == "mac" ]; then
    artifact_name="jo_mac"
elif [ "$platform" == "linux" ]; then
    artifact_name="jo_linux"
else
    echo "Invalid platform. Use 'mac' or 'linux'."
    exit 1
fi

# Get the ID of the latest successful workflow run
run_id=$(gh run list --workflow "Build and Publish" --json conclusion,databaseId --jq '.[] | select(.conclusion=="success") | .databaseId' --limit 1)

if [ -z "$run_id" ]; then
    echo "No successful workflow run found."
    exit 1
fi

# Download the artifact
gh run download $run_id -n $artifact_name

mkdir -p build
echo "Downloaded $artifact_name from the latest successful workflow run."
chmod +x $artifact_name

# Move the binary to the build directory
mv $artifact_name build/
