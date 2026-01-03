#!/bin/bash
# Build script for Linux/MinGW

echo "Building KeyboardMouseMap..."
echo

# Create build directory if it doesn't exist
mkdir -p build

# Enter build directory
cd build

# Generate build files
echo "Generating build files with CMake..."
cmake ..

if [ $? -ne 0 ]; then
    echo
    echo "ERROR: CMake configuration failed!"
    echo "Make sure CMake is installed."
    exit 1
fi

echo
echo "Building project..."
cmake --build . --config Release

if [ $? -ne 0 ]; then
    echo
    echo "ERROR: Build failed!"
    exit 1
fi

echo
echo "======================================"
echo "Build completed successfully!"
echo "======================================"
echo
echo "Executable location: build/bin/KeyboardMouseMap.exe"
echo
echo "To run the application:"
echo "  cd build/bin"
echo "  ./KeyboardMouseMap.exe"
echo
