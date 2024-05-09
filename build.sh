#!/bin/bash
set -e

# Set variables based on OS
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    LIB_EXT="so"
    VIDEO_SINK="xvimagesink"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    LIB_EXT="dylib"
    VIDEO_SINK="osxvideosink"
else
    echo "Unsupported OS!"
    exit 1
fi

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
    if [ -f "$1/libgstprojectm.$LIB_EXT" ] ; then
        mv "$1/libgstprojectm.$LIB_EXT" "$2"
    else
        echo "Build failed!"
        exit 1
    fi
}

# Prompt user to install build
prompt_install() {
    # Install to gstreamer plugin, if found
    if [ -f "$2/libgstprojectm.$LIB_EXT" ]; then
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
            if [ -f "$2/libgstprojectm.$LIB_EXT" ] ; then
                mv "$2/libgstprojectm.$LIB_EXT" "$HOME/.local/share/gstreamer-1.0/plugins/"
            else
                echo "Install failed!"
                exit 1
            fi
            
            # Print example command
            echo
            echo "Done! Here's an example command:"
            echo "gst-launch-1.0 audiotestsrc ! queue ! audioconvert ! projectm ! "video/x-raw,width=512,height=512,framerate=60/1" ! videoconvert ! $VIDEO_SINK sync=false"
        else
            echo
            echo "Done!"
            echo "You can install the plugin manually by moving <dist/libgstprojectm.$LIB_EXT> to <$HOME/.local/share/gstreamer-1.0/plugins/libgstprojectm.$LIB_EXT>"
        fi
    else
        echo "Install failed!"
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