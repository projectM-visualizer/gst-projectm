#ifndef __GST_PROJECTM_CONFIG_H__
#define __GST_PROJECTM_CONFIG_H__

#include <glib.h>

G_BEGIN_DECLS

/**
 * @brief Plugin Details
 */

#define PACKAGE "GstProjectMOne"
#define PACKAGE_NAME "GstProjectM"
#define PACKAGE_VERSION "0.0.1"
#define PACKAGE_LICENSE "LGPL"
#define PACKAGE_ORIGIN "https://github.com/anomievision/gst-plugin-projectm"

/**
 * @brief ProjectM Settings (defaults)
 */

#define DEFAULT_PRESET_PATH NULL
#define DEFAULT_TEXTURE_DIR_PATH NULL
#define DEFAULT_BEAT_SENSITIVITY 1.0
#define DEFAULT_HARD_CUT_DURATION 3.0
#define DEFAULT_HARD_CUT_ENABLED FALSE
#define DEFAULT_HARD_CUT_SENSITIVITY 1.0
#define DEFAULT_SOFT_CUT_DURATION 3.0
#define DEFAULT_PRESET_DURATION 0.0
#define DEFAULT_MESH_SIZE "48,32"
#define DEFAULT_ASPECT_CORRECTION TRUE
#define DEFAULT_EASTER_EGG 0.0
#define DEFAULT_PRESET_LOCKED TRUE

G_END_DECLS

#endif /* __GST_PROJECTM_CONFIG_H__ */