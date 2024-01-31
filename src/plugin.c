#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/gl/gl.h>
#include <gst/gl/gstglfuncs.h>
#include <gst/pbutils/gstaudiovisualizer.h>

#include <projectM-4/projectM.h>

#include "plugin.h"
#include "caps.h"
#include "config.h"
#include "debug.h"
#include "enums.h"
#include "gl.h"
#include "projectm.h"

GST_DEBUG_CATEGORY_STATIC(gst_projectm_debug);
#define GST_CAT_DEFAULT gst_projectm_debug

G_DEFINE_TYPE_WITH_CODE(GstProjectM, gst_projectm, GST_TYPE_AUDIO_VISUALIZER,
                        GST_DEBUG_CATEGORY_INIT(gst_projectm_debug,
                                                "gstprojectm", 0,
                                                "Plugin Root"));

void gst_projectm_set_property(GObject *object, guint property_id,
                               const GValue *value, GParamSpec *pspec)
{
  GstProjectM *plugin = GST_PROJECTM(object);

  const gchar *property_name = g_param_spec_get_name(pspec);
  GST_DEBUG_OBJECT(plugin, "set-property <%s>", property_name);

  switch (property_id)
  {
  case PROP_PRESET_PATH:
    plugin->preset_path = g_strdup(g_value_get_string(value));
    break;
  case PROP_TEXTURE_DIR_PATH:
    plugin->texture_dir_path = g_strdup(g_value_get_string(value));
    break;
  case PROP_BEAT_SENSITIVITY:
    plugin->beat_sensitivity = g_value_get_float(value);
    break;
  case PROP_HARD_CUT_DURATION:
    plugin->hard_cut_duration = g_value_get_double(value);
    break;
  case PROP_HARD_CUT_ENABLED:
    plugin->hard_cut_enabled = g_value_get_boolean(value);
    break;
  case PROP_HARD_CUT_SENSITIVITY:
    plugin->hard_cut_sensitivity = g_value_get_float(value);
    break;
  case PROP_SOFT_CUT_DURATION:
    plugin->soft_cut_duration = g_value_get_double(value);
    break;
  case PROP_PRESET_DURATION:
    plugin->preset_duration = g_value_get_double(value);
    break;
  case PROP_MESH_SIZE:
  {
    const gchar *meshSizeStr = g_value_get_string(value);
    gint width, height;

    gchar **parts = g_strsplit(meshSizeStr, ",", 2);

    if (parts && g_strv_length(parts) == 2)
    {
      width = atoi(parts[0]);
      height = atoi(parts[1]);

      plugin->mesh_width = width;
      plugin->mesh_height = height;

      g_strfreev(parts);
    }
  }
  break;
  case PROP_ASPECT_CORRECTION:
    plugin->aspect_correction = g_value_get_boolean(value);
    break;
  case PROP_EASTER_EGG:
    plugin->easter_egg = g_value_get_float(value);
    break;
  case PROP_PRESET_LOCKED:
    plugin->preset_locked = g_value_get_boolean(value);
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

  const gchar *property_name = g_param_spec_get_name(pspec);
  GST_DEBUG_OBJECT(plugin, "get-property <%s>", property_name);

  switch (property_id)
  {
  case PROP_PRESET_PATH:
    g_value_set_string(value, plugin->preset_path);
    break;
  case PROP_TEXTURE_DIR_PATH:
    g_value_set_string(value, plugin->texture_dir_path);
    break;
  case PROP_BEAT_SENSITIVITY:
    g_value_set_float(value, plugin->beat_sensitivity);
    break;
  case PROP_HARD_CUT_DURATION:
    g_value_set_double(value, plugin->hard_cut_duration);
    break;
  case PROP_HARD_CUT_ENABLED:
    g_value_set_boolean(value, plugin->hard_cut_enabled);
    break;
  case PROP_HARD_CUT_SENSITIVITY:
    g_value_set_float(value, plugin->hard_cut_sensitivity);
    break;
  case PROP_SOFT_CUT_DURATION:
    g_value_set_double(value, plugin->soft_cut_duration);
    break;
  case PROP_PRESET_DURATION:
    g_value_set_double(value, plugin->preset_duration);
    break;
  case PROP_MESH_SIZE:
  {
    gchar *meshSizeStr = g_strdup_printf("%d,%d", plugin->mesh_width, plugin->mesh_height);
    g_value_set_string(value, meshSizeStr);
    g_free(meshSizeStr);
    break;
  }
  case PROP_ASPECT_CORRECTION:
    g_value_set_boolean(value, plugin->aspect_correction);
    break;
  case PROP_EASTER_EGG:
    g_value_set_float(value, plugin->easter_egg);
    break;
  case PROP_PRESET_LOCKED:
    g_value_set_boolean(value, plugin->preset_locked);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    break;
  }
}

static void gst_projectm_init(GstProjectM *plugin)
{
  // Set default values for properties
  plugin->preset_path = DEFAULT_PRESET_PATH;
  plugin->texture_dir_path = DEFAULT_TEXTURE_DIR_PATH;
  plugin->beat_sensitivity = DEFAULT_BEAT_SENSITIVITY;
  plugin->hard_cut_duration = DEFAULT_HARD_CUT_DURATION;
  plugin->hard_cut_enabled = DEFAULT_HARD_CUT_ENABLED;
  plugin->hard_cut_sensitivity = DEFAULT_HARD_CUT_SENSITIVITY;
  plugin->soft_cut_duration = DEFAULT_SOFT_CUT_DURATION;
  plugin->preset_duration = DEFAULT_PRESET_DURATION;

  const gchar *meshSizeStr = DEFAULT_MESH_SIZE;
  gint width, height;

  gchar **parts = g_strsplit(meshSizeStr, ",", 2);

  if (parts && g_strv_length(parts) == 2)
  {
    width = atoi(parts[0]);
    height = atoi(parts[1]);

    plugin->mesh_width = width;
    plugin->mesh_height = height;

    g_strfreev(parts);
  }

  plugin->aspect_correction = DEFAULT_ASPECT_CORRECTION;
  plugin->easter_egg = DEFAULT_EASTER_EGG;
  plugin->preset_locked = DEFAULT_PRESET_LOCKED;
}

static void gst_projectm_finalize(GObject *object)
{
  GstProjectM *plugin = GST_PROJECTM(object);

  if (plugin->framebuffer)
  {
    GST_DEBUG_OBJECT(plugin, "Freeing framebuffer");
    free(plugin->framebuffer);
  }
  if (plugin->handle)
  {
    GST_DEBUG_OBJECT(plugin, "Destroying ProjectM instance");
    projectm_destroy(plugin->handle);
  }

  G_OBJECT_CLASS(gst_projectm_parent_class)->finalize(object);
}

static void gst_projectm_class_init(GstProjectMClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass *)klass;
  GstElementClass *element_class = (GstElementClass *)klass;
  GstAudioVisualizerClass *scope_class = (GstAudioVisualizerClass *)klass;

  // Setup audio and video caps
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
                                        "AnomieVision <anomievision@gmail.com> | Tristan Charpentier <tristan_charpentier@hotmail.com>");

  // Setup properties
  gobject_class->set_property = gst_projectm_set_property;
  gobject_class->get_property = gst_projectm_get_property;

  g_object_class_install_property(gobject_class, PROP_PRESET_PATH,
                                  g_param_spec_string("preset", "Preset",
                                                      "Specifies the path to the preset file. The preset file determines the visual style and behavior of the audio visualizer.",
                                                      DEFAULT_PRESET_PATH,
                                                      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property(gobject_class, PROP_TEXTURE_DIR_PATH,
                                  g_param_spec_string("texture-dir", "Texture Directory",
                                                      "Sets the path to the directory containing textures used in the visualizer.",
                                                      DEFAULT_TEXTURE_DIR_PATH,
                                                      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property(gobject_class, PROP_BEAT_SENSITIVITY,
                                  g_param_spec_float("beat-sensitivity", "Beat Sensitivity",
                                                     "Controls the sensitivity to audio beats. Higher values make the visualizer respond more strongly to beats.",
                                                     0.0, 5.0, DEFAULT_BEAT_SENSITIVITY,
                                                     G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property(gobject_class, PROP_HARD_CUT_DURATION,
                                  g_param_spec_double("hard-cut-duration", "Hard Cut Duration",
                                                      "Sets the duration, in seconds, for hard cuts. Hard cuts are abrupt transitions in the visualizer.",
                                                      0.0, 999999.0, DEFAULT_HARD_CUT_DURATION,
                                                      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property(gobject_class, PROP_HARD_CUT_ENABLED,
                                  g_param_spec_boolean("hard-cut-enabled", "Hard Cut Enabled",
                                                       "Enables or disables hard cuts. When enabled, the visualizer may exhibit sudden transitions based on the audio input.",
                                                       DEFAULT_HARD_CUT_ENABLED,
                                                       G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property(gobject_class, PROP_HARD_CUT_SENSITIVITY,
                                  g_param_spec_float("hard-cut-sensitivity", "Hard Cut Sensitivity",
                                                     "Adjusts the sensitivity of the visualizer to hard cuts. Higher values increase the responsiveness to abrupt changes in audio.",
                                                     0.0, 1.0, DEFAULT_HARD_CUT_SENSITIVITY,
                                                     G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property(gobject_class, PROP_SOFT_CUT_DURATION,
                                  g_param_spec_double("soft-cut-duration", "Soft Cut Duration",
                                                      "Sets the duration, in seconds, for soft cuts. Soft cuts are smoother transitions between visualizer states.",
                                                      0.0, 999999.0, DEFAULT_SOFT_CUT_DURATION,
                                                      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property(gobject_class, PROP_PRESET_DURATION,
                                  g_param_spec_double("preset-duration", "Preset Duration",
                                                      "Sets the duration, in seconds, for each preset. A zero value causes the preset to play indefinitely.",
                                                      0.0, 999999.0, DEFAULT_PRESET_DURATION,
                                                      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property(gobject_class, PROP_MESH_SIZE,
                                  g_param_spec_string("mesh-size", "Mesh Size",
                                                      "Sets the size of the mesh used in rendering. The format is 'width,height'.",
                                                      DEFAULT_MESH_SIZE,
                                                      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property(gobject_class, PROP_ASPECT_CORRECTION,
                                  g_param_spec_boolean("aspect-correction", "Aspect Correction",
                                                       "Enables or disables aspect ratio correction. When enabled, the visualizer adjusts for aspect ratio differences in rendering.",
                                                       DEFAULT_ASPECT_CORRECTION,
                                                       G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property(gobject_class, PROP_EASTER_EGG,
                                  g_param_spec_float("easter-egg", "Easter Egg",
                                                     "DControls the activation of an Easter Egg feature. The value determines the likelihood of triggering the Easter Egg.",
                                                     0.0, 1.0, DEFAULT_EASTER_EGG,
                                                     G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property(gobject_class, PROP_PRESET_LOCKED,
                                  g_param_spec_boolean("preset-locked", "Preset Locked",
                                                       "Locks or unlocks the current preset. When locked, the visualizer remains on the current preset without automatic changes.",
                                                       DEFAULT_PRESET_LOCKED,
                                                       G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  gobject_class->finalize = gst_projectm_finalize;

  scope_class->setup = GST_DEBUG_FUNCPTR(projectm_setup);
  scope_class->render = GST_DEBUG_FUNCPTR(projectm_render);
}

static gboolean projectm_setup(GstAudioVisualizer *bscope)
{
  // Cast the audio visualizer to the ProjectM plugin
  GstProjectM *plugin = GST_PROJECTM(bscope);

  // Check if GL context, window, and display exist, and create if not
  if (!plugin->display || !plugin->context || !plugin->window)
  {
    gl_init(plugin);
  }

  // Check if ProjectM instance exists, and create if not
  if (!plugin->handle)
  {
    // Create ProjectM instance
    projectm_init(plugin);

    // Calculate depth based on pixel stride and bits
    gint depth = bscope->vinfo.finfo->pixel_stride[0] * ((bscope->vinfo.finfo->bits >= 8) ? 8 : 1);

    // Calculate required samples per frame
    bscope->req_spf = (bscope->ainfo.channels * bscope->ainfo.rate * 2) / bscope->vinfo.fps_n;

    // Allocate memory for the framebuffer
    plugin->framebuffer = (uint8_t *)malloc(GST_VIDEO_INFO_WIDTH(&bscope->vinfo) * GST_VIDEO_INFO_HEIGHT(&bscope->vinfo) * 4);

    if (plugin->framebuffer == NULL)
    {
      GST_ERROR_OBJECT(plugin, "Failed to allocate memory for framebuffer");
      return FALSE;
    }

    // Log audio info
    GST_DEBUG_OBJECT(plugin,
                     "Audio Information <Channels: %d, SampleRate: %d, Description: %s>",
                     bscope->ainfo.channels, bscope->ainfo.rate, bscope->ainfo.finfo->description);

    // Log video info
    GST_DEBUG_OBJECT(plugin,
                     "Video Information <Dimensions: %dx%d, FPS: %d/%d, Depth: %dbit, SamplesPerFrame: %d>",
                     GST_VIDEO_INFO_WIDTH(&bscope->vinfo),
                     GST_VIDEO_INFO_HEIGHT(&bscope->vinfo),
                     bscope->vinfo.fps_n, bscope->vinfo.fps_d,
                     depth, bscope->req_spf);
  }

  return TRUE;
}

static gboolean projectm_render(GstAudioVisualizer *bscope, GstBuffer *audio,
                                GstVideoFrame *video)
{
  GstProjectM *plugin = GST_PROJECTM(bscope);

  return true;
}

static gboolean plugin_init(GstPlugin *plugin)
{
  GST_DEBUG_CATEGORY_INIT(gst_projectm_debug, "projectm", 0,
                          "projectM visualizer plugin");

  return gst_element_register(plugin, "projectm", GST_RANK_NONE, GST_TYPE_PROJECTM);
}

GST_PLUGIN_DEFINE(GST_VERSION_MAJOR, GST_VERSION_MINOR, projectm,
                  "plugin to visualize audio using the ProjectM library", plugin_init,
                  PACKAGE_VERSION, PACKAGE_LICENSE, PACKAGE_NAME, PACKAGE_ORIGIN)