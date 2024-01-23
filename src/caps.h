
/*
 * GStreamer
 * Copyright (C) 2024 AnomieVision <>
 */

#ifndef __GST_PLUGIN_CAPS_H__
#define __GST_PLUGIN_CAPS_H__

#include <glib.h>

#include "plugin.h"

G_BEGIN_DECLS

const gchar* get_audio_sink_cap(unsigned int type);

const gchar* get_video_src_cap(unsigned int type);

gboolean gst_caps_init(GstPluginProjectMClass *klass);

G_END_DECLS

#endif /* __GST_PLUGIN_CAPS_H__ */