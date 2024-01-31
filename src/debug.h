#ifndef __GST_PROJECTM_DEBUG_H__
#define __GST_PROJECTM_DEBUG_H__

#include <glib.h>
#include <gst/gl/gl.h>

G_BEGIN_DECLS

/**
 * @brief OpenGL error codes.
 */

#define GL_NO_ERROR 0
#define GL_INVALID_ENUM 0x0500
#define GL_INVALID_VALUE 0x0501
#define GL_INVALID_OPERATION 0x0502
#define GL_STACK_OVERFLOW 0x0503
#define GL_STACK_UNDERFLOW 0x0504
#define GL_OUT_OF_MEMORY 0x0505
#define GL_INVALID_FRAMEBUFFER_OPERATION 0x0506
#define GL_CONTEXT_LOST 0x0507

/**
 * @brief Print the current OpenGL error to stderr.
 *
 * @param context The OpenGL context.
 * @param data Unused.
 */
void gl_error_handler(GstGLContext *context, gpointer data);

G_END_DECLS

#endif /* __GST_PROJECTM_DEBUG_H__ */