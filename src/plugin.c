#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib-object.h>
#include <gst/gst.h>

#include "plugin.h"

GST_DEBUG_CATEGORY(gst_plugin_projectm_debug);
#define GST_CAT_DEFAULT gst_plugin_projectm_debug

G_DEFINE_TYPE_WITH_CODE(GstPluginProjectM, gst_plugin_projectm, GST_TYPE_AUDIO_VISUALIZER,
                        GST_DEBUG_CATEGORY_INIT(gst_plugin_projectm_debug,
                                                "projectM", 0,
                                                "projectM visualizer plugin with code"));

enum
{
  LAST_SIGNAL
};

enum { 
    PROP_0, 
    PROP_CONTEXT, 
    PROP_LOCATION 
};

static void gst_plugin_projectm_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec) {
  GstPluginProjectM *plugin = GST_PLUGIN_PROJECTM(object);

  GST_DEBUG_OBJECT(plugin, "set_property");

  switch (property_id) {
    case PROP_LOCATION:
      plugin->preset = g_strdup(g_value_get_string(value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
      break;
  }
}

static void gst_plugin_projectm_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec) {
  GstPluginProjectM *plugin = GST_PLUGIN_PROJECTM(object);

  GST_DEBUG_OBJECT(plugin, "get_property");

  switch (property_id) {
    case PROP_LOCATION:
      g_value_set_string(value, plugin->preset);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
      break;
  }
}

static void gst_plugin_projectm_finalize(GObject *object) {
  G_OBJECT_CLASS(gst_plugin_projectm_parent_class)->finalize(object);
}

static void gst_plugin_projectm_init(GstPluginProjectM *plugin) {
//   gst_plugin_projectm_class_init(gst_plugin_projectm_parent_class);
}

static void gst_plugin_projectm_class_init(GstPluginProjectMClass *klass) {
  GObjectClass *gobject_class = (GObjectClass *) klass;
  GstElementClass *gstelement_class = (GstElementClass *) klass;
  GstAudioVisualizerClass *scope_class = (GstAudioVisualizerClass *) klass;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;
//   gst_caps_init(klass);
  
  gobject_class->set_property = gst_plugin_projectm_set_property;
  gobject_class->get_property = gst_plugin_projectm_get_property;

  gst_element_class_set_static_metadata(GST_ELEMENT_CLASS(klass), "ProjectM Visualizer",
                                        "Visualization",
                                        "A plugin to visualize audio using the ProjectM library",
                                        "AnomieVision <>");
  
  g_object_class_install_property(
      gobject_class, PROP_LOCATION,
      g_param_spec_string("preset", "Preset file Location",
                          "Location of the MilkDrop preset",
                          "/home/tristan/dev/gst/trido/presets/Supernova/"
                          "Shimmer/EoS - starburst 05 phasing.milk",
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  gobject_class->finalize = gst_plugin_projectm_finalize;

  // scope_class->setup = GST_DEBUG_FUNCPTR(gst_projectm_setup);
  // scope_class->render = GST_DEBUG_FUNCPTR(gst_projectm_render);
}

static gboolean plugin_init(GstPlugin *plugin) {
  GST_DEBUG_CATEGORY_INIT(gst_plugin_projectm_debug, "projectm", 0,
                          "projectm audio visualisations");

  gst_element_register(plugin, "projectm", GST_RANK_NONE, GST_TYPE_PLUGIN_PROJECTM);

  return TRUE;
}

#ifndef PACKAGE
#define PACKAGE "GstProjectM"
#endif
#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "0.0.1"
#endif
#ifndef PACKAGE_LICENSE
#define PACKAGE_LICENSE "LGPL"
#endif
#ifndef PACKAGE_ORIGIN
#define PACKAGE_ORIGIN "http://FIXME.org/"
#endif

GST_PLUGIN_DEFINE(GST_VERSION_MAJOR, GST_VERSION_MINOR, projectm,
                  "plugin to visualize audio using the ProjectM library", plugin_init,
                  PACKAGE_VERSION, PACKAGE_LICENSE, PACKAGE, PACKAGE_ORIGIN)
