#!/bin/bash
set -e

# ------------
# FUNCTIONS: PACKAGES

# Install packages based on the selected package manager: apt, pacman, brew
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

# Prompt user to install dependencies, (and choose a package manager, if multiple are available)
prompt_install_dependencies() {
    if [ $1 = false ]; then
        echo
        echo -n "Install dependencies? (Y/n): "
        read -r install_deps
    else
        install_deps="n"
    fi
    
    if [[ "$install_deps" != "N" && "$install_deps" != "n" ]]; then
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
        
    else
        echo
        echo "Skipping dependency installation."
    fi
}

# ------------
# FUNCTIONS: FIXES

# Fix missing "#include <gst/gl/gstglconfig.h>"
fix_missing_gstglconfig_h() {
    if [ ! -f "/usr/include/gstreamer-1.0/gst/gl/gstglconfig.h" ]; then
        echo
        echo "Fixing missing gst/gl/gstglconfig.h..."
        echo
        
        # Create missing header file
        sudo touch /usr/include/gstreamer-1.0/gst/gl/gstglconfig.h
        
        # Add contents to header file
        sudo tee /usr/include/gstreamer-1.0/gst/gl/gstglconfig.h <<EOF
/* gstglconfig.h */

#ifndef __GST_GL_CONFIG_H__
#define __GST_GL_CONFIG_H__

#include <gst/gst.h>

G_BEGIN_DECLS


#define GST_GL_HAVE_OPENGL 1
#define GST_GL_HAVE_GLES2 1
#define GST_GL_HAVE_GLES3 1
#define GST_GL_HAVE_GLES3EXT3_H 1

#define GST_GL_HAVE_WINDOW_X11 1
#define GST_GL_HAVE_WINDOW_COCOA 0
#define GST_GL_HAVE_WINDOW_WIN32 0
#define GST_GL_HAVE_WINDOW_WINRT 0
#define GST_GL_HAVE_WINDOW_WAYLAND 1
#define GST_GL_HAVE_WINDOW_ANDROID 0
#define GST_GL_HAVE_WINDOW_DISPMANX 0
#define GST_GL_HAVE_WINDOW_EAGL 0
#define GST_GL_HAVE_WINDOW_VIV_FB 0
#define GST_GL_HAVE_WINDOW_GBM 1

#define GST_GL_HAVE_PLATFORM_EGL 1
#define GST_GL_HAVE_PLATFORM_GLX 1
#define GST_GL_HAVE_PLATFORM_WGL 0
#define GST_GL_HAVE_PLATFORM_CGL 0
#define GST_GL_HAVE_PLATFORM_EAGL 0

#define GST_GL_HAVE_DMABUF 1
#define GST_GL_HAVE_VIV_DIRECTVIV 0

#define GST_GL_HAVE_GLEGLIMAGEOES 1
#define GST_GL_HAVE_GLCHAR 1
#define GST_GL_HAVE_GLSIZEIPTR 1
#define GST_GL_HAVE_GLINTPTR 1
#define GST_GL_HAVE_GLSYNC 1
#define GST_GL_HAVE_GLUINT64 1
#define GST_GL_HAVE_GLINT64 1
#define GST_GL_HAVE_EGLATTRIB 1
#define GST_GL_HAVE_EGLUINT64KHR 1

G_END_DECLS

#endif  /* __GST_GL_CONFIG_H__ */
EOF
    fi
}

# Prompt user to apply fixes
prompt_apply_fixes() {
    if [ $1 = false ]; then
        echo
        echo "The following fixes are available:"
        echo "  - Missing gst/gl/gstglconfig.h"
        echo -n "Apply fixes? (Y/n): "
        read -r apply_fixes
    else
        install_deps="n"
    fi
    
    if [[ "$apply_fixes" != "N" && "$apply_fixes" != "n" ]]; then
        fix_missing_gstglconfig_h
    else
        echo
        echo "Skipping fixes."
    fi
}

# ------------
# Main

AUTO=false

# Skip prompt if --auto is passed
if [ "$1" = "--auto" ] ; then
    AUTO=true
    elif [ "$1" = "--fixes" ] ; then
    prompt_apply_fixes true
    exit 0
fi

prompt_install_dependencies $AUTO
