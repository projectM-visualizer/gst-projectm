#ifndef __GST_PROJECTM_ENUMS_H__
#define __GST_PROJECTM_ENUMS_H__

#include <glib.h>

G_BEGIN_DECLS

/**
 * @brief Properties
 */

enum {
  PROP_0,
  PROP_PRESET_PATH,
  PROP_TEXTURE_DIR_PATH,
  PROP_BEAT_SENSITIVITY,
  PROP_HARD_CUT_DURATION,
  PROP_HARD_CUT_ENABLED,
  PROP_HARD_CUT_SENSITIVITY,
  PROP_SOFT_CUT_DURATION,
  PROP_PRESET_DURATION,
  PROP_MESH_SIZE,
  PROP_ASPECT_CORRECTION,
  PROP_EASTER_EGG,
  PROP_PRESET_LOCKED,
  PROP_SHUFFLE_PRESETS,
  PROP_ENABLE_PLAYLIST
};

G_END_DECLS

#endif /* __GST_PROJECTM_ENUMS_H__ */
