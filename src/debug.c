#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gl/gl.h>
#include <gst/gl/gstglfuncs.h>

#include "debug.h"

void gl_error_handler(GstGLContext *context, gpointer data)
{
    GLuint error = context->gl_vtable->GetError();

    switch (error)
    {
    case GL_NO_ERROR:
        // No error
        break;
    case GL_INVALID_ENUM:
        g_error("OpenGL Error: GL_INVALID_ENUM - Enumeration parameter is not legal\n");
        break;
    case GL_INVALID_VALUE:
        g_error("OpenGL Error: GL_INVALID_VALUE - Value parameter is not legal\n");
        break;
    case GL_INVALID_OPERATION:
        g_error("OpenGL Error: GL_INVALID_OPERATION - Set of state is not legal for the parameters given\n");
        break;
    case GL_STACK_OVERFLOW:
        g_error("OpenGL Error: GL_STACK_OVERFLOW - Stack pushing operation would overflow\n");
        break;
    case GL_STACK_UNDERFLOW:
        g_error("OpenGL Error: GL_STACK_UNDERFLOW - Stack popping operation would underflow\n");
        break;
    case GL_OUT_OF_MEMORY:
        g_error("OpenGL Error: GL_OUT_OF_MEMORY - Memory allocation failed\n");
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        g_error("OpenGL Error: GL_INVALID_FRAMEBUFFER_OPERATION - Incomplete framebuffer operation\n");
        break;
    case GL_CONTEXT_LOST:
        g_error("OpenGL Error: GL_CONTEXT_LOST - OpenGL context lost\n");
        break;
    default:
        g_error("OpenGL Error: Unknown error code - 0x%x\n", error);
        break;
    }
}