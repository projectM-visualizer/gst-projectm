/*
 * GStreamer
 * Copyright (C) 2024 AnomieVision <>
 */

#ifndef __GST_PLUGIN_ENUMS_H__
#define __GST_PLUGIN_ENUMS_H__

#include <glib.h>

G_BEGIN_DECLS

/* 
 * Filter signals and args
 */
enum
{
  LAST_SIGNAL
};

/* 
 * Properties 
 */
enum { 
    PROP_0, 
    PROP_CONTEXT, 
    PROP_LOCATION 
};

G_END_DECLS

#endif /* __GST_PLUGIN_ENUMS_H__ */