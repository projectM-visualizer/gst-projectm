#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gst/audio/audio-format.h>
#include <gst/video/video-format.h>

#include "caps.h"
#include "plugin.h"

GST_DEBUG_CATEGORY_STATIC(gst_caps_debug);
#define GST_CAT_DEFAULT gst_caps_debug

const gchar *get_audio_sink_cap(unsigned int type)
{
  const char *format;

  switch (type)
  {
  case 0:
    format = "audio/x-raw, "
             "format = (string) " GST_AUDIO_NE(
                 S16) ", "
                      "layout = (string) interleaved, "
                      "channels = (int) { 2 }, "
                      "rate = (int) { 44100 }, "
                      "channel-mask = (bitmask) { 0x0003 }";
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
    format = "video/x-raw, "
             "format = (string) " GST_VIDEO_NE(
                 RGB) ", "
                      "framerate = (fraction) [ 0/1, MAX ]";
    break;
  default:
    format = NULL;
    break;
  }

  return format;
}

gboolean gst_caps_init(GstPluginProjectMClass *klass)
{
  GST_DEBUG_CATEGORY_INIT(gst_caps_debug, "caps", 0, "caps");

  gst_element_class_add_pad_template(
      GST_ELEMENT_CLASS(klass),
      gst_pad_template_new("sink", GST_PAD_SINK, GST_PAD_ALWAYS,
                           gst_caps_from_string(get_audio_sink_cap(0))));

  gst_element_class_add_pad_template(
      GST_ELEMENT_CLASS(klass),
      gst_pad_template_new("src", GST_PAD_SRC, GST_PAD_ALWAYS,
                           gst_caps_from_string(get_video_src_cap(0))));

  return TRUE;
}