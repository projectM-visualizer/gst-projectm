# Check if environment variables are set
if (!$Env:PROJECTM_ROOT) {
    Write-Host "PROJECTM_ROOT environment variable not set"
    exit 1
}

if (!$Env:VCPKG_ROOT) {
    Write-Host "VCPKG_ROOT environment variable not set"
    exit 1
}

$AUTO = "false"

# Skip prompt if --auto is passed
if ($args[0] -eq "--auto") {
    $AUTO = "true"
}

# Set current directory
$ROOT = Get-Location
$BUILD = "$ROOT\build"
$DIST = "$ROOT\dist"

# Clean up previous build files, if found
if (Test-Path "$BUILD" || Test-Path "$DIST") {
    if ($AUTO -eq "false") {
        # Ask to clean
        Write-Host
        Write-Host "Clean previous build? [Y/n]"
        $CLEAN_ANSWER = Read-Host
    } else {
        $CLEAN_ANSWER = "Y"
    }

    if ($CLEAN_ANSWER -ne "N" -and $CLEAN_ANSWER -ne "n") {
        Remove-Item -Recurse -Force -ErrorAction SilentlyContinue -Path "$BUILD"
        Remove-Item -Recurse -Force -ErrorAction SilentlyContinue -Path "$DIST"
    }
}

if (!(Test-Path "$BUILD")) {
    New-Item -Path "$BUILD" -ItemType Directory | Out-Null
}
if (!(Test-Path "$DIST")) {
    New-Item -Path "$DIST" -ItemType Directory | Out-Null
}

# Configure build
cmake `
    -G "Visual Studio 17 2022" `
    -A "X64" `
    -S "$ROOT" `
    -B "$BUILD" `
    -DCMAKE_TOOLCHAIN_FILE="${Env:VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" `
    -DVCPKG_TARGET_TRIPLET=x64-windows `
    -DCMAKE_MSVC_RUNTIME_LIBRARY="MultiThreaded$<$<CONFIG:Debug>:Debug>DLL" `
    -DCMAKE_VERBOSE_MAKEFILE=YES `
    -DCMAKE_PREFIX_PATH="${Env:PROJECTM_ROOT}/lib/cmake/projectM4"

# Build
cmake --build "$BUILD" --config "Release" --parallel

if (Test-Path "$BUILD\Release\gstprojectm.dll") {
    # Move to dist
    Copy-Item -Path "$BUILD\Release\gstprojectm.dll" -Destination "$DIST\gstprojectm.dll" -Force

    if ($AUTO -eq "false") {
        # Ask to install
        Write-Host
        Write-Host "Install to gstreamer plugins? [Y/n]"
        $INSTALL_ANSWER = Read-Host
    } else {
        $INSTALL_ANSWER = "Y"
    }

    if ($INSTALL_ANSWER -ne "N" -and $INSTALL_ANSWER -ne "n") {
        # Use environment variable if set, otherwise use default
        if ($Env:GST_PLUGIN_PATH) {
            $GST_PLUGINS_DIR = $Env:GST_PLUGIN_PATH
        }
        else {
            $GST_PLUGINS_DIR = "$Env:USERPROFILE\.gstreamer\1.0\plugins"
            # Set environment variable
            [Environment]::SetEnvironmentVariable("GST_PLUGIN_PATH", $GST_PLUGINS_DIR, "User")
        }

        # Create the destination directory if it doesn't exist
        New-Item -Path "$GST_PLUGINS_DIR" -ItemType Directory -ErrorAction SilentlyContinue | Out-Null

        # Move the file to the destination, overwriting if it exists
        Copy-Item -Path "$DIST\gstprojectm.dll" -Destination "$GST_PLUGINS_DIR\gstprojectm.dll" -Force

        # Print example command
        Write-Host
        Write-Host "Done! Here's an example command:"
        Write-Host 'gst-launch-1.0 audiotestsrc ! queue ! audioconvert ! projectm ! "video/x-raw,width=512,height=512,framerate=60/1" ! videoconvert ! xvimagesink sync=false'
    }
    else {
        Write-Host
        Write-Host "Done!"
        Write-Host "You can install the plugin manually by moving <dist\libgstprojectm.dll> to <$GST_PLUGINS_DIR\libgstprojectm.dll>"
    }
}
else {
    Write-Host "Build failed!"
    exit 1
}