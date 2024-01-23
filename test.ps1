# Check if gst-inspect-1.0 is installed
if (!(Get-Command gst-inspect-1.0 -ErrorAction SilentlyContinue)) {
    Write-Host "Unable to find: gst-inspect-1.0"
    exit 1
}

# Check if gst-launch-1.0 is installed
if (!(Get-Command gst-launch-1.0 -ErrorAction SilentlyContinue)) {
    Write-Host "Unable to find: gst-launch-1.0"
    exit 1
}

# Check if plugin is installed
if (!(Test-Path "$Env:GST_PLUGIN_PATH\gstprojectm.dll")) {
    Write-Host "Missing plugin: gstprojectm.dll"
    exit 1
}

Write-Host
gst-inspect-1.0 projectm

gst-launch-1.0 audiotestsrc ! queue ! audioconvert ! projectm ! "video/x-raw,width=512,height=512,framerate=60/1" ! videoconvert ! xvimagesink sync=false