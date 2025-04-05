/*
 * GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
 * Copyright (C) 2002,2007 David A. Schleef <ds@schleef.org>
 * Copyright (C) 2008 Julien Isorce <julien.isorce@gmail.com>
 * Copyright (C) 2019 Philippe Normand <philn@igalia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

/*
 * The code in this file is based on code from
 * GStreamer / gst-plugins-base / 1.19.2: gst-libs/gst/gl/gstglbasesrc.h
 * Git Repository:
 * https://github.com/GStreamer/gst-plugins-base/blob/master/gst-libs/gst/gl/gstglbasesrc.h
 * Original copyright notice has been retained at the top of this file.
 */

#ifndef __GST_GL_BASE_AUDIO_VISUALIZER_H__
#define __GST_GL_BASE_AUDIO_VISUALIZER_H__

#include <gst/gl/gstgl_fwd.h>
#include <gst/pbutils/gstaudiovisualizer.h>
#include <gst/video/video-info.h>
#include <stdint.h>

typedef struct _GstGLBaseAudioVisualizer GstGLBaseAudioVisualizer;
typedef struct _GstGLBaseAudioVisualizerClass GstGLBaseAudioVisualizerClass;
typedef struct _GstGLBaseAudioVisualizerPrivate GstGLBaseAudioVisualizerPrivate;

G_DEFINE_AUTOPTR_CLEANUP_FUNC(GstGLBaseAudioVisualizer, gst_object_unref)

G_BEGIN_DECLS

GST_GL_API
GType gst_gl_base_audio_visualizer_get_type(void);

#define GST_TYPE_GL_BASE_AUDIO_VISUALIZER                                      \
  (gst_gl_base_audio_visualizer_get_type())
#define GST_GL_BASE_AUDIO_VISUALIZER(obj)                                      \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_GL_BASE_AUDIO_VISUALIZER,        \
                              GstGLBaseAudioVisualizer))
#define GST_GL_BASE_AUDIO_VISUALIZER_CLASS(klass)                              \
  (G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_GL_BASE_AUDIO_VISUALIZER,         \
                           GstGLBaseAudioVisualizerClass))
#define GST_IS_GL_BASE_AUDIO_VISUALIZER(obj)                                   \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_GL_BASE_AUDIO_VISUALIZER))
#define GST_IS_GL_BASE_AUDIO_VISUALIZER_CLASS(klass)                           \
  (G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_GL_BASE_AUDIO_VISUALIZER))
#define GST_GL_BASE_AUDIO_VISUALIZER_GET_CLASS(obj)                            \
  (G_TYPE_INSTANCE_GET_CLASS((obj), GST_TYPE_GL_BASE_AUDIO_VISUALIZER,         \
                             GstGLBaseAudioVisualizerClass))

/**
 * GstGLBaseAudioVisualizer:
 * @display: the currently configured #GstGLDisplay
 * @context: the currently configured #GstGLContext
 *
 * The parent instance type of a base GL Audio Visualizer.
 */
struct _GstGLBaseAudioVisualizer {
  GstAudioVisualizer parent;

  /*< public >*/
  GstGLDisplay *display;
  GstGLContext *context;

  /*< private >*/
  gpointer _padding[GST_PADDING];

  GstGLBaseAudioVisualizerPrivate *priv;
};

/**
 * GstGLBaseAudioVisualizerClass:
 * @supported_gl_api: the logical-OR of #GstGLAPI's supported by this element
 * @gl_start: called in the GL thread to setup the element GL state.
 * @gl_stop: called in the GL thread to clean up the element GL state.
 * @gl_render: called in the GL thread to fill the current video texture.
 * @setup: called when the format changes (delegate from
 * GstAudioVisualizer.setup)
 *
 * The base class for OpenGL based audio visualizers.
 *
 */
struct _GstGLBaseAudioVisualizerClass {
  GstAudioVisualizerClass parent_class;

  /*< public >*/
  GstGLAPI supported_gl_api;
  gboolean (*gl_start)(GstGLBaseAudioVisualizer *glav);
  void (*gl_stop)(GstGLBaseAudioVisualizer *glav);
  gboolean (*gl_render)(GstGLBaseAudioVisualizer *glav, GstBuffer *audio,
                        GstVideoFrame *video);
  gboolean (*setup)(GstGLBaseAudioVisualizer *glav);
  /*< private >*/
  gpointer _padding[GST_PADDING];
};

G_END_DECLS

#endif /* __GST_GL_BASE_AUDIO_VISUALIZER_H__ */
