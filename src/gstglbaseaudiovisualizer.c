/*
 * GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
 * Copyright (C) 2002,2007 David A. Schleef <ds@schleef.org>
 * Copyright (C) 2008 Julien Isorce <julien.isorce@gmail.com>
 * Copyright (C) 2015 Matthew Waters <matthew@centricular.com>
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
 * GStreamer / gst-plugins-base / 1.19.2: gst-libs/gst/gl/gstglbasesrc.c
 * Git Repository: https://github.com/GStreamer/gst-plugins-base/blob/master/gst-libs/gst/gl/gstglbasesrc.c
 * Original copyright notice has been retained at the top of this file.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gl/gl.h>
#include "gstglbaseaudiovisualizer.h"

/**
 * SECTION:GstGLBaseAudioVisualizer
 * @short_description: #GstAudioVisualizer subclass for injecting OpenGL resources in a pipeline
 * @title: GstGLBaseAudioVisualizer
 * @see_also: #GstAudioVisualizer
 *
 * Wrapper for GstAudioVisualizer for handling OpenGL contexts.
 *
 * #GstGLBaseAudioVisualizer handles the nitty gritty details of retrieving an OpenGL
 * context. It also provides `gl_start()` and `gl_stop()` virtual methods
 * that ensure an OpenGL context is available and current in the calling thread for initializing and cleaning up
 * OpenGL dependent resources.
 * The `gl_render` virtual method is used to perform OpenGL rendering.
 */

#define GST_CAT_DEFAULT gst_gl_base_audio_visualizer_debug
GST_DEBUG_CATEGORY_STATIC (GST_CAT_DEFAULT);

struct _GstGLBaseAudioVisualizerPrivate
{
    GstGLContext *other_context;

    gint64 n_frames;              /* total frames sent */
    gboolean gl_result;
    gboolean gl_started;

    GRecMutex context_lock;
};

/* Properties */
enum
{
    PROP_0
};

#define gst_gl_base_audio_visualizer_parent_class parent_class
G_DEFINE_ABSTRACT_TYPE_WITH_CODE (GstGLBaseAudioVisualizer, gst_gl_base_audio_visualizer,
                                  GST_TYPE_AUDIO_VISUALIZER, G_ADD_PRIVATE (GstGLBaseAudioVisualizer)
                                          GST_DEBUG_CATEGORY_INIT (gst_gl_base_audio_visualizer_debug,
                                                                   "glbaseaudiovisualizer", 0, "glbaseaudiovisualizer element");
);

static void gst_gl_base_audio_visualizer_finalize (GObject * object);
static void gst_gl_base_audio_visualizer_set_property (GObject * object, guint prop_id,
                                          const GValue * value, GParamSpec * pspec);
static void gst_gl_base_audio_visualizer_get_property (GObject * object, guint prop_id,
                                          GValue * value, GParamSpec * pspec);

static void gst_gl_base_audio_visualizer_set_context (GstElement * element,
                                         GstContext * context);
static GstStateChangeReturn gst_gl_base_audio_visualizer_change_state (GstElement * element,
                                                          GstStateChange transition);

static gboolean gst_gl_base_audio_visualizer_render (GstAudioVisualizer *bscope, GstBuffer *audio, GstVideoFrame *video);
static void gst_gl_base_audio_visualizer_start (GstGLBaseAudioVisualizer * glav);
static void gst_gl_base_audio_visualizer_stop (GstGLBaseAudioVisualizer * glav);
static gboolean gst_gl_base_audio_visualizer_decide_allocation (GstAudioVisualizer * gstav,
                                                   GstQuery * query);

static gboolean gst_gl_base_audio_visualizer_default_setup (GstGLBaseAudioVisualizer * glav);
static gboolean gst_gl_base_audio_visualizer_default_gl_start (GstGLBaseAudioVisualizer * glav);
static void gst_gl_base_audio_visualizer_default_gl_stop (GstGLBaseAudioVisualizer * glav);
static gboolean gst_gl_base_audio_visualizer_default_gl_render (GstGLBaseAudioVisualizer * glav,
                                                                GstBuffer* audio, GstVideoFrame *video);

static gboolean gst_gl_base_audio_visualizer_find_gl_context_unlocked (GstGLBaseAudioVisualizer * glav);

static gboolean gst_gl_base_audio_visualizer_setup(GstAudioVisualizer * gstav);

static void
gst_gl_base_audio_visualizer_class_init (GstGLBaseAudioVisualizerClass * klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    GstAudioVisualizerClass *gstav_class = GST_AUDIO_VISUALIZER_CLASS (klass);
    GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

    gobject_class->finalize = gst_gl_base_audio_visualizer_finalize;
    gobject_class->set_property = gst_gl_base_audio_visualizer_set_property;
    gobject_class->get_property = gst_gl_base_audio_visualizer_get_property;

    element_class->set_context = GST_DEBUG_FUNCPTR (gst_gl_base_audio_visualizer_set_context);

    element_class->change_state =
            GST_DEBUG_FUNCPTR (gst_gl_base_audio_visualizer_change_state);

    gstav_class->decide_allocation = GST_DEBUG_FUNCPTR (gst_gl_base_audio_visualizer_decide_allocation);
    gstav_class->setup = GST_DEBUG_FUNCPTR (gst_gl_base_audio_visualizer_setup);

    gstav_class->render = GST_DEBUG_FUNCPTR (gst_gl_base_audio_visualizer_render);

    klass->supported_gl_api = GST_GL_API_ANY;
    klass->gl_start = GST_DEBUG_FUNCPTR (gst_gl_base_audio_visualizer_default_gl_start);
    klass->gl_stop = GST_DEBUG_FUNCPTR (gst_gl_base_audio_visualizer_default_gl_stop);
    klass->gl_render = GST_DEBUG_FUNCPTR (gst_gl_base_audio_visualizer_default_gl_render);
    klass->setup = GST_DEBUG_FUNCPTR (gst_gl_base_audio_visualizer_default_setup);
}

static void
gst_gl_base_audio_visualizer_init (GstGLBaseAudioVisualizer * glav)
{
    glav->priv = gst_gl_base_audio_visualizer_get_instance_private (glav);
    glav->priv->gl_started = FALSE;
    glav->priv->gl_result = TRUE;
    glav->context = NULL;
    g_rec_mutex_init (&glav->priv->context_lock);
    gst_gl_base_audio_visualizer_start(glav);
}

static void
gst_gl_base_audio_visualizer_finalize (GObject * object)
{
    GstGLBaseAudioVisualizer *glav = GST_GL_BASE_AUDIO_VISUALIZER (object);
    gst_gl_base_audio_visualizer_stop(glav);

    g_rec_mutex_clear (&glav->priv->context_lock);

    G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gst_gl_base_audio_visualizer_set_property (GObject * object, guint prop_id,
                              const GValue * value, GParamSpec * pspec)
{
    GstGLBaseAudioVisualizer *glav = GST_GL_BASE_AUDIO_VISUALIZER (object);

    switch (prop_id) {
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
gst_gl_base_audio_visualizer_get_property (GObject * object, guint prop_id,
                              GValue * value, GParamSpec * pspec)
{
    GstGLBaseAudioVisualizer *glav = GST_GL_BASE_AUDIO_VISUALIZER (object);

    switch (prop_id) {
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}


static void
gst_gl_base_audio_visualizer_set_context (GstElement * element, GstContext * context)
{
    GstGLBaseAudioVisualizer *glav = GST_GL_BASE_AUDIO_VISUALIZER (element);
    GstGLBaseAudioVisualizerClass *klass = GST_GL_BASE_AUDIO_VISUALIZER_GET_CLASS (glav);
    GstGLDisplay *old_display, *new_display;

    g_rec_mutex_lock (&glav->priv->context_lock);
    old_display = glav->display ? gst_object_ref (glav->display) : NULL;
    gst_gl_handle_set_context (element, context, &glav->display,
                               &glav->priv->other_context);
    if (glav->display)
        gst_gl_display_filter_gl_api (glav->display, klass->supported_gl_api);
    new_display = glav->display ? gst_object_ref (glav->display) : NULL;

    if (old_display && new_display) {
        if (old_display != new_display) {
            gst_clear_object (&glav->context);
            if (gst_gl_base_audio_visualizer_find_gl_context_unlocked (glav)) {
              // TODO does this need to be handled ?
              //gst_pad_mark_reconfigure (GST_BASE_SRC_PAD (glav));
            }
        }
    }
    gst_clear_object (&old_display);
    gst_clear_object (&new_display);
    g_rec_mutex_unlock (&glav->priv->context_lock);

    GST_ELEMENT_CLASS (parent_class)->set_context (element, context);
}

static gboolean
gst_gl_base_audio_visualizer_default_gl_start (GstGLBaseAudioVisualizer * glav)
{
    return TRUE;
}

static gboolean
gst_gl_base_audio_visualizer_default_setup (GstGLBaseAudioVisualizer * glav)
{
    return TRUE;
}

static void
gst_gl_base_audio_visualizer_gl_start (GstGLContext * context, gpointer data)
{
    GstGLBaseAudioVisualizer *glav = GST_GL_BASE_AUDIO_VISUALIZER (data);
    GstGLBaseAudioVisualizerClass *glav_class = GST_GL_BASE_AUDIO_VISUALIZER_GET_CLASS (glav);

    GST_INFO_OBJECT (glav, "starting");
    gst_gl_insert_debug_marker (glav->context,
                                "starting element %s", GST_OBJECT_NAME (glav));

    glav->priv->gl_started = glav_class->gl_start (glav);
}

static void
gst_gl_base_audio_visualizer_default_gl_stop (GstGLBaseAudioVisualizer * glav)
{
}

static void
gst_gl_base_audio_visualizer_gl_stop (GstGLContext * context, gpointer data)
{
    GstGLBaseAudioVisualizer *glav = GST_GL_BASE_AUDIO_VISUALIZER (data);
    GstGLBaseAudioVisualizerClass *glav_class = GST_GL_BASE_AUDIO_VISUALIZER_GET_CLASS (glav);

    GST_INFO_OBJECT (glav, "stopping");
    gst_gl_insert_debug_marker (glav->context,
                                "stopping element %s", GST_OBJECT_NAME (glav));

    if (glav->priv->gl_started)
        glav_class->gl_stop (glav);

    glav->priv->gl_started = FALSE;
}

static gboolean gst_gl_base_audio_visualizer_setup(GstAudioVisualizer * gstav) {
    GstGLBaseAudioVisualizer *glav = GST_GL_BASE_AUDIO_VISUALIZER (gstav);
    GstGLBaseAudioVisualizerClass *glav_class = GST_GL_BASE_AUDIO_VISUALIZER_GET_CLASS(gstav);

    // cascade setup to the derived plugin after gl initialization has been completed
    return glav_class->setup(glav);
}

static gboolean
gst_gl_base_audio_visualizer_default_gl_render (GstGLBaseAudioVisualizer * glav, GstBuffer* audio, GstVideoFrame *video)
{
    return TRUE;
}

typedef struct {
    GstGLBaseAudioVisualizer *glav;
    GstBuffer *in_audio;
    GstVideoFrame *out_video;
} GstGLRenderCallbackParams;

static void
gst_gl_base_audio_visualizer_gl_thread_render_callback (gpointer params)
{
    GstGLRenderCallbackParams* cb_params = (GstGLRenderCallbackParams*)params;
    GstGLBaseAudioVisualizerClass *klass = GST_GL_BASE_AUDIO_VISUALIZER_GET_CLASS (cb_params->glav);

    // inside gl thread: call virtual render function with audio and video
    cb_params->glav->priv->gl_result = klass->gl_render (cb_params->glav, cb_params->in_audio, cb_params->out_video);
}

static gboolean
gst_gl_base_audio_visualizer_render (GstAudioVisualizer *bscope, GstBuffer *audio, GstVideoFrame *video)
{
    GstGLBaseAudioVisualizer *glav = GST_GL_BASE_AUDIO_VISUALIZER (bscope);
    GstGLRenderCallbackParams cb_params;
    GstGLWindow *window;

    g_rec_mutex_lock (&glav->priv->context_lock);

    // wrap params into cb_params struct to pass them to the GL window/thread via userdata pointer
    cb_params.glav = glav;
    cb_params.in_audio = audio;
    cb_params.out_video = video;

    window = gst_gl_context_get_window (glav->context);

    // dispatch render call through the gl thread
    // call is blocking, accessing audio and video params from gl thread *should* be safe
    gst_gl_window_send_message(window, GST_GL_WINDOW_CB(gst_gl_base_audio_visualizer_gl_thread_render_callback),
                               &cb_params);

    gst_object_unref(window);

    g_rec_mutex_unlock (&glav->priv->context_lock);

    if (glav->priv->gl_result) {
        glav->priv->n_frames++;
    } else {
        // gl error
        GST_ELEMENT_ERROR (glav, RESOURCE, NOT_FOUND, (("failed to render audio visualizer")),
                         (("A GL error occurred")));
    }

    return glav->priv->gl_result;
}

static void
gst_gl_base_audio_visualizer_start (GstGLBaseAudioVisualizer * glav)
{
    glav->priv->n_frames = 0;
}

static void
gst_gl_base_audio_visualizer_stop (GstGLBaseAudioVisualizer * glav)
{
    g_rec_mutex_lock (&glav->priv->context_lock);

    if (glav->context) {
        if (glav->priv->gl_started)
            gst_gl_context_thread_add (glav->context, gst_gl_base_audio_visualizer_gl_stop, glav);

        gst_object_unref (glav->context);
    }

    glav->context = NULL;
    g_rec_mutex_unlock (&glav->priv->context_lock);
}

static gboolean
_find_local_gl_context_unlocked (GstGLBaseAudioVisualizer * glav)
{
    GstGLContext *context, *prev_context;
    gboolean ret;

    if (glav->context && glav->context->display == glav->display)
        return TRUE;

    context = prev_context = glav->context;
    g_rec_mutex_unlock (&glav->priv->context_lock);
    /* we need to drop the lock to query as another element may also be
     * performing a context query on us which would also attempt to take the
     * context_lock. Our query could block on the same lock in the other element.
     */
    ret =
            gst_gl_query_local_gl_context (GST_ELEMENT (glav), GST_PAD_SRC, &context);
    g_rec_mutex_lock (&glav->priv->context_lock);
    if (ret) {
        if (glav->context != prev_context) {
            /* we need to recheck everything since we dropped the lock and the
             * context has changed */
            if (glav->context && glav->context->display == glav->display) {
                if (context != glav->context)
                    gst_clear_object (&context);
                return TRUE;
            }
        }

        if (context->display == glav->display) {
            glav->context = context;
            return TRUE;
        }
        if (context != glav->context)
            gst_clear_object (&context);
    }
    return FALSE;
}

static gboolean
gst_gl_base_audio_visualizer_find_gl_context_unlocked (GstGLBaseAudioVisualizer * glav)
{
    GstGLBaseAudioVisualizerClass *klass = GST_GL_BASE_AUDIO_VISUALIZER_GET_CLASS (glav);
    GError *error = NULL;
    gboolean new_context = FALSE;

    GST_DEBUG_OBJECT (glav, "attempting to find an OpenGL context, existing %"
            GST_PTR_FORMAT, glav->context);

    if (!glav->context)
        new_context = TRUE;

    if (!gst_gl_ensure_element_data (glav, &glav->display,
                                     &glav->priv->other_context))
        return FALSE;

    gst_gl_display_filter_gl_api (glav->display, klass->supported_gl_api);

    _find_local_gl_context_unlocked (glav);

    if (!glav->context) {
        GST_OBJECT_LOCK (glav->display);
        do {
            if (glav->context) {
                gst_object_unref (glav->context);
                glav->context = NULL;
            }
            /* just get a GL context.  we don't care */
            glav->context =
                    gst_gl_display_get_gl_context_for_thread (glav->display, NULL);
            if (!glav->context) {
                if (!gst_gl_display_create_context (glav->display,
                                                    glav->priv->other_context, &glav->context, &error)) {
                    GST_OBJECT_UNLOCK (glav->display);
                    goto context_error;
                }
            }
        } while (!gst_gl_display_add_context (glav->display, glav->context));
        GST_OBJECT_UNLOCK (glav->display);
    }
    GST_INFO_OBJECT (glav, "found OpenGL context %" GST_PTR_FORMAT, glav->context);

    if (new_context || !glav->priv->gl_started) {
        if (glav->priv->gl_started)
            gst_gl_context_thread_add (glav->context, gst_gl_base_audio_visualizer_gl_stop, glav);

        {
            if ((gst_gl_context_get_gl_api (glav->
                    context) & klass->supported_gl_api) == 0)
                goto unsupported_gl_api;
        }

        gst_gl_context_thread_add (glav->context, gst_gl_base_audio_visualizer_gl_start, glav);

        if (!glav->priv->gl_started)
            goto error;
    }

    return TRUE;

    unsupported_gl_api:
    {
        GstGLAPI gl_api = gst_gl_context_get_gl_api (glav->context);
        gchar *gl_api_str = gst_gl_api_to_string (gl_api);
        gchar *supported_gl_api_str =
                gst_gl_api_to_string (klass->supported_gl_api);
        GST_ELEMENT_ERROR (glav, RESOURCE, BUSY,
                           ("GL API's not compatible context: %s supported: %s", gl_api_str,
                                   supported_gl_api_str), (NULL));

        g_free (supported_gl_api_str);
        g_free (gl_api_str);
        return FALSE;
    }
    context_error:
    {
        if (error) {
            GST_ELEMENT_ERROR (glav, RESOURCE, NOT_FOUND, ("%s", error->message),
                               (NULL));
            g_clear_error (&error);
        } else {
            GST_ELEMENT_ERROR (glav, RESOURCE, NOT_FOUND, (NULL), (NULL));
        }
        if (glav->context)
            gst_object_unref (glav->context);
        glav->context = NULL;
        return FALSE;
    }
    error:
    {
        GST_ELEMENT_ERROR (glav, LIBRARY, INIT,
                           ("Subclass failed to initialize."), (NULL));
        return FALSE;
    }
}

static gboolean
gst_gl_base_audio_visualizer_decide_allocation (GstAudioVisualizer * gstav, GstQuery * query)
{
    GstGLBaseAudioVisualizer *glav = GST_GL_BASE_AUDIO_VISUALIZER (gstav);
    GstGLContext *context;
    GstBufferPool *pool = NULL;
    GstStructure *config;
    GstCaps *caps;
    guint min, max, size;
    gboolean update_pool;

    g_rec_mutex_lock (&glav->priv->context_lock);
    if (!gst_gl_base_audio_visualizer_find_gl_context_unlocked (glav)) {
        g_rec_mutex_unlock (&glav->priv->context_lock);
        return FALSE;
    }
    context = gst_object_ref (glav->context);
    g_rec_mutex_unlock (&glav->priv->context_lock);

    gst_query_parse_allocation (query, &caps, NULL);

    if (gst_query_get_n_allocation_pools (query) > 0) {
        gst_query_parse_nth_allocation_pool (query, 0, &pool, &size, &min, &max);

        update_pool = TRUE;
    } else {
        GstVideoInfo vinfo;

        gst_video_info_init (&vinfo);
        gst_video_info_from_caps (&vinfo, caps);
        size = vinfo.size;
        min = max = 0;
        update_pool = FALSE;
    }

    if (!pool || !GST_IS_GL_BUFFER_POOL (pool)) {
        /* can't use this pool */
        if (pool)
            gst_object_unref (pool);
        pool = gst_gl_buffer_pool_new (context);
    }
    config = gst_buffer_pool_get_config (pool);

    gst_buffer_pool_config_set_params (config, caps, size, min, max);
    gst_buffer_pool_config_add_option (config, GST_BUFFER_POOL_OPTION_VIDEO_META);
    if (gst_query_find_allocation_meta (query, GST_GL_SYNC_META_API_TYPE, NULL))
        gst_buffer_pool_config_add_option (config,
                                           GST_BUFFER_POOL_OPTION_GL_SYNC_META);
    gst_buffer_pool_config_add_option (config,
                                       GST_BUFFER_POOL_OPTION_VIDEO_GL_TEXTURE_UPLOAD_META);

    gst_buffer_pool_set_config (pool, config);

    if (update_pool)
        gst_query_set_nth_allocation_pool (query, 0, pool, size, min, max);
    else
        gst_query_add_allocation_pool (query, pool, size, min, max);

    gst_object_unref (pool);
    gst_object_unref (context);

    return TRUE;
}

static GstStateChangeReturn
gst_gl_base_audio_visualizer_change_state (GstElement * element, GstStateChange transition)
{
    GstGLBaseAudioVisualizer *glav = GST_GL_BASE_AUDIO_VISUALIZER (element);
    GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;

    GST_DEBUG_OBJECT (glav, "changing state: %s => %s",
                      gst_element_state_get_name (GST_STATE_TRANSITION_CURRENT (transition)),
                      gst_element_state_get_name (GST_STATE_TRANSITION_NEXT (transition)));

    ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);
    if (ret == GST_STATE_CHANGE_FAILURE)
        return ret;

    switch (transition) {
        case GST_STATE_CHANGE_READY_TO_NULL:
            g_rec_mutex_lock (&glav->priv->context_lock);
            gst_clear_object (&glav->priv->other_context);
            gst_clear_object (&glav->display);
            g_rec_mutex_unlock (&glav->priv->context_lock);
            break;
        default:
            break;
    }

    return ret;
}

