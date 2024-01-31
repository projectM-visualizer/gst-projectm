#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>

#include "projectm.h"
#include "plugin.h"

GST_DEBUG_CATEGORY_STATIC(projectm_debug);
#define GST_CAT_DEFAULT projectm_debug

void projectm_init(GstProjectM *plugin)
{
    GST_DEBUG_CATEGORY_INIT(projectm_debug, "projectm",
                            0, "ProjectM");

    // Create ProjectM instance
    plugin->handle = projectm_create();

    if (!plugin->handle)
    {
        GST_INFO("Could not create instance");
    }
    else
    {
        GST_INFO("Created instance!");
    }

    // Log properties
    GST_INFO_OBJECT(plugin, "Using Properties: "
                            "preset=%s, "
                            "texture-dir=%s, "
                            "beat-sensitivity=%f, "
                            "hard-cut-duration=%f, "
                            "hard-cut-enabled=%d, "
                            "hard-cut-sensitivity=%f, "
                            "soft-cut-duration=%f, "
                            "preset-duration=%f, "
                            "mesh-size=(%d, %d)"
                            "aspect-correction=%d, "
                            "easter-egg=%f, "
                            "preset-locked=%d, ",
                    plugin->preset_path,
                    plugin->texture_dir_path,
                    plugin->beat_sensitivity,
                    plugin->hard_cut_duration,
                    plugin->hard_cut_enabled,
                    plugin->hard_cut_sensitivity,
                    plugin->soft_cut_duration,
                    plugin->preset_duration,
                    plugin->mesh_width,
                    plugin->mesh_height,
                    plugin->aspect_correction,
                    plugin->easter_egg,
                    plugin->preset_locked);

    // Load preset file if path is provided
    if (plugin->preset_path != NULL)
        projectm_load_preset_file(plugin->handle, plugin->preset_path, false);

    // Set texture search path if directory path is provided
    if (plugin->texture_dir_path != NULL)
    {
        const gchar *texturePaths[1] = {plugin->texture_dir_path};
        projectm_set_texture_search_paths(plugin->handle, texturePaths, 1);
    }

    // Set properties
    projectm_set_beat_sensitivity(plugin->handle, plugin->beat_sensitivity);
    projectm_set_hard_cut_duration(plugin->handle, plugin->hard_cut_duration);
    projectm_set_hard_cut_enabled(plugin->handle, plugin->hard_cut_enabled);
    projectm_set_hard_cut_sensitivity(plugin->handle, plugin->hard_cut_sensitivity);
    projectm_set_soft_cut_duration(plugin->handle, plugin->soft_cut_duration);

    // Set preset duration, or set to in infinite duration if zero
    if (plugin->preset_duration > 0.0)
    {
        projectm_set_preset_duration(plugin->handle, plugin->preset_duration);
    }
    else
    {
        projectm_set_preset_duration(plugin->handle, 999999.0);
    }

    projectm_set_mesh_size(plugin->handle, plugin->mesh_width, plugin->mesh_height);
    projectm_set_aspect_correction(plugin->handle, plugin->aspect_correction);
    projectm_set_easter_egg(plugin->handle, plugin->easter_egg);
    projectm_set_preset_locked(plugin->handle, plugin->preset_locked);

    projectm_set_fps(plugin->handle, plugin->fps);
    projectm_set_window_size(plugin->handle, plugin->window_width, plugin->window_height);
}