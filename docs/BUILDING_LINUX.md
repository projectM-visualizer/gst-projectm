## Building on Linux

### Prerequisites

* [Git](https://git-scm.com/download/win)
* [Ninja](https://github.com/ninja-build/ninja)
* [CMake](https://cmake.org/download/) (3.8 or higher)
* [GStreamer](https://gstreamer.freedesktop.org/download/) (1.16 or higher)
* [ProjectM](https://github.com/projectM-visualizer/projectm) (4.0 or higher)

### Building

1. Clone the repository

```bash
git clone https://github.com/anomievision/gst-plugin-projectm.git
``` 

2. Install dependencies

```bash
# apt
sudo apt update
sudo apt install -y ninja-build mesa-common-dev build-essential git cmake pkg-config libgstreamer1.0-dev libgstreamer-gl1.0-0 libgstreamer-plugins-base1.0-dev

# pacman
sudo pacman -Syu
sudo pacman -S base-devel ninja cmake mesa gst-plugins-base-libs
```

3. Set Environment Variables

- PROJECTM_ROOT - Path to built projectM directory

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