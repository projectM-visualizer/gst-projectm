#ifndef __GST_PROJECTM_H__
#define __GST_PROJECTM_H__

#include <gst/gst.h>
#include "gstglbaseaudiovisualizer.h"

typedef struct _GstProjectMPrivate GstProjectMPrivate;

G_BEGIN_DECLS

#define GST_TYPE_PROJECTM (gst_projectm_get_type())
G_DECLARE_FINAL_TYPE(GstProjectM, gst_projectm, GST,
                     PROJECTM, GstGLBaseAudioVisualizer)

struct _GstProjectM
{
  GstGLBaseAudioVisualizer element;

  gchar *preset_path;
  gchar *texture_dir_path;

  gfloat beat_sensitivity;
  gdouble hard_cut_duration;
  gboolean hard_cut_enabled;
  gfloat hard_cut_sensitivity;
  gdouble soft_cut_duration;
  gdouble preset_duration;
  gulong mesh_width;
  gulong mesh_height;
  gboolean aspect_correction;
  gfloat easter_egg;
  gboolean preset_locked;
  gboolean shuffle_presets;

  GstProjectMPrivate *priv;
};

struct _GstProjectMClass
{
  GstAudioVisualizerClass parent_class;
};

static void gst_projectm_set_property(GObject *object, guint prop_id,
                                      const GValue *value,
                                      GParamSpec *pspec);

static void gst_projectm_get_property(GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec);

static void gst_projectm_init(GstProjectM *plugin);

static void gst_projectm_finalize(GObject *object);

static gboolean gst_projectm_gl_start(GstGLBaseAudioVisualizer *glav);

static void gst_projectm_gl_stop(GstGLBaseAudioVisualizer *glav);

static gboolean gst_projectm_render(GstGLBaseAudioVisualizer *glav, GstBuffer *audio, GstVideoFrame *video);

static void gst_projectm_class_init(GstProjectMClass *klass);

static gboolean plugin_init(GstPlugin *plugin);

static gboolean gst_projectm_setup(GstGLBaseAudioVisualizer *glav);

G_END_DECLS

#endif /* __GST_PROJECTM_H__ */