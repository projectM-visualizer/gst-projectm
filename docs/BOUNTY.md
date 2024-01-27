# BOUNTY

### [[$500 BOUNTY] Implement a cross-platform GStreamer plug-in for libprojectM](https://github.com/projectM-visualizer/projectm/issues/668)

## Summary

Implement a cross-platform GStreamer plug-in for libprojectM
  - Ensure cross-platform compatibility with GStreamer and libprojectM. (Linux, OSX, Windows)
  - Implement the plug-in using the new C API in libprojectM 4.0 and link to the shared library.
  - Accept an audio/x-raw stream and forward it to projectM.
  - Generate a video/x-raw stream as the plug-in's output.
  
## Details

#### GStreamer Plug-in
- [ ] Compiling from source
  - [x] Linux
  - [ ] OSX
  - [x] Windows
- [ ] Working with GStreamer
  - [x] Linux
  - [ ] OSX
  - [ ] Windows (after days of testing, I've yet to find a solution to GStreamers linking problem when using plugins)
- [x] Accepting an audio/x-raw stream (coded to add more formats later, if needed)
- [x] Generating a video/x-raw stream (coded to add more formats later, if needed)
- [x] Utilizing the new C API in libprojectM 4.0
- [x] Implemented properties with defaults (aka settings)

#### GStreamer Plug-in Testing

- [ ] Tested: Playing an audio stream and outputting a video stream
  - [x] Linux
  - [ ] OSX
  - [ ] Windows
- [ ] Tested: Turning an audio file into a video file
  - [x] Linux
  - [ ] OSX
  - [ ] Windows

## Issues

- **Windows**: Wont work! After days of testing, I've yet to find a solution to GStreamers linking problem when using plugins. I'm beginning to believe that GStreamer is broken on Windows.

## Contributions

- [Discord: tristancmoi](https://github.com/hashFactory)
  - Provided a semi-working example of a GStreamer plugin utilizing the ProjectM library
  - I built off this knowledge and thank you for your contribution
- [Discord: CodAv](https://github.com/kblaschke)
  - Helped with basic knowledge of C#
  - Tested and helped with OSX issues