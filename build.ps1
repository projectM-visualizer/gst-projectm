# ------------
# FUNCTIONS

# Prompt user to clean previous build files
function Invoke-PromptCleanBuild {
    param (
        [string]$Auto,
        [string]$Build,
        [string]$Dist
    )

    # Clean up previous build files, if found
    if ((Test-Path "$Build") -or (Test-Path "$Dist")) {
        if ($AUTO -eq "false") {
            # Ask to clean
            Write-Host
            Write-Host "Clean previous build? [Y/n]"
            $Clean = Read-Host
        }
        else {
            $Clean = "Y"
        }
    
        if ($Clean -ne "N" -and $Clean -ne "n") {
            Remove-Item -Recurse -Force -ErrorAction SilentlyContinue -Path "$BUILD"
            Remove-Item -Recurse -Force -ErrorAction SilentlyContinue -Path "$Dist"
        }
    }
    
    # Create build and dist directories, if they don't exist
    if (!(Test-Path "$Build")) {
        New-Item -Path "$Build" -ItemType Directory | Out-Null
    }
    if (!(Test-Path "$Dist")) {
        New-Item -Path "$Dist" -ItemType Directory | Out-Null
    }
}

# Configure build
function Start-ConfigureBuild {
    param (
        [string]$Source,
        [string]$Build
    )

    cmake `
        -G "Visual Studio 17 2022" `
        -A "X64" `
        -S "$Source" `
        -B "$Build" `
        -DCMAKE_TOOLCHAIN_FILE="${Env:VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" `
        -DVCPKG_TARGET_TRIPLET=x64-windows `
        -DCMAKE_MSVC_RUNTIME_LIBRARY="MultiThreaded$<$<CONFIG:Debug>:Debug>DLL" `
        -DCMAKE_VERBOSE_MAKEFILE=YES `
        -DCMAKE_PREFIX_PATH="${Env:PROJECTM_ROOT}/lib/cmake/projectM4"
}

# Copy required DLLs to dist directory
function Read-LibsToDest {
    param (
        [string]$Source,
        [string]$Destination,
        [string[]]$RequiredLibs
    )

    foreach ($Lib in $RequiredLibs) {
        $LibPath = Join-Path $Source "$Lib"
        if (Test-Path $LibPath -PathType Leaf) {
            Copy-Item -Path $LibPath -Destination $Destination -Force
        }
        else {
            Write-Host "Error: $Lib not found in $Source\Release directory."
        }
    }
}

# Build project
function Start-Build {
    param (
        [string]$Build,
        [string]$Dist
    )

    cmake --build "$BUILD" --config "Release" --parallel

    if (Test-Path "$BUILD\Release\gstprojectm.dll") {
        $RequiredBuildLibs = @("gstprojectm.dll", "gstreamer-1.0-0.dll", "gstvideo-1.0-0.dll", "gstgl-1.0-0.dll", "gstpbutils-1.0-0.dll", "glib-2.0-0.dll", "gobject-2.0-0.dll")
        Read-LibsToDest -Source $Build\Release -Destination $Dist -RequiredLibs $RequiredBuildLibs

        $RequiredExternalLibs = @("projectM-4.dll")
        Read-LibsToDest -Source $Env:PROJECTM_ROOT\lib -Destination $Dist -RequiredLibs $RequiredExternalLibs
    }
    else {
        Write-Host "Build failed!"
        exit 1
    }
}

# Prompt user to install build
function Invoke-PromptInstall {
    param (
        [string]$Auto,
        [string]$Dist
    )

    if (Test-Path "$Dist\gstprojectm.dll") {
        if ($Auto -eq "false") {
            # Ask to install
            Write-Host
            Write-Host "Install to gstreamer plugins? [Y/n]"
            $Install = Read-Host
        }
        else {
            $Install = "Y"
        }
    
        if ($Install -ne "N" -and $Install -ne "n") {
            # Use environment variable if set, otherwise use default
            if ($Env:GST_PLUGIN_PATH) {
                $GSTPluginDir = $Env:GST_PLUGIN_PATH
            }
            else {
                $GSTPluginDir = "$Env:USERPROFILE\.gstreamer\1.0\plugins"
                # Set environment variable
                [Environment]::SetEnvironmentVariable("GST_PLUGIN_PATH", $GSTPluginDir, "User")
            }
    
            # Create the destination directory if it doesn't exist
            New-Item -Path "$GSTPluginDir" -ItemType Directory -ErrorAction SilentlyContinue | Out-Null
    
            # Move files to the destination, overwriting if it exists
            $RequiredLibs = @("gstprojectm.dll", "gstreamer-1.0-0.dll", "gstvideo-1.0-0.dll", "gstgl-1.0-0.dll", "gstpbutils-1.0-0.dll", "glib-2.0-0.dll", "gobject-2.0-0.dll", "projectM-4.dll")
            Read-LibsToDest -Source $Dist -Destination $GSTPluginDir -RequiredLibs $RequiredLibs
    
            # Print example command
            Write-Host
            Write-Host "Done! Here's an example command:"
            Write-Host 'gst-launch-1.0 audiotestsrc ! queue ! audioconvert ! projectm ! "video/x-raw,width=512,height=512,framerate=60/1" ! videoconvert ! xvimagesink sync=false'
        }
        else {
            Write-Host
            Write-Host "Done!"
            Write-Host "You can install the plugin manually by moving the files in <$Dist> to <$GSTPluginDir>"
        }
    }
    else {
        Write-Host "Build failed!"
        exit 1
    }
}

# ------------
# Main

# Check if environment variables are set
if (!$Env:PROJECTM_ROOT) {
    if ((Test-Path "C:\Program Files\LibProjectM")) {
        [Environment]::SetEnvironmentVariable("PROJECTM_ROOT", "C:\Program Files\LibProjectM", "User")
    } else {
        Write-Host "PROJECTM_ROOT environment variable not set"
        exit 1
    }
}

if (!$Env:VCPKG_ROOT) {
    Write-Host "VCPKG_ROOT environment variable not set"
    exit 1
}

$Auto = "false"

# Skip prompt if --auto is passed
if ($args[0] -eq "--auto") {
    $Auto = "true"
}

# Set current directory
$Root = Get-Location
$Build = "$Root\build"
$Dist = "$Root\dist"

Invoke-PromptCleanBuild -Auto $Auto -Build $Build -Dist $Dist
Start-ConfigureBuild -Source $Root -Build $Build
Start-Build -Build $Build -Dist $Dist
Invoke-PromptInstall -Auto $Auto -Dist $Dist
