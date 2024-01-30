# BOUNTY

### [[$500 BOUNTY] Implement a cross-platform GStreamer plug-in for libprojectM](https://github.com/projectM-visualizer/projectm/issues/668)

## Summary

Implement a cross-platform GStreamer plug-in for libprojectM
  - Ensure cross-platform compatibility with GStreamer and libprojectM. (Linux, OSX, Windows)
  - Implement the plug-in using the new C API in libprojectM 4.0 and link to the shared library.
  - Accept an audio/x-raw stream and forward it to projectM.
  - Generate a video/x-raw stream as the plug-in's output.
  
## Details

- Automated build system (workflow/scripts)
  - [x] Linux
  - [x] OSX
  - [x] Windows
- Compiling from source
  - [x] Linux
  - [x] OSX
  - [x] Windows
- Working with GStreamer
  - [x] Linux
  - [x] OSX
  - [ ] Windows (see issues)
- [x] Accepting an audio/x-raw stream (coded to add more formats later, if needed)
- [x] Generating a video/x-raw stream (coded to add more formats later, if needed)
- [x] Utilizing the new C API in libprojectM 4.0
- [x] Implemented properties with defaults (aka settings)


#### Testing

- Tested: Playing an audio stream and outputting a video stream
  - [x] Linux
  - [ ] OSX
  - [ ] Windows (see issues)
- Tested: Turning an audio file into a video file
  - [x] Linux
  - [ ] OSX (see issues)
  - [ ] Windows (see issues)

## Issues

- **ALL** - GL issue. `gst_gl_context_activate(context, true) fails`
- **Windows**:  ***BROKEN*** - After days of testing, I've yet to find a solution to GStreamers linking problem when using plugins.
  - Wont run with inspect/launch. DLL dependency issue

## Contributions

- [Discord: tristancmoi](https://github.com/hashFactory) [**AWARD: 20%**]
  - Provided a semi-working example of a GStreamer plugin utilizing the ProjectM library, on Linux
- [Discord: CodAv](https://github.com/kblaschke) [**AWARD: 20%**]
  - Provided knowledge of C#, and GL
  - Traced OSX issues
  - Traced GL issues