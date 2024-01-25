# Check if gst-inspect-1.0 is installed
if ! command -v gst-inspect-1.0 > /dev/null 2>&1; then
    echo "gst-inspect-1.0 is not installed. Please install it and try again."
    exit 1
fi

# Check if gst-launch-1.0 is installed
if ! command -v gst-launch-1.0 > /dev/null 2>&1; then
    echo "gst-launch-1.0 is not installed. Please install it and try again."
    exit 1
fi

# Check if plugin is installed
if [ ! -f "$HOME/.local/share/gstreamer-1.0/plugins/libgstprojectm.so" ]; then
    echo "libgstprojectm.so is missing. Please install it and try again."
    exit 1
fi

echo
# gst-inspect-1.0 projectm

GST_DEBUG=3 gst-launch-1.0 audiotestsrc ! queue ! audioconvert ! projectm ! "video/x-raw,width=512,height=512,framerate=60/1" ! videoconvert ! xvimagesink sync=false