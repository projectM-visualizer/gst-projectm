#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gl/gl.h>
#include <gst/gl/gstglfuncs.h>

#include "gl.h"
#include "plugin.h"

void gl_init(GstProjectM *plugin)
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
}