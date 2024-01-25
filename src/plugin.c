#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/gl/gl.h>
#include <gst/gl/gstglfuncs.h>
#include <gst/pbutils/gstaudiovisualizer.h>

#include <projectM-4/projectM.h>

#include "plugin.h"
#include "enums.h"

GST_DEBUG_CATEGORY_STATIC(gst_projectm_debug);
#define GST_CAT_DEFAULT gst_projectm_debug

G_DEFINE_TYPE_WITH_CODE(GstProjectM, gst_projectm, GST_TYPE_AUDIO_VISUALIZER,
                        GST_DEBUG_CATEGORY_INIT(gst_projectm_debug,
                                                "projectM", 0,
                                                "projectM visualizer plugin with code"));

#if G_BYTE_ORDER == G_BIG_ENDIAN
#define RGB_ORDER_CAPS "xRGB, RGB"
#else
#define RGB_ORDER_CAPS "BGRx, BGR"
#endif

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
    format = GST_VIDEO_CAPS_MAKE("video/x-raw, format = (string) { RGBA, BGRA }, framerate=(fraction)[0/1,MAX]");
    break;
  default:
    format = NULL;
    break;
  }

  return format;
}

static void check_gl_error(GstGLContext *context, gpointer data)
{
  GLuint error = context->gl_vtable->GetError();
  if (error != GL_NONE)
    g_print("GL error 0x%x encountered during processing\n",
            error);
}

void gst_projectm_set_property(GObject *object, guint property_id,
                               const GValue *value, GParamSpec *pspec)
{
  GstProjectM *plugin = GST_PROJECTM(object);

  GST_DEBUG_OBJECT(plugin, "set_property");

  switch (property_id)
  {
  case PROP_LOCATION:
    plugin->preset = g_strdup(g_value_get_string(value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    break;
  }
}

void gst_projectm_get_property(GObject *object, guint property_id,
                               GValue *value, GParamSpec *pspec)
{
  GstProjectM *plugin = GST_PROJECTM(object);

  GST_DEBUG_OBJECT(plugin, "get_property");

  switch (property_id)
  {
  case PROP_LOCATION:
    g_value_set_string(value, g_strdup("REPLACE_ME"));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    break;
  }
}

static void gst_projectm_init(GstProjectM *plugin)
{
  // Here to prevent compile errors
}

static void gst_projectm_finalize(GObject *object)
{
  GstProjectM *plugin = GST_PROJECTM(object);

  if (plugin->framebuffer)
    free(plugin->framebuffer);
  if (plugin->handle)
    projectm_destroy(plugin->handle);

  G_OBJECT_CLASS(gst_projectm_parent_class)->finalize(object);
}

static void gst_projectm_class_init(GstProjectMClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass *)klass;
  GstElementClass *element_class = (GstElementClass *)klass;
  GstAudioVisualizerClass *scope_class = (GstAudioVisualizerClass *)klass;

  // Setup caps
  const gchar *audio_sink_caps = get_audio_sink_cap(0);
  const gchar *video_src_caps = get_video_src_cap(0);

  gst_element_class_add_pad_template(GST_ELEMENT_CLASS(klass),
                                     gst_pad_template_new("src", GST_PAD_SRC, GST_PAD_ALWAYS,
                                                          gst_caps_from_string(video_src_caps)));
  gst_element_class_add_pad_template(GST_ELEMENT_CLASS(klass),
                                     gst_pad_template_new("sink", GST_PAD_SINK, GST_PAD_ALWAYS,
                                                          gst_caps_from_string(audio_sink_caps)));

  gst_element_class_set_static_metadata(GST_ELEMENT_CLASS(klass),
                                        "ProjectM Visualizer", "Generic", "A plugin for visualizing music using ProjectM",
                                        "Tristan Charpentier <tristan_charpentier@hotmail.com>");

  gobject_class->set_property = gst_projectm_set_property;
  gobject_class->get_property = gst_projectm_get_property;

  g_object_class_install_property(gobject_class, PROP_LOCATION,
                                  g_param_spec_string("preset", "Preset file Location",
                                                      "Location of the MilkDrop preset", "/home/tristan/dev/gst/trido/presets/Supernova/Shimmer/EoS - starburst 05 phasing.milk",
                                                      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  gobject_class->finalize = gst_projectm_finalize;

  scope_class->setup = GST_DEBUG_FUNCPTR(projectm_setup);
  scope_class->render = GST_DEBUG_FUNCPTR(projectm_render);
}

static void projectm_init_instance(GstProjectM *plugin)
{
  GstGLContext *other = gst_gl_context_get_current();
  if (other)
    return;

  GstGLDisplay *display = gst_gl_display_new_with_type(GST_GL_DISPLAY_TYPE_ANY);
  GstGLContext *context = gst_gl_context_new(display);
  GError *error = NULL;

  GstGLWindow *window;

  gint major;
  gint minor;

  GstAudioVisualizer *scope = GST_AUDIO_VISUALIZER(plugin);

  size_t width = scope->vinfo.width;
  size_t height = scope->vinfo.height;
  GST_LOG_OBJECT(plugin, "fps: %d", scope->vinfo.fps_n);

  gst_gl_context_create(context, 0, &error);
  window = gst_gl_context_get_window(context);

  gst_gl_context_get_gl_version(context, &major, &minor);

  gst_gl_window_set_preferred_size(window, width, height);
  gst_gl_window_set_render_rectangle(window, 0, 0, width, height);
  gst_gl_window_resize(window, width, height);

  gst_gl_context_activate(context, true);
  GstGLFramebuffer *fbo = gst_gl_framebuffer_new(context);

  gst_gl_memory_init_once();

  gst_gl_context_set_window(context, window);

  guint curr_con = gst_gl_context_get_current_gl_context(GST_GL_PLATFORM_GLX);

  GST_DEBUG_OBJECT(plugin, "current context: %d\n", curr_con);

  plugin->display = display;
  plugin->window = window;
  plugin->context = context;

  projectm_handle projectMHandle = projectm_create();
  if (!projectMHandle)
  {
    GST_LOG("Could not create instance");
  }
  else
  {
    GST_LOG("Created instance!");
  }

  plugin->handle = projectMHandle;
}

static gboolean projectm_setup(GstAudioVisualizer *bscope)
{
  GstProjectM *projectm = GST_PROJECTM(bscope);
  gint depth;

  projectm_init_instance(projectm);

  if (projectm->preset)
    projectm_load_preset_file(projectm->handle, projectm->preset, false);

  /* FIXME: we need to know how many bits we actually have in memory */
  depth = bscope->vinfo.finfo->pixel_stride[0];
  if (bscope->vinfo.finfo->bits >= 8)
  {
    depth *= 8;
  }

  bscope->req_spf = (bscope->ainfo.channels * bscope->ainfo.rate * 2) / bscope->vinfo.fps_n;
  GST_LOG_OBJECT(projectm, "%s", bscope->ainfo.finfo->description);

  projectm_set_fps(projectm->handle, bscope->vinfo.fps_n);
  projectm_set_window_size(projectm->handle, GST_VIDEO_INFO_WIDTH(&bscope->vinfo), GST_VIDEO_INFO_HEIGHT(&bscope->vinfo));

  projectm->framebuffer = (uint8_t *)malloc(GST_VIDEO_INFO_WIDTH(&bscope->vinfo) * GST_VIDEO_INFO_HEIGHT(&bscope->vinfo) * 4);

  GST_DEBUG_OBJECT(projectm, "WxH: %dx%d, depth: %d, fps: %d/%d",
                   GST_VIDEO_INFO_WIDTH(&bscope->vinfo),
                   GST_VIDEO_INFO_HEIGHT(&bscope->vinfo), depth,
                   (bscope->vinfo.fps_n), (bscope->vinfo.fps_d));

  return TRUE;
  /* ERRORS */
}

static gboolean projectm_render(GstAudioVisualizer *bscope, GstBuffer *audio,
                                GstVideoFrame *video)
{
  GstProjectM *projectm = GST_PROJECTM(bscope);
  GstMapInfo amap;
  gint16 *adata;
  gint i, channels;
  gboolean res = TRUE;
  guint32 vrate;
  guint num_samples;

  // AUDIO
  channels = GST_AUDIO_INFO_CHANNELS(&bscope->ainfo);
  num_samples = amap.size / (GST_AUDIO_INFO_CHANNELS(&bscope->ainfo) * sizeof(gint16));

  GstMemory *mem = gst_buffer_get_all_memory(audio);
  GST_DEBUG_OBJECT(projectm, "mem size: %lu", mem->size);

  gst_buffer_map(audio, &amap, GST_MAP_READ);

  GST_DEBUG_OBJECT(projectm, "samples: %lu, offset: %lu, offset end: %lu, vrate: %d, fps: %d, req_spf: %d", amap.size / 8, audio->offset, audio->offset_end, bscope->ainfo.rate, bscope->vinfo.fps_n, bscope->req_spf);

  projectm_pcm_add_int16(projectm->handle, (gint16 *)amap.data, amap.size / 4, PROJECTM_STEREO);

  GST_DEBUG_OBJECT(projectm, "audio data: %d %d %d %d", ((gint16 *)amap.data)[100], ((gint16 *)amap.data)[101], ((gint16 *)amap.data)[102], ((gint16 *)amap.data)[103]);

  gst_video_frame_map(video, &video->info, video->buffer, GST_MAP_READWRITE);

  const GstGLFuncs *gl = projectm->context->gl_vtable;

  size_t width, height;

  projectm_get_window_size(projectm->handle, &width, &height);
  gl->Viewport(0, 0, width, height);

  projectm_opengl_render_frame(projectm->handle);
  check_gl_error(projectm->context, projectm);

  uint8_t *data = projectm->framebuffer;

  gl->ReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, data);
  GST_DEBUG_OBJECT(projectm, "%d %d %d %d", data[0], data[1], data[2], data[3]);

  check_gl_error(projectm->context, projectm);

  gst_gl_context_swap_buffers(projectm->context);

  uint8_t *vdata = ((uint8_t *)(video->data[0]));

  /*// RGBA
  for (int r = 0; r < width*height*4; r+=4) {
    vdata[r+3] = data[r];
    //vdata[r+1] = data[r+1];
    //vdata[r+2] = data[r+2];
    vdata[r] = data[r+3];
  }
  */
  // BGRA
  for (int r = 0; r < width * height * 4; r += 4)
  {
    vdata[r + 3] = data[r];
    vdata[r + 2] = data[r + 2];
    vdata[r + 1] = data[r + 1];
    vdata[r] = data[r + 3];
  }

  GST_DEBUG_OBJECT(projectm, "v2 %d %d\n", GST_VIDEO_FRAME_N_PLANES(video), ((uint8_t *)(GST_VIDEO_FRAME_PLANE_DATA(video, 0)))[0]);

  GST_DEBUG_OBJECT(projectm, "rendered one frame");
done:
  gst_buffer_unmap(audio, &amap);

  return res;
}

static gboolean plugin_init(GstPlugin *plugin)
{
  GST_DEBUG_CATEGORY_INIT(gst_projectm_debug, "projectm", 0,
                          "projectm audio visualisations");

  return gst_element_register(plugin, "projectm", GST_RANK_NONE, GST_TYPE_PROJECTM);
}

#ifndef PACKAGE
#define PACKAGE "GstProjectM"
#endif
#ifndef PACKAGE_NAME
#define PACKAGE_NAME "GstProjectM"
#endif
#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "0.0.1"
#endif
#ifndef PACKAGE_LICENSE
#define PACKAGE_LICENSE "LGPL"
#endif
#ifndef PACKAGE_ORIGIN
#define PACKAGE_ORIGIN "https://github.com/anomievision/gst-plugin-projectm"
#endif

GST_PLUGIN_DEFINE(GST_VERSION_MAJOR, GST_VERSION_MINOR, projectm,
                  "plugin to visualize audio using the ProjectM library", plugin_init,
                  PACKAGE_VERSION, PACKAGE_LICENSE, PACKAGE_NAME, PACKAGE_ORIGIN)