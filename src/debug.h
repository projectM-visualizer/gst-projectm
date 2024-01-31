#ifndef __GST_PROJECTM_DEBUG_H__
#define __GST_PROJECTM_DEBUG_H__

#include <glib.h>
#include <gst/gl/gl.h>

G_BEGIN_DECLS

/**
 * @brief Print the current OpenGL error to stderr.
 * 
 * @param context The OpenGL context.
 * @param data Unused.
 */
void gl_error_handler(GstGLContext *context, gpointer data);

G_END_DECLS

#endif /* __GST_PROJECTM_DEBUG_H__ */