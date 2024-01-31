#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gl/gl.h>
#include <gst/gl/gstglfuncs.h>

#include "gl.h"
#include "debug.h"
#include "plugin.h"

GST_DEBUG_CATEGORY_STATIC(gstprojectm_gl_debug);
#define GST_CAT_DEFAULT gstprojectm_gl_debug

void gl_init(GstProjectM *plugin)
{
  GST_DEBUG_CATEGORY_INIT(gstprojectm_gl_debug, "projectm",
                          0, "OpenGL");

  // Error object
  GError *error = NULL;

  // Check if there is another GL context active
  GstGLContext *other = gst_gl_context_get_current();
  if (other)
    return;

  // Create GL display
  GstGLDisplay *display = gst_gl_display_new_with_type(GST_GL_DISPLAY_TYPE_ANY);

  // Check if display was created
  if (!display)
  {
    GST_ERROR_OBJECT(plugin, "Failed to create OpenGL display");
    return;
  }

  // Create GL context
  GstGLContext *context = gst_gl_context_new(display);
  gboolean created_context = gst_gl_context_create(context, NULL, &error);

  // Check if context was created
  if (!created_context)
  {
    GST_ERROR_OBJECT(plugin, "Failed to create OpenGL context");
    gl_error_handler(context, plugin);
    return;
  }

  // Get GL window
  GstGLWindow *window;
  window = gst_gl_context_get_window(context);

  // Get GL Info
  GstGLAPI api = gst_gl_context_get_gl_api(context);
  gint major, minor;
  gst_gl_context_get_gl_version(context, &major, &minor);
  GstGLSLVersion glsl_version = gst_gl_version_to_glsl_version(api, major, minor);
  const gchar *glsl_version_string = gst_glsl_version_to_string(glsl_version);
  GstGLSLProfile profile = gst_glsl_profile_from_string(glsl_version_string);

  GST_DEBUG_OBJECT(plugin, "GL API: %d", api);
  GST_DEBUG_OBJECT(plugin, "GL version: %d.%d", major, minor);
  GST_DEBUG_OBJECT(plugin, "GLSL version: %d", glsl_version);
  GST_DEBUG_OBJECT(plugin, "GLSL profile: %d", profile);

  // Check whether context supports the requested version
  if (!gst_gl_context_check_gl_version(context, api, major, minor))
  {
    GST_ERROR_OBJECT(plugin, "OpenGL context does not support the requested version");
    gl_error_handler(context, plugin);
  }
  else
  {
    GST_DEBUG_OBJECT(plugin, "OpenGL context supports the requested version: %d.%d", major, minor);
  }

  // Check whether context supports the combination of version with profile
  if (!gst_gl_context_supports_glsl_profile_version(context, glsl_version, profile))
  {
    GST_ERROR_OBJECT(plugin, "OpenGL context does not support the combination of version with profile");
    gl_error_handler(context, plugin);
  }
  else
  {
    GST_DEBUG_OBJECT(plugin, "OpenGL context supports the combination of version with profile");
  }

  // Check whether context supports the 'precision' specifier in GLSL shaders

  if (!gst_gl_context_supports_precision(context, glsl_version, profile))
  {
    GST_WARNING_OBJECT(plugin, "OpenGL context does not support the 'precision' specifier in GLSL shaders");
    gl_error_handler(context, plugin);
  }
  else
  {
    GST_DEBUG_OBJECT(plugin, "OpenGL context supports the 'precision' specifier in GLSL shaders");
  }

  // Check whether context supports the 'precision highp' specifier in GLSL shaders
  if (!gst_gl_context_supports_precision_highp(context, glsl_version, profile))
  {
    GST_WARNING_OBJECT(plugin, "OpenGL context does not support the 'precision highp' specifier in GLSL shaders");
    gl_error_handler(context, plugin);
  }
  else
  {
    GST_DEBUG_OBJECT(plugin, "OpenGL context supports the 'precision highp' specifier in GLSL shaders");
  }

  // Get information from the audio visualizer
  GstAudioVisualizer *scope = GST_AUDIO_VISUALIZER(plugin);
  plugin->fps = scope->vinfo.fps_n;
  plugin->window_width = scope->vinfo.width;
  plugin->window_height = scope->vinfo.height;

  // TODO: Needs to be research to resize to the window, currently doesnt work right.
  // Set preferred size and render rectangle
  gst_gl_window_set_preferred_size(window, plugin->window_width, plugin->window_height);
  gst_gl_window_set_render_rectangle(window, 0, 0, plugin->window_width, plugin->window_height);
  gst_gl_window_resize(window, plugin->window_width, plugin->window_height);

  // Activate GL context
  if (!gst_gl_context_activate(context, true))
  {
    GST_ERROR_OBJECT(plugin, "Failed to create OpenGL context");
    gl_error_handler(context, plugin);
    return;
  }

  // Create GL framebuffer
  GstGLFramebuffer *framebuffer = gst_gl_framebuffer_new(context);

  // Check if framebuffer was created
  if (!gst_gl_context_check_framebuffer_status(context, GL_FRAMEBUFFER))
  {
    GST_ERROR_OBJECT(plugin, "Failed to create OpenGL framebuffer");
    gl_error_handler(context, plugin);
    return;
  }

  // Initialize GL memory
  gst_gl_memory_init_once();

  // Set GL window for the context
  gst_gl_context_set_window(context, window);

  // Get current GL context
  guint curr_con = gst_gl_context_get_current_gl_context(GST_GL_PLATFORM_ANY);
  GST_DEBUG_OBJECT(plugin, "OpenGL Current Context: %d\n", curr_con);

  // Set display, context, and window on the plugin object
  plugin->display = display;
  plugin->context = context;
  plugin->window = window;
}