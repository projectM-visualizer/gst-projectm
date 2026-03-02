<a id="readme-top"></a>

<div align="center">

[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![LGPL-2.1][license-shield]][license-url]

<br />

<h3 align="center">gst-projectm</h3>

  <p align="center">
    GStreamer plugin utilizing the <a href="https://github.com/projectM-visualizer/projectm" target="_blank">projectM</a> library.
    <br />
    <br />
    <a href="https://github.com/projectM-visualizer/gst-projectm/issues" target="_blank">Report Bug</a>
    ·
    <a href="https://github.com/projectM-visualizer/gst-projectm/issues" target="_blank">Request Feature</a>
  </p>
</div>

<br />

<details>
  <summary>Table of Contents</summary>
  <ol>
    <li><a href="#getting-started">Getting Started</a></li>
    <li><a href="#plugin-properties">Plugin Properties</a></li>
    <li><a href="#presets-and-textures">Presets and Textures</a></li>
    <li><a href="#release-packages">Release Packages</a></li>
    <li><a href="#usage-examples">Usage Examples</a></li>
    <li><a href="#easy-audio-to-video-conversion">Easy Audio to Video Conversion (Docker)</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#support">Support</a></li>
    <li><a href="#contact">Contact</a></li>
  </ol>
</details>

<br />

## [Demo Videos (4K)](https://www.youtube.com/watch?v=fI3BMiVDQgU&list=PLFLkbObX4o6TK1jGL6pm1wMwvq2FXnpYJ&index=7)

## Getting Started

**gst-projectm** is a [GStreamer](https://gstreamer.freedesktop.org/) plugin that takes an audio stream as input and produces a video stream of real-time [projectM](https://github.com/projectM-visualizer/projectm) visualizations. It is not a standalone application — it is a plugin element (`projectm`) that runs inside any GStreamer pipeline, which means it works from the command line with `gst-launch-1.0`, inside applications that use GStreamer, in Docker containers, or anywhere GStreamer runs.

There are several ways to get the plugin, depending on what you need:

| Option | What it does | Best for | Guide |
| --- | --- | --- | --- |
| **Install a package** | Adds the plugin to your existing GStreamer installation. Use with `gst-launch-1.0` or any GStreamer app. | Most users | **[Linux](docs/LINUX.md)** · **[macOS](docs/OSX.md)** · **[Windows](docs/WINDOWS.md)** |
| **Rebuild from source packages** | Recompile from `.dsc` or `.src.rpm` for your exact distro and architecture, without cloning the repo. | Distro packagers, non-x86 architectures | **[Linux](docs/LINUX.md#building-from-source-packages)** |
| **Build from source** | Clone the repo and build. Full control over dependencies and build options. | Developers, custom projectM builds | **[Linux](docs/LINUX.md#building-from-source)** · **[macOS](docs/OSX.md#building-from-source)** · **[Windows](docs/WINDOWS.md#building-from-source)** |
| **Docker container** | One-command audio-to-video conversion. No setup required. | Quick audio-to-video rendering | [Docker conversion](#easy-audio-to-video-conversion) |
| **Flatpak / AppImage** | Self-contained bundles that ship their own GStreamer and run `gst-launch-1.0` directly. No system GStreamer needed. | Quick testing without installing anything | **[Linux — Portable Bundles](docs/LINUX.md#portable-bundles-flatpak--appimage)** |

Packages that install the plugin (DEB, RPM, ZIP, tar.gz) place a single shared library (`libgstprojectm.so` / `.dll`) into your GStreamer plugin path. After that, the `projectm` element is available in every pipeline on the system. The Flatpak and AppImage bundles are different — they are self-contained executables that include their own copy of GStreamer and are intended for quick testing, not as a way to add the plugin to your system.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Plugin Properties

The `projectm` element accepts these properties, set in a pipeline with `property=value` syntax:

| Property | Type | Default | Description |
| --- | --- | --- | --- |
| `preset` | string | *(none)* | Path to a directory of MilkDrop preset files (`.milk`). |
| `texture-dir` | string | *(none)* | Path to a directory of texture images used by presets. |
| `preset-duration` | double | `0` (indefinite) | Seconds before auto-switching to the next preset. `0` plays indefinitely. |
| `soft-cut-duration` | double | `3.0` | Duration in seconds of smooth crossfade transitions. |
| `hard-cut-duration` | double | `3.0` | Minimum seconds between hard (abrupt) cuts. |
| `hard-cut-enabled` | boolean | `false` | Allow hard cuts triggered by beats in the audio. |
| `hard-cut-sensitivity` | float | `1.0` | Sensitivity for hard cut triggers (0.0–1.0). |
| `beat-sensitivity` | float | `1.0` | How strongly the visualizer responds to beats (0.0–5.0). |
| `mesh-size` | string | `48,32` | Visualization mesh resolution as `width,height`. Higher = smoother but slower. |
| `aspect-correction` | boolean | `true` | Adjust rendering for non-square aspect ratios. |
| `easter-egg` | float | `0.0` | Probability of triggering the easter egg (0.0–1.0). |
| `preset-locked` | boolean | `false` | Lock on the current preset, disabling automatic switching. |
| `enable-playlist` | boolean | `true` | Enable playlist-based preset switching. |
| `shuffle-presets` | boolean | `true` | Randomize preset order. Requires `enable-playlist=true`. |

Output video resolution and framerate are set through GStreamer caps, not plugin properties:

```
... ! projectm ! "video/x-raw,width=1920,height=1080,framerate=60/1" ! ...
```

Run `gst-inspect-1.0 projectm` for the full listing from your installed version.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Presets and Textures

The plugin requires [MilkDrop](https://en.wikipedia.org/wiki/MilkDrop)-compatible preset files to produce visualizations. Presets may also reference textures. Neither presets nor textures are bundled with any release package — download them separately:

| Resource | Repository |
| --- | --- |
| **Presets** (curated) | [projectM-visualizer/presets-cream-of-the-crop](https://github.com/projectM-visualizer/presets-cream-of-the-crop) |
| **Textures** | [projectM-visualizer/presets-milkdrop-texture-pack](https://github.com/projectM-visualizer/presets-milkdrop-texture-pack) |

```bash
git clone https://github.com/projectM-visualizer/presets-cream-of-the-crop.git ~/projectM-presets
git clone https://github.com/projectM-visualizer/presets-milkdrop-texture-pack.git ~/projectM-textures
```

Then pass the paths to the plugin:

```bash
gst-launch-1.0 audiotestsrc ! queue ! audioconvert \
  ! projectm preset=~/projectM-presets texture-dir=~/projectM-textures preset-duration=10 \
  ! "video/x-raw,width=1920,height=1080,framerate=60/1" \
  ! videoconvert ! autovideosink sync=false
```

You can also use any directory of `.milk` preset files, including your own.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Release Packages

Pre-built packages are available on the [GitHub Releases](https://github.com/projectM-visualizer/gst-projectm/releases) page.

### Package Variants

| Variant | Description |
| --- | --- |
| **static-gl** | projectM statically linked with desktop OpenGL. Self-contained — no external projectM needed. Recommended for most desktop systems. |
| **static-gles** | projectM statically linked with OpenGL ES. For embedded or GLES-only environments. |
| **dynamic** | projectM **not** included. Requires [projectM](https://github.com/projectM-visualizer/projectm) (>= 4.1.0) installed separately. |

Only one variant can be installed at a time — they conflict with each other.

### Platform Packages

| Format | Platform | Guide |
| --- | --- | --- |
| DEB | Ubuntu, Debian | **[Linux](docs/LINUX.md)** |
| RPM | Fedora, RHEL | **[Linux](docs/LINUX.md)** |
| DEB Source / SRPM | Any DEB/RPM-based distro | **[Linux](docs/LINUX.md#building-from-source-packages)** |
| ZIP | Windows (x64) | **[Windows](docs/WINDOWS.md)** |
| tar.gz | macOS (arm64) | **[macOS](docs/OSX.md)** |
| Flatpak / AppImage | Linux (x86_64) | **[Linux — Portable Bundles](docs/LINUX.md#portable-bundles-flatpak--appimage)** |

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Usage Examples

These examples assume the plugin is installed and presets/textures have been downloaded (see above).

### Live Audio Visualization

```bash
# Visualize a test tone
gst-launch-1.0 audiotestsrc ! queue ! audioconvert \
  ! projectm preset=~/projectM-presets texture-dir=~/projectM-textures preset-duration=5 \
  ! "video/x-raw,width=1920,height=1080,framerate=60/1" \
  ! videoconvert ! autovideosink sync=false

# Visualize PipeWire/PulseAudio system audio
gst-launch-1.0 pipewiresrc ! queue ! audioconvert \
  ! projectm preset=~/projectM-presets preset-duration=5 \
  ! "video/x-raw,width=2048,height=1440,framerate=60/1" \
  ! videoconvert ! autovideosink sync=false
```

### Audio to Video Conversion

```bash
gst-launch-1.0 -e \
  filesrc location=input.mp3 ! decodebin ! tee name=t \
    t. ! queue ! audioconvert ! audioresample \
      ! capsfilter caps="audio/x-raw,format=F32LE,channels=2,rate=44100" \
      ! avenc_aac bitrate=320000 ! queue ! mux. \
    t. ! queue ! audioconvert \
      ! projectm preset=~/projectM-presets texture-dir=~/projectM-textures \
          preset-duration=6 mesh-size=1024,576 \
      ! identity sync=false ! videoconvert ! videorate \
      ! "video/x-raw,framerate=60/1,width=3840,height=2160" \
      ! x264enc bitrate=50000 key-int-max=200 speed-preset=veryslow \
      ! "video/x-h264,stream-format=avc,alignment=au" ! queue ! mux. \
  mp4mux name=mux ! filesink location=output.mp4
```

Some elements (x264enc, avenc_aac) require additional GStreamer plugin packages on your system (e.g. `gstreamer1.0-plugins-ugly`, `gstreamer1.0-libav`).

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Easy Audio to Video Conversion

A Docker-based conversion tool is included for one-command audio-to-video rendering with no manual setup.

### Prerequisites

- [Docker](https://docs.docker.com/get-docker/)
- GPU acceleration: [NVIDIA Container Toolkit](https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/install-guide.html) (NVIDIA) or no extra setup (AMD/Intel)

### Quick Start

```bash
git clone https://github.com/projectM-visualizer/gst-projectm.git
cd gst-projectm
./projectm-convert -i your-audio-file.mp3 -o output-video.mp4
```

The first run builds the container automatically (takes a while). Subsequent runs use the cached image. Conversion time depends on audio length and settings.

### Conversion Options

| Option                | Description                                        | Default         |
| --------------------- | -------------------------------------------------- | --------------- |
| `-d, --duration SEC`  | Seconds between preset transitions                 | 6               |
| `--mesh WxH`          | Mesh size for visualization                        | 1024x576        |
| `--video-size WxH`    | Output video resolution                            | 1920x1080       |
| `-r, --framerate FPS` | Output frame rate                                  | 60              |
| `-b, --bitrate KBPS`  | Video bitrate in kbps                              | 8000            |
| `--speed PRESET`      | x264 speed preset (ultrafast to veryslow)          | medium          |
| `-p, --preset DIR`    | Path to custom presets directory                   | Default presets |

```bash
# 4K high quality
./projectm-convert -i song.mp3 -o viz-4k.mp4 --video-size 3840x2160 -b 16000 --speed veryslow

# Quick test
./projectm-convert -i song.mp3 -o viz-test.mp4 --speed ultrafast --video-size 1280x720
```

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Licensing

This project distinguishes between the **plugin source code** and the **distribution bundle**:

*   **The Plugin:** The source code for `gst-projectm` is licensed under the **GNU Lesser General Public License (LGPL) v2.1 or later**. This allows for broad compatibility with both open-source and proprietary applications. See [LICENSE](LICENSE).
*   **The AppImage/Flatpak:** The binary distribution bundle is licensed under the **GNU General Public License (GPL) v2.0 or later**.

> **Note:** This "upgrade" to GPL applies only to the bundle itself. It is required because the bundle includes GStreamer's "Ugly" plugin set, which contains GPL-licensed dependencies (such as x264). See [BUNDLE-LICENSE](BUNDLE-LICENSE).

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Support

[![Discord][discord-shield]][discord-url]

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Contact

Blaquewithaq (Discord: SoFloppy#1289) - [@anomievision](https://twitter.com/anomievision) - anomievision@gmail.com

Mischa (Discord: mish) - [@revmischa](https://github.com/revmischa)

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!----------------------------------------------------------------------->
<!-- MARKDOWN LINKS & IMAGES -->

[contributors-shield]: https://img.shields.io/github/contributors/projectM-visualizer/gst-projectm.svg?style=for-the-badge
[contributors-url]: https://github.com/projectM-visualizer/gst-projectm/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/projectM-visualizer/gst-projectm.svg?style=for-the-badge
[forks-url]: https://github.com/projectM-visualizer/gst-projectm/network/members
[stars-shield]: https://img.shields.io/github/stars/projectM-visualizer/gst-projectm.svg?style=for-the-badge
[stars-url]: https://github.com/projectM-visualizer/gst-projectm/stargazers
[issues-shield]: https://img.shields.io/github/issues/projectM-visualizer/gst-projectm.svg?style=for-the-badge
[issues-url]: https://github.com/projectM-visualizer/gst-projectm/issues
[license-shield]: https://img.shields.io/github/license/projectM-visualizer/gst-projectm.svg?style=for-the-badge
[license-url]: https://github.com/projectM-visualizer/gst-projectm/blob/master/LICENSE
[crates-shield]: https://img.shields.io/crates/v/gst-projectm?style=for-the-badge
[crates-url]: https://crates.io/crates/gst-projectm
[crates-dl-shield]: https://img.shields.io/crates/d/gst-projectm?style=for-the-badge
[crates-dl-url]: https://crates.io/crates/gst-projectm
[discord-shield]: https://img.shields.io/discord/737206408482914387?style=for-the-badge
[discord-url]: https://discord.gg/7fQXN43n9W
