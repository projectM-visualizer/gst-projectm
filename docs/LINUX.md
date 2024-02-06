## Building on Linux

### Prerequisites

* [Git](https://git-scm.com/download/win)
* [Ninja](https://github.com/ninja-build/ninja)
* [CMake](https://cmake.org/download/) (3.8 or higher)
* [GStreamer](https://gstreamer.freedesktop.org/download/) (1.16 or higher)
* [ProjectM](https://github.com/projectM-visualizer/projectm) (4.0 or higher)

### Building

1. Download (or clone, if git installed) the repository

```bash
git clone https://github.com/projectM-visualizer/gst-projectm.git
``` 

2. Setup

```bash
./setup.sh
# OR
./setup.sh --auto # Skips prompts by using default options
```

1. Set Environment Variables

- PROJECTM_ROOT - Path to built projectM directory
  - Defaults to "/usr/local" if ProjectM found

```bash
echo "export PROJECTM_ROOT=YOUR_PATH_HERE" >> ~/.bashrc
echo "" >> ~/.bashrc
source ~/.bashrc
# OR
echo "export PROJECTM_ROOT=YOUR_PATH_HERE" >> ~/.zshrc
echo "" >> ~/.zshrc
source ~/.zshrc
```

4. Run build script

```bash
.\build.sh

.\build.sh --auto # Skips prompts by using default options
```

### Installing

#### Automatic

The build script will ask if you want to install the plugin.

#### Manual

1. Create GStreamer plugins directory.

```bash
mkdir -p $HOME/.local/share/gstreamer-1.0/plugins/
```

2. Copy the built plugin to the plugins directory.

```bash
mv "dist/libgstprojectm.so" "$HOME/.local/share/gstreamer-1.0/plugins/libgstprojectm.so"
```

3. Set GST_PLUGIN_PATH environment variable to the plugins directory.

```bash
echo "export GST_PLUGIN_PATH=$HOME/.local/share/gstreamer-1.0/plugins/" >> ~/.bashrc
echo "" >> ~/.bashrc
source ~/.bashrc
# OR
echo "export GST_PLUGIN_PATH=$HOME/.local/share/gstreamer-1.0/plugins/" >> ~/.zshrc
echo "" >> ~/.zshrc
source ~/.zshrc
# OR
echo "export GST_PLUGIN_PATH=$HOME/.local/share/gstreamer-1.0/plugins/" >> ~/.zprofile
echo "" >> ~/.zprofile
source ~/.zprofile
# OR
echo "export GST_PLUGIN_PATH=$HOME/.local/share/gstreamer-1.0/plugins/" >> ~/.bash_profile
echo "" >> ~/.bash_profile
source ~/.bash_profile
```

### Using

To utilize the plugin with the example, please install GStreamer

```bash
gst-launch-1.0 audiotestsrc ! queue ! audioconvert ! projectm ! "video/x-raw,width=512,height=512,framerate=60/1" ! videoconvert ! xvimagesink sync=false
```

### Testing

```bash
./test.sh --inspect # Inspect the plugin
./test.sh --audio # Test the plugin with audio
./test.sh --preset # Test the plugin with a preset
./test.sh --properties # Test the plugin with properties
./test.sh --output-video # Test the plugin with video output (video only)
./test.sh --encode-output-video # Test the plugin with encoded video output (audio/video)
```