# OVERVIEW
  
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

## Contributors

- [Discord: tristancmoi](https://github.com/hashFactory)
- [Discord: CodAv](https://github.com/kblaschke)