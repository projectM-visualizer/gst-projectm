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
    GStreamer plugin utilizing the <a href="https://github.com/projectM-visualizer/projectm" target="_blank">ProjectM</a> library.
    <br />
    <br />
    <a href="https://github.com/projectM-visualizer/gst-projectm/issues" target="_blank">Report Bug</a>
    ·
    <a href="https://github.com/projectM-visualizer/gst-projectm/issues" target="_blank">Request Feature</a>
  </p>
</div>

<br />

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li><a href="#getting-started">Getting Started</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#support">Support</a></li>
    <li><a href="#contact">Contact</a></li>
  </ol>
</details>

<br />

<!-- GETTING STARTED -->

## Getting Started

The documentation has been organized into distinct files, each dedicated to a specific platform. Within each file, you'll find detailed instructions covering the setup of prerequisites, the building process, installation steps, and guidance on utilizing the plugin on the respective platform.

- **[Linux](docs/LINUX.md)**
- **[OSX](docs/OSX.md)**
- **[Windows](docs/WINDOWS.md)**

Once the plugin has been installed, you can use it something like this:

```shell
gst-launch pipewiresrc ! queue ! audioconvert ! projectm preset=/usr/local/share/projectM/presets preset-duration=5 ! video/x-raw,width=2048,height=1440,framerate=60/1 ! videoconvert ! xvimagesink sync=false
```

Or to convert an audio file to video:

```shell
gst-launch-1.0 -e \
  filesrc location=input.mp3  ! \
    decodebin ! tee name=t \
      t. ! queue ! audioconvert ! audioresample ! \
            capsfilter caps="audio/x-raw, format=F32LE, channels=2, rate=44100" ! avenc_aac bitrate=320000 ! queue ! mux. \
      t. ! queue ! audioconvert ! projectm preset=/usr/local/share/projectM/presets texture-dir=/usr/local/share/projectM/textures preset-duration=6 mesh-size=1024,576 ! \
            identity sync=false ! videoconvert ! videorate ! video/x-raw,framerate=60/1,width=3840,height=2160 ! \
            x264enc bitrate=50000 key-int-max=200 speed-preset=veryslow ! video/x-h264,stream-format=avc,alignment=au ! queue ! mux. \
    mp4mux name=mux ! filesink location=output.mp4
```

Available options

```shell
gst-inspect projectm
```

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- CONTRIBUTING -->

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

<!-- LICENSE -->

## License

Distributed under the LGPL-2.1 license. See `LICENSE` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- SUPPORT -->

## Support

[![Discord][discord-shield]][discord-url]

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- CONTACT -->

## Contact

Blaquewithaq (Discord: SoFloppy#1289) - [@anomievision](https://twitter.com/anomievision) - anomievision@gmail.com

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!----------------------------------------------------------------------->
<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->

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
