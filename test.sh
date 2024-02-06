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

# Check if output directory exists, if not create it
if [ ! -d "test/output" ]; then
    mkdir -p test/output
fi

echo
echo

case "$1" in
    "--details")
        gst-inspect-1.0 --plugin projectm
        ;;
    
    "--inspect")
        gst-inspect-1.0 projectm
        ;;

    "--audio")
        GST_DEBUG=projectm:5 gst-launch-1.0 -v \
            audiotestsrc ! queue ! audioconvert ! \
            projectm \
            ! "video/x-raw,width=512,height=512,framerate=60/1" ! videoconvert ! xvimagesink sync=false
        ;;

    "--preset")
        GST_DEBUG=4 gst-launch-1.0 -v \
            audiotestsrc ! queue ! audioconvert ! \
            projectm preset="test/presets/250-wavecode.milk.milk" \
            ! "video/x-raw,width=512,height=512,framerate=60/1" ! videoconvert ! xvimagesink sync=false
        ;;

    "--properties")
        GST_DEBUG=3 gst-launch-1.0 -v \
            audiotestsrc ! queue ! audioconvert ! \
            projectm \
            preset="test/presets/250-wavecode.milk.milk" \
            texture-dir="test/textures" \
            beat-sensitivity=0.5 \
            hard-cut-duration=1 \
            hard-cut-enabled=true \
            hard-cut-sensitivity=0.5 \
            soft-cut-duration=1 \
            preset-duration=30 \
            mesh-size="512,512" \
            easter-egg=0.75 \
            preset-locked=false \
            ! "video/x-raw,width=512,height=512,framerate=30/1" ! videoconvert ! xvimagesink sync=false
        ;;

    "--output-video")
        GST_DEBUG=3 gst-launch-1.0 -v \
            filesrc location="test/audio/upbeat-future-bass.mp3" ! decodebin ! audioconvert ! \
            projectm preset="test/presets/250-wavecode.milk.milk" ! videoscale ! videoconvert ! video/x-raw,width=1280,height=720 ! \
            x264enc ! mp4mux ! filesink location="test/output/test_video.mp4"
        ;;

    "--encode-output-video")
        GST_DEBUG=3 gst-launch-1.0 -v \
            filesrc location="test/audio/upbeat-future-bass.mp3" ! decodebin name=dec ! \
            audioconvert ! avenc_aac ! avmux_mp4 ! filesink location="test/output/video2.mp4" \
            dec. ! \
            projectm preset="test/presets/250-wavecode.milk.milk" ! videoconvert ! x264enc ! avenc_mp4 ! avmux_mp4.video_0
        ;;

    *)
        echo "Usage: $0 [--details|--inspect|--audio|--preset|--properties|--output-video|--encode-output-video]"
        exit 1
        ;;
esac
