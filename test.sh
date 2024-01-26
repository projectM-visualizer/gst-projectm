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

# GST_DEBUG=3 gst-launch-1.0 -v \
#     audiotestsrc ! queue ! audioconvert ! \
#     projectm preset="test/presets/215-wave.milk" \
#     ! "video/x-raw,width=512,height=512,framerate=60/1" ! videoconvert ! xvimagesink sync=false

# GST_DEBUG=3 gst-launch-1.0 -v \
#   filesrc location="test/audio/blast.mp3" ! decodebin ! audioconvert ! \
#   projectm ! videoconvert ! x264enc ! mp4mux ! filesink location="test/output/test_video.mp4"

GST_DEBUG=3 gst-launch-1.0 -v \
  filesrc location="test/audio/upbeat-future-bass.mp3" ! decodebin name=dec ! \
  audioconvert ! avenc_aac ! avmux_mp4 ! filesink location="test/output/video2.mp4" \
  dec. ! \
  projectm ! videoconvert ! x264enc ! avenc_mp4 ! avmux_mp4.video_0
