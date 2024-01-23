/*
 * GStreamer
 * Copyright (C) 2024 AnomieVision <>
 */

#ifndef __GST_PLUGIN_PROJECTM_H__
#define __GST_PLUGIN_PROJECTM_H__

#include <gst/gst.h>
#include <gst/pbutils/gstaudiovisualizer.h>

G_BEGIN_DECLS

#define GST_TYPE_PLUGIN_PROJECTM (gst_plugin_projectm_get_type())
G_DECLARE_FINAL_TYPE(GstPluginProjectM, gst_plugin_projectm, GST,
                     PLUGIN_PROJECTM, GstElement)

struct _GstPluginProjectM {
  GstAudioVisualizer element;
  GstPad *sinkpad, *srcpad;
  gboolean silent;
  
  gchar *preset;
};

struct _GstPluginProjectMClass {
  GstAudioVisualizerClass parent_class;
};

static void gst_plugin_projectm_set_property(GObject *object, guint prop_id,
                                             const GValue *value,
                                             GParamSpec *pspec);
static void gst_plugin_projectm_get_property(GObject *object, guint prop_id,
                                             GValue *value, GParamSpec *pspec);

static void gst_plugin_projectm_init(GstPluginProjectM *plugin);

static void gst_plugin_projectm_finalize(GObject *object);

static void gst_plugin_projectm_class_init(GstPluginProjectMClass *klass);

static gboolean plugin_init (GstPlugin * plugin);

G_END_DECLS

#endif /* __GST_PLUGIN_PROJECTM_H__ */