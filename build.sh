#!/bin/bash
set -e

# Check if environment variables are set, if needed
if [ -d "/usr/local/include/projectM-4" ] ; then
    export PROJECTM_ROOT="/usr/local"
else
    if [ -z "$PROJECTM_ROOT" ] ; then
        echo "PROJECTM_ROOT is not set!"
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

# Ask to install dependencies
if [ $AUTO = false ]; then
    echo
    echo -n "Install dependencies? (y/N): "
    read -r install_deps
else
    install_deps="N"
fi

if [[ "$install_deps" != "Y" && "$install_deps" != "Y" ]]; then
    echo
    echo "Skipping dependency installation."
else
    # Function to install packages based on the package manager
    install_packages() {
        local package_manager=$1
        local package_list=("${!2}")

        case $package_manager in
            apt)
                sudo apt update
                sudo apt install "${package_list[@]}"
                ;;
            pacman)
                sudo pacman -Syyu
                sudo pacman -S "${package_list[@]}"
                ;;
            brew)
                brew update
                brew install "${package_list[@]}"
                ;;
        esac
    }

    # Check for available package managers
    available_package_managers=()

    if command -v apt &>/dev/null; then
        available_package_managers+=("apt")
    fi

    if command -v pacman &>/dev/null; then
        available_package_managers+=("pacman")
    fi

    if command -v brew &>/dev/null; then
        available_package_managers+=("brew")
    fi

    # Prompt user to choose a package manager
    if [ ${#available_package_managers[@]} -eq 0 ]; then
        echo "No supported package managers found."
        exit 1
    elif [ ${#available_package_managers[@]} -eq 1 ] || [ $AUTO = true ]; then
        selected_package_manager=${available_package_managers[0]}
    else
        echo -n "Multiple package managers found. Please choose one:"
        select selected_package_manager in "${available_package_managers[@]}"; do
            if [ -n "$selected_package_manager" ]; then
                break
            else
                echo "Invalid selection. Please choose a number."
            fi
        done
    fi

    # Install packages based on the selected package manager
    case $selected_package_manager in
        apt)
            package_list=("ninja-build" "mesa-common-dev" "build-essential" "git" "cmake" "pkg-config" "libgstreamer1.0-dev" "libgstreamer-gl1.0-0" "libgstreamer-plugins-base1.0-dev")
            install_packages "$selected_package_manager" package_list[@]
            ;;
        pacman)
            package_list=("base-devel" "ninja" "cmake" "mesa" "gst-plugins-base-libs")
            install_packages "$selected_package_manager" package_list[@]
            ;;
        brew)
            package_list=("git" "ninja" "cmake" "gstreamer")
            install_packages "$selected_package_manager" package_list[@]
            ;;
    esac
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
