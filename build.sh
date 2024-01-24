#!/bin/bash
set -e

# Check if environment variables are set
if [ -z "$PROJECTM_ROOT" ] ; then
    echo "PROJECTM_ROOT is not set!"
    exit 1
fi

AUTO=false

# Skip prompt if --auto is passed
if [ "$1" = "--auto" ] ; then
    AUTO=true
fi

# Set current directory
ROOT="$(pwd)"
BUILD="$ROOT/build"
DIST="$ROOT/dist"

# Install needed packages
if command -v apt &>/dev/null; then
    APT_PACKAGE_LIST=("ninja-build" "mesa-common-dev" "build-essential" "git" "cmake" "pkg-config" "libgstreamer1.0-dev" "libgstreamer-gl1.0-0" "libgstreamer-plugins-base1.0-dev")
    
    # Check for required packages before updating and installing
    for PACKAGE in "${APT_PACKAGE_LIST[@]}"; do
        if ! dpkg -s "$PACKAGE" >/dev/null 2>&1 ; then
            sudo apt update
            sudo apt install "${APT_PACKAGE_LIST[@]}"
            break
        fi
    done
fi

if command -v pacman &>/dev/null; then
    # PACMAN_PACKAGE_LIST="base-devel ninja cmake mesa gst-plugins-base-libs"
    PACMAN_PACKAGE_LIST=("base-devel" "ninja" "cmake" "mesa" "gst-plugins-base-libs")
    
    # Check for required packages before updating and installing
    for PACKAGE in "${PACMAN_PACKAGE_LIST[@]}"; do
        if ! pacman -Q "$PACKAGE" >/dev/null 2>&1 ; then
            sudo pacman -Syu
            sudo pacman -S "${PACMAN_PACKAGE_LIST[@]}"
            break
        fi
    done
fi

if command -v brew &>/dev/null; then
    BREW_PACKAGE_LIST=("git" "ninja" "cmake" "gstreamer")

    # Check for required packages before updating and installing
    for PACKAGE in "${BREW_PACKAGE_LIST[@]}"; do
        if ! brew list | grep -q "$PACKAGE"; then
            brew update
            brew install "${BREW_PACKAGE_LIST[@]}"
            break
        fi
    done
fi

# Clean up previous build files, if found
if [ -d "$BUILD" ] || [ -d "$DIST" ]; then
    if [ $AUTO = false ]; then
        # Ask to clean up previous build files
        echo
        echo -n "Clean up previous build files? (Y/n): "
        read -r clean
    else
        clean="Y"
    fi

    if [[ "$clean" != "N" && "$clean" != "n" ]]; then
        if [ -d "$BUILD" ]; then
            rm -rf "$BUILD"
        fi

        if [ -d "$DIST" ]; then
            rm -rf "$DIST"
        fi
    fi
fi

if [ ! -d "$BUILD" ] ; then
    mkdir -p "$BUILD"
fi

if [ ! -d "$DIST" ] ; then
    mkdir -p "$DIST"
fi

# Configure build
cmake \
    -G "Ninja" \
    -S "$ROOT" \
    -B "$BUILD" \
    -DCMAKE_VERBOSE_MAKEFILE=YES \
    -DCMAKE_PREFIX_PATH="$PROJECTM_ROOT/lib/cmake/projectM4"

# Build
cmake --build "$BUILD" --config "Release" --parallel

if [ -f "$BUILD/libgstprojectm.so" ] ; then
    # Move to dist
    mv "$BUILD/libgstprojectm.so" "$DIST"

    if [ "$AUTO" = false ] ; then
        # Ask to install
        echo
        echo -n "Install to gstreamer plugins? (Y/n): "
        read -r install
    else
        install="Y"
    fi

    if [[ "$install" != "N" && "$install" != "n" ]] ; then
        # Create the destination directory if it doesn't exist
        mkdir -p "$HOME/.local/share/gstreamer-1.0/plugins/"
        
        # Move the file to the destination, overwriting if it exists
        mv "$DIST/libgstprojectm.so" "$HOME/.local/share/gstreamer-1.0/plugins/"

        # Print example command
        echo
        echo "Done! Here's an example command:"
        echo 'gst-launch-1.0 audiotestsrc ! queue ! audioconvert ! projectm ! "video/x-raw,width=512,height=512,framerate=60/1" ! videoconvert ! xvimagesink sync=false'
    else
        echo
        echo "Done!"
        echo 'You can install the plugin manually by moving <dist/libgstprojectm.so> to <$HOME/.local/share/gstreamer-1.0/plugins/libgstprojectm.so>'
    fi
else
    echo "Build failed!"
    exit 1
fi
