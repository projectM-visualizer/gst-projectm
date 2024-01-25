#ifndef __GST_PROJECTM_H__
#define __GST_PROJECTM_H__

#include <gst/gst.h>
#include <gst/gl/gl.h>
#include <gst/pbutils/gstaudiovisualizer.h>

#include <projectM-4/projectM.h>

G_BEGIN_DECLS

#define GST_TYPE_PROJECTM (gst_projectm_get_type())
G_DECLARE_FINAL_TYPE(GstProjectM, gst_projectm, GST,
                     PROJECTM, GstAudioVisualizer)

struct _GstProjectM
{
  GstAudioVisualizer element;

  GstGLDisplay *display;
  GstGLWindow *window;
  GstGLContext *context;

  projectm_handle handle;
  uint8_t *framebuffer;
  gchar *preset;
  gint texture;
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

static void gst_projectm_class_init(GstProjectMClass *klass);

static gboolean plugin_init(GstPlugin *plugin);

static void check_gl_error(GstGLContext *context, gpointer data);

static void projectm_init_instance(GstProjectM *plugin);

static gboolean projectm_setup(GstAudioVisualizer *bscope);

static gboolean projectm_render(GstAudioVisualizer *bscope, GstBuffer *audio, GstVideoFrame *video);

G_END_DECLS

#endif /* __GST_PROJECTM_H__ */