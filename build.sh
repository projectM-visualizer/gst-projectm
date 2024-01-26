#!/bin/bash
set -e

# ------------
# FUNCTIONS

# Prompt user to clean previous build files
prompt_clean_build() {
    # Clean up previous build files, if found
    if [ -d "$2" ] || [ -d "$3" ]; then
        if [ $1 = false ]; then
            echo
            echo -n "Clean up previous build files? (Y/n): "
            read -r clean
        else
            clean="Y"
        fi
        
        if [[ "$clean" != "N" && "$clean" != "n" ]]; then
            if [ -d "$2" ]; then
                rm -rf "$2"
            fi
            
            if [ -d "$3" ]; then
                rm -rf "$3"
            fi
        fi
    fi
    
    # Create build and dist directories, if they don't exist
    if [ ! -d "$2" ] ; then
        mkdir -p "$2"
    fi
    
    if [ ! -d "$3" ] ; then
        mkdir -p "$3"
    fi
}

# Configure build
configure_build() {
    cmake \
    -G "Ninja" \
    -S "$1" \
    -B "$2" \
    -DCMAKE_VERBOSE_MAKEFILE=YES \
    -DCMAKE_PREFIX_PATH="$PROJECTM_ROOT/lib/cmake/projectM4"
}

# Build project
build() {
    cmake --build "$1" --config "Release" --parallel
    
    # Move to dist
    if [ -f "$1/libgstprojectm.so" ] ; then
        mv "$1/libgstprojectm.so" "$2"
    else
        echo "Build failed!"
        exit 1
    fi
}

# Prompt user to install build
prompt_install() {
    # Install to gstreamer plugin, if found
    if [ -f "$2/libgstprojectm.so" ] ; then
        if [ "$1" = false ] ; then
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
            mv "$2/libgstprojectm.so" "$HOME/.local/share/gstreamer-1.0/plugins/"
            
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
}


# ------------
# Main

# Check if environment variables are set, if needed
if [ -d "/usr/local/include/projectM-4" ] ; then
    export PROJECTM_ROOT="/usr/local"
else
    if [ -z "$PROJECTM_ROOT" ] ; then
        echo "PROJECTM_ROOT environment variable is not set!"
        exit 1
    fi
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

prompt_clean_build "$AUTO" "$BUILD" "$DIST"
configure_build "$ROOT" "$BUILD"
build "$BUILD" "$DIST"
prompt_install "$AUTO" "$DIST"