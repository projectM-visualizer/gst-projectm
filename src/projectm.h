#ifndef __PROJECTM_H__
#define __PROJECTM_H__

#include <glib.h>

#include "plugin.h"
#include <projectM-4/projectM.h>

G_BEGIN_DECLS

/**
 * @brief Initialize ProjectM
 */
projectm_handle projectm_init(GstProjectM *plugin);

/**
 * @brief Render ProjectM
 */
// void projectm_render(GstProjectM *plugin, gint16 *samples, gint sample_count);

G_END_DECLS

#endif /* __PROJECTM_H__ */