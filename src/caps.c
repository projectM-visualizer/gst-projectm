
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gst/audio/audio-format.h>
#include <gst/video/video-format.h>

#include "caps.h"
#include "plugin.h"

GST_DEBUG_CATEGORY_STATIC(gst_projectm_caps_debug);
#define GST_CAT_DEFAULT gst_projectm_caps_debug

const gchar *get_audio_sink_cap(unsigned int type)
{
  const char *format;

  switch (type)
  {
  case 0:
    format = GST_AUDIO_CAPS_MAKE("audio/x-raw, "
                                 "format = (string) " GST_AUDIO_NE(
                                     S16) ", "
                                          "layout = (string) interleaved, "
                                          "channels = (int) { 2 }, "
                                          "rate = (int) { 44100 }, "
                                          "channel-mask = (bitmask) { 0x0003 }");
    break;
  default:
    format = NULL;
    break;
  }

  return format;
}

const gchar *get_video_src_cap(unsigned int type)
{
  const char *format;

  switch (type)
  {
  case 0:
    format = GST_VIDEO_CAPS_MAKE("video/x-raw, format = (string) { ABGR }, framerate=(fraction)[0/1,MAX]");
    break;
  default:
    format = NULL;
    break;
  }

  return format;
}