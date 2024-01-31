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

GST_DEBUG_CATEGORY_STATIC(gst_projectm_debug);
#define GST_CAT_DEFAULT gst_projectm_debug

G_DEFINE_TYPE_WITH_CODE(GstProjectM, gst_projectm, GST_TYPE_AUDIO_VISUALIZER,
                        GST_DEBUG_CATEGORY_INIT(gst_projectm_debug,
                                                "projectM", 0,
                                                "projectM visualizer plugin with code"));

void gst_projectm_set_property(GObject *object, guint property_id,
                               const GValue *value, GParamSpec *pspec)
{
  GstProjectM *plugin = GST_PROJECTM(object);

  GST_DEBUG_OBJECT(plugin, "set_property");

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

  GST_DEBUG_OBJECT(plugin, "get_property");

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

static void projectm_init_instance(GstProjectM *plugin)
{
  // Check if there is another GL context active
  GstGLContext *other = gst_gl_context_get_current();
  if (other)
    return;

  // Create GL display and context
  GstGLDisplay *display = gst_gl_display_new_with_type(GST_GL_DISPLAY_TYPE_ANY);
  GstGLContext *context = gst_gl_context_new(display);
  GError *error = NULL;

  GstGLWindow *window;

  // Get information from the audio visualizer
  GstAudioVisualizer *scope = GST_AUDIO_VISUALIZER(plugin);
  plugin->fps = scope->vinfo.fps_n;
  plugin->window_width = scope->vinfo.width;
  plugin->window_height = scope->vinfo.height;

  GST_LOG_OBJECT(plugin, "fps: %d", plugin->fps);

  // Create GL context and window
  gst_gl_context_create(context, NULL, &error);
  window = gst_gl_context_get_window(context);

  // Get GL version
  gint major, minor;
  gst_gl_context_get_gl_version(context, &major, &minor);

  // Set preferred size and render rectangle
  gst_gl_window_set_preferred_size(window, plugin->window_width, plugin->window_height);
  gst_gl_window_set_render_rectangle(window, 0, 0, plugin->window_width, plugin->window_height);
  gst_gl_window_resize(window, plugin->window_width, plugin->window_height);

  // Activate GL context
  gboolean is_context_active = gst_gl_context_activate(context, true);

  // Check if GL context is active
  if (!is_context_active)
  {
    GST_ERROR_OBJECT(plugin, "Failed to create GL context");
    gl_error_handler(context, plugin);
    return;
  }

  // Create GL framebuffer
  GstGLFramebuffer *fbo = gst_gl_framebuffer_new(context);

  // Check if framebuffer was created
  // gboolean is_fbo_active = gst_gl_context_check_framebuffer_status(context, gst_gl_framebuffer_get_id(fbo));

  // if (!is_fbo_active)
  // {
  //   GST_ERROR_OBJECT(plugin, "Failed to create GL framebuffer");
  //   gl_error_handler(context, plugin);
  //   return;
  // }

  // Initialize GL memory
  gst_gl_memory_init_once();

  // Set GL window for the context
  gst_gl_context_set_window(context, window);

  // Get current GL context
  guint curr_con = gst_gl_context_get_current_gl_context(GST_GL_PLATFORM_GLX);
  GST_DEBUG_OBJECT(plugin, "current context: %d\n", curr_con);

  // Set display, window, context, and handle for the plugin
  plugin->display = display;
  plugin->window = window;
  plugin->context = context;

  // Create ProjectM instance
  plugin->handle = projectm_create();
  if (!plugin->handle)
  {
    GST_LOG("Could not create instance");
  }
  else
  {
    GST_LOG("Created instance!");
  }
}

static gboolean projectm_setup(GstAudioVisualizer *bscope)
{
  // Cast the audio visualizer to the ProjectM plugin
  GstProjectM *plugin = GST_PROJECTM(bscope);

  // Check if the plugin has already been initialized
  if (!plugin->handle)
  {
    // Calculate depth based on pixel stride and bits
    gint depth = bscope->vinfo.finfo->pixel_stride[0] * ((bscope->vinfo.finfo->bits >= 8) ? 8 : 1);

    // Initialize the ProjectM instance

    projectm_init_instance(plugin);

    // Load preset file if path is provided
    if (plugin->preset_path != NULL)
      projectm_load_preset_file(plugin->handle, plugin->preset_path, false);

    // Set texture search paths if directory path is provided
    if (plugin->texture_dir_path != NULL)
    {
      const gchar *texturePaths[1] = {plugin->texture_dir_path};
      projectm_set_texture_search_paths(plugin->handle, texturePaths, 1);
    }

    // Calculate required samples per frame
    bscope->req_spf = (bscope->ainfo.channels * bscope->ainfo.rate * 2) / bscope->vinfo.fps_n;

    // Log audio info description
    GST_LOG_OBJECT(plugin, "%s", bscope->ainfo.finfo->description);

    // Log All Settings
    g_print("\npreset: %s", plugin->preset_path);
    g_print("\ntexture-dir: %s", plugin->texture_dir_path);
    g_print("\nbeat-sensitivity: %f", plugin->beat_sensitivity);
    g_print("\nhard-cut-duration: %f", plugin->hard_cut_duration);
    g_print("\nhard-cut-enabled: %d", plugin->hard_cut_enabled);
    g_print("\nhard-cut-sensitivity: %f", plugin->hard_cut_sensitivity);
    g_print("\nsoft-cut-duration: %f", plugin->soft_cut_duration);
    g_print("\npreset-duration: %f", plugin->preset_duration);
    g_print("\nmesh-size: %d,%d", plugin->mesh_width, plugin->mesh_height);
    g_print("\naspect-correction: %d", plugin->aspect_correction);
    g_print("\neaster-egg: %f", plugin->easter_egg);
    g_print("\npreset-locked: %d\n\n", plugin->preset_locked);

    // Set various ProjectM parameters
    projectm_set_fps(plugin->handle, bscope->vinfo.fps_n);
    projectm_set_window_size(plugin->handle, GST_VIDEO_INFO_WIDTH(&bscope->vinfo), GST_VIDEO_INFO_HEIGHT(&bscope->vinfo));
    projectm_set_beat_sensitivity(plugin->handle, plugin->beat_sensitivity);
    projectm_set_hard_cut_duration(plugin->handle, plugin->hard_cut_duration);
    projectm_set_hard_cut_enabled(plugin->handle, plugin->hard_cut_enabled);
    projectm_set_hard_cut_sensitivity(plugin->handle, plugin->hard_cut_sensitivity);
    projectm_set_soft_cut_duration(plugin->handle, plugin->soft_cut_duration);

    if (plugin->preset_duration >= 0.0)
    {
      projectm_set_preset_duration(plugin->handle, plugin->preset_duration);
    }
    else
    {
      projectm_set_preset_duration(plugin->handle, 999999.0);
    }

    projectm_set_mesh_size(plugin->handle, plugin->mesh_width, plugin->mesh_height);
    projectm_set_aspect_correction(plugin->handle, plugin->aspect_correction);
    projectm_set_easter_egg(plugin->handle, plugin->easter_egg);
    projectm_set_preset_locked(plugin->handle, plugin->preset_locked);

    // Allocate memory for the framebuffer
    plugin->framebuffer = (uint8_t *)malloc(GST_VIDEO_INFO_WIDTH(&bscope->vinfo) * GST_VIDEO_INFO_HEIGHT(&bscope->vinfo) * 4);

    // Log video information
    GST_DEBUG_OBJECT(plugin, "WxH: %dx%d, depth: %d, fps: %d/%d",
                     GST_VIDEO_INFO_WIDTH(&bscope->vinfo),
                     GST_VIDEO_INFO_HEIGHT(&bscope->vinfo), depth,
                     bscope->vinfo.fps_n, bscope->vinfo.fps_d);
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