#ifndef __GST_PROJECTM_CAPS_H__
#define __GST_PROJECTM_CAPS_H__

#include <glib.h>

#include "plugin.h"

G_BEGIN_DECLS

/**
 * @brief Get audio sink caps based on the given type.
 *
 * @param type - The type of audio caps to retrieve.
 * @return The audio caps format string.
 */
const gchar *get_audio_sink_cap(unsigned int type);

/**
 * Get video source caps based on the given type.
 *
 * @param type - The type of video caps to retrieve.
 * @return The video caps format string.
 */
const gchar *get_video_src_cap(unsigned int type);

G_END_DECLS

#endif /* __GST_PROJECTM_CAPS_H__ */