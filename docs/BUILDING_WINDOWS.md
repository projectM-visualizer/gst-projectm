## Building on Windows

### Prerequisites

* [Git](https://git-scm.com/download/win)
* [Visual Studio 2017](https://www.visualstudio.com/downloads/) (Community edition is fine)
* [Vcpkg](https://github.com/Microsoft/vcpkg)
* [CMake](https://cmake.org/download/) (3.8 or higher)
* [GStreamer](https://gstreamer.freedesktop.org/download/) (1.16 or higher) **[NOT NEEDED TO BUILD]**

### Building

1. Clone the repository

```
git clone https://github.com/anomievision/gst-plugin-projectm.git
``` 

2. Set Environment Variables

- PROJECTM - Path to built projectM directory
- VCPKG - Path to vcpkg installtion directory

```powershell
[Environment]::SetEnvironmentVariable("PROJECTM", YOUR_PATH_HERE, "User")
[Environment]::SetEnvironmentVariable("VCPKG", YOUR_PATH_HERE, "User")
```

3. Run build script

```powershell
.\build.ps1
```

### Installing

#### Automatic
The build script will ask if you want to install the plugin.

#### Manual
1. Copy the built plugin to the GStreamer plugins directory or create your own.

```powershell
New-Item -Path "$Env:USERPROFILE\.gstreamer\1.0\plugins" -ItemType Directory | Out-Null
```

2. Copy the built plugin to the plugins directory.

```powershell
Copy-Item -Path "dist\gstprojectm.dll" -Destination "$Env:USERPROFILE\.gstreamer\1.0\plugins\gstprojectm.dll" -Force
```

3. Set GST_PLUGIN_PATH environment variable to the plugins directory.

```powershell
[Environment]::SetEnvironmentVariable("GST_PLUGIN_PATH", "$Env:USERPROFILE\.gstreamer\1.0\plugins", "User")
```

### Using

To utilize the plugin with the example, please install GStreamer

```
gst-launch-1.0 audiotestsrc ! queue ! audioconvert ! projectm ! "video/x-raw,width=512,height=512,framerate=60/1" ! videoconvert ! xvimagesink sync=false
```