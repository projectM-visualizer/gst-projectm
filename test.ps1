# Check if gst-inspect-1.0 is installed
if (!(Get-Command gst-inspect-1.0 -ErrorAction SilentlyContinue)) {
    Write-Host "gst-inspect-1.0 is not installed. Please install it and try again."
    exit 1
}

# Check if gst-launch-1.0 is installed
if (!(Get-Command gst-launch-1.0 -ErrorAction SilentlyContinue)) {
    Write-Host "gst-launch-1.0 is not installed. Please install it and try again."
    exit 1
}

# Check if plugin is installed
if (!(Test-Path "$Env:GST_PLUGIN_PATH\gstprojectm.dll")) {
    Write-Host "gstprojectm.dll is missing. Please install it and try again."
    exit 1
}

Write-Host

switch ($args) {
    "--details" {
        & gst-inspect-1.0 --plugin projectm
        break
    }

    "--inspect" {
        & gst-inspect-1.0 projectm
        break
    }

    "--audio" {
        $env:GST_DEBUG = "3"
        & gst-launch-1.0 -v `
            audiotestsrc ! queue ! audioconvert ! `
            projectm `
            ! "video/x-raw,width=512,height=512,framerate=60/1" ! videoconvert ! xvimagesink sync=false
        break
    }

    "--preset" {
        $env:GST_DEBUG = "4"
        & gst-launch-1.0 -v `
            audiotestsrc ! queue ! audioconvert ! `
            projectm preset="test/presets/215-wave.milk" `
            ! "video/x-raw,width=512,height=512,framerate=60/1" ! videoconvert ! xvimagesink sync=false
        break
    }

    "--properties" {
        $env:GST_DEBUG = "3"
        & gst-launch-1.0 -v `
            audiotestsrc ! queue ! audioconvert ! `
            projectm `
            preset="test/presets/215-wave.milk" `
            texture-dir="test/textures" `
            beat-sensitivity=0.5 `
            hard-cut-duration=1 `
            hard-cut-enabled=true `
            hard-cut-sensitivity=0.5 `
            soft-cut-duration=1 `
            preset-duration=30 `
            mesh-size="512,512" `
            easter-egg=0.75 `
            preset-locked=false `
            ! "video/x-raw,width=512,height=512,framerate=30/1" ! videoconvert ! xvimagesink sync=false
        break
    }

    "--output-video" {
        $env:GST_DEBUG = "3"
        & gst-launch-1.0 -v `
            filesrc location="test/audio/upbeat-future-bass.mp3" ! decodebin ! audioconvert ! `
            projectm ! videoscale ! videoconvert ! video/x-raw,width=1280,height=720 ! `
            x264enc ! mp4mux ! filesink location="test/output/test_video.mp4"
        break
    }

    "--encode-output-video" {
        $env:GST_DEBUG = "3"
        & gst-launch-1.0 -v `
            filesrc location="test/audio/upbeat-future-bass.mp3" ! decodebin name=dec ! `
            audioconvert ! avenc_aac ! avmux_mp4 ! filesink location="test/output/video2.mp4" `
            dec. ! `
            projectm ! videoconvert ! x264enc ! avenc_mp4 ! avmux_mp4.video_0
        break
    }

    default {
        Write-Host "Usage: $scriptName [--details|--inspect|--audio|--preset|--properties|--output-video|--encode-output-video]"
        exit 1
    }
}