#!/bin/bash
set -e

# Default values
PRESET_PATH="/usr/local/share/projectM/presets"
TEXTURE_DIR="/usr/local/share/projectM/textures"
PRESET_DURATION=4
MESH_X=256
MESH_Y=144
VIDEO_WIDTH=1920
VIDEO_HEIGHT=1080
FRAMERATE=60
BITRATE=8000
SPEED_PRESET="medium"

# Process ID for the gst-launch process
GST_PID=""

# Signal handler for proper termination
cleanup() {
    echo ""
    echo "Caught signal, stopping conversion..."
    if [ ! -z "$GST_PID" ]; then
        kill -INT $GST_PID 2>/dev/null || true
        sleep 1
        # If it's still running, try harder
        kill -TERM $GST_PID 2>/dev/null || true
    fi

    # Kill Xvfb if running
    pkill Xvfb || true

    exit 0
}

# Setup signal traps for proper termination
trap cleanup INT TERM

# Display help information
show_help() {
    echo "ProjectM Audio to Video Converter"
    echo ""
    echo "Usage: $0 [options] -i input_file -o output_file"
    echo ""
    echo "Options:"
    echo "  -i, --input FILE       Input audio file path (required)"
    echo "  -o, --output FILE      Output video file path (required)"
    echo "  -p, --preset DIR       Path to projectM preset directory (default: $PRESET_PATH)"
    echo "  -t, --texture DIR      Path to projectM texture directory (default: $TEXTURE_DIR)"
    echo "  -d, --duration SEC     Preset duration in seconds (default: $PRESET_DURATION)"
    echo "  --mesh WxH             Mesh size (default: ${MESH_X}x${MESH_Y})"
    echo "  --video-size WxH       Output video size (default: ${VIDEO_WIDTH}x${VIDEO_HEIGHT})"
    echo "  -r, --framerate FPS    Output video framerate (default: $FRAMERATE)"
    echo "  -b, --bitrate KBPS     Output video bitrate in kbps (default: $BITRATE)"
    echo "  --speed PRESET         x264 encoding speed preset (default: $SPEED_PRESET)"
    echo "                         Options: ultrafast, superfast, veryfast, faster, fast, medium, slow, slower, veryslow"
    echo "  -h, --help             Display this help message and exit"
    echo ""
    echo "Example:"
    echo "  $0 -i input.mp3 -o output.mp4 --video-size 3840x2160 -r 30"
    echo ""
    exit 1
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    key="$1"
    case $key in
        -i|--input)
            INPUT_FILE="$2"
            shift 2
            ;;
        -o|--output)
            OUTPUT_FILE="$2"
            shift 2
            ;;
        -p|--preset)
            PRESET_PATH="$2"
            shift 2
            ;;
        -t|--texture)
            TEXTURE_DIR="$2"
            shift 2
            ;;
        -d|--duration)
            PRESET_DURATION="$2"
            shift 2
            ;;
        --mesh)
            MESH_SIZE="$2"
            MESH_X=$(echo $MESH_SIZE | cut -d'x' -f1)
            MESH_Y=$(echo $MESH_SIZE | cut -d'x' -f2)
            shift 2
            ;;
        --video-size)
            VIDEO_SIZE="$2"
            VIDEO_WIDTH=$(echo $VIDEO_SIZE | cut -d'x' -f1)
            VIDEO_HEIGHT=$(echo $VIDEO_SIZE | cut -d'x' -f2)
            shift 2
            ;;
        -r|--framerate)
            FRAMERATE="$2"
            shift 2
            ;;
        -b|--bitrate)
            BITRATE="$2"
            shift 2
            ;;
        --speed)
            SPEED_PRESET="$2"
            shift 2
            ;;
        -h|--help)
            show_help
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            ;;
    esac
done

# Check for required arguments
if [ -z "$INPUT_FILE" ] || [ -z "$OUTPUT_FILE" ]; then
    echo "Error: Input and output files are required"
    show_help
fi

# If running in Docker, use the environment
if [ -z "$INSIDE_DOCKER" ]; then
    # Display conversion parameters
    echo "Starting Xvfb..."
    Xvfb :99 -screen 0 ${VIDEO_WIDTH}x${VIDEO_HEIGHT}x24 &
    export DISPLAY=:99
    export INSIDE_DOCKER=1

    echo "Converting $INPUT_FILE to $OUTPUT_FILE"
    echo "Preset path: $PRESET_PATH"
    echo "Preset duration: $PRESET_DURATION seconds"
    echo "Mesh size: ${MESH_X}x${MESH_Y}"
    echo "Video size: ${VIDEO_WIDTH}x${VIDEO_HEIGHT}"
    echo "Framerate: $FRAMERATE fps"
    echo "Bitrate: $BITRATE kbps"
    echo "Encoding speed: $SPEED_PRESET"
fi

# Wait a moment for Xvfb to start
sleep 1

# Run the actual conversion
gst-launch-1.0 -e \
  filesrc location=$INPUT_FILE ! \
    decodebin ! tee name=t \
      t. ! queue ! audioconvert ! audioresample ! \
            capsfilter caps="audio/x-raw, format=F32LE, channels=2, rate=44100" ! \
            avenc_aac bitrate=320000 ! queue ! mux. \
      t. ! queue ! audioconvert ! projectm \
            preset=$PRESET_PATH \
            texture-dir=$TEXTURE_DIR \
            preset-duration=$PRESET_DURATION \
            mesh-size=${MESH_X},${MESH_Y} ! \
            identity sync=false ! videoconvert ! videorate ! \
            video/x-raw,framerate=$FRAMERATE/1,width=$VIDEO_WIDTH,height=$VIDEO_HEIGHT ! \
            x264enc bitrate=$(($BITRATE * 1000)) key-int-max=200 speed-preset=$SPEED_PRESET ! \
            video/x-h264,stream-format=avc,alignment=au ! queue ! mux. \
    mp4mux name=mux ! filesink location=$OUTPUT_FILE &

GST_PID=$!

# Wait for the conversion to finish or for signals
echo "Conversion running. Press Ctrl+C to stop."
wait $GST_PID || true

echo "Conversion complete! Output saved to $OUTPUT_FILE"
