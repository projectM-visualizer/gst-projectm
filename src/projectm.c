#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>

#include <projectM-4/playlist.h>
#include <projectM-4/projectM.h>

#include "plugin.h"
#include "projectm.h"

GST_DEBUG_CATEGORY_STATIC(projectm_debug);
#define GST_CAT_DEFAULT projectm_debug

projectm_handle projectm_init(GstProjectM *plugin) {
  projectm_handle handle = NULL;
  projectm_playlist_handle playlist = NULL;

  GST_DEBUG_CATEGORY_INIT(projectm_debug, "projectm", 0, "ProjectM");

  GstAudioVisualizer *bscope = GST_AUDIO_VISUALIZER(plugin);

  // Create ProjectM instance
  GST_DEBUG_OBJECT(plugin, "Creating projectM instance..");
  handle = projectm_create();

  if (!handle) {
    GST_DEBUG_OBJECT(
        plugin,
        "project_create() returned NULL, projectM instance was not created!");
    return NULL;
  } else {
    GST_DEBUG_OBJECT(plugin, "Created projectM instance!");
  }

  if (plugin->enable_playlist) {
    GST_DEBUG_OBJECT(plugin, "Playlist enabled");

    // initialize preset playlist
    playlist = projectm_playlist_create(handle);
    projectm_playlist_set_shuffle(playlist, plugin->shuffle_presets);
    // projectm_playlist_set_preset_switched_event_callback(_playlist,
    // &ProjectMWrapper::PresetSwitchedEvent, static_cast<void*>(this));
  } else {
    GST_DEBUG_OBJECT(plugin, "Playlist disabled");
  }

  // Log properties
  GST_INFO_OBJECT(
      plugin,
      "Using Properties: "
      "preset=%s, "
      "texture-dir=%s, "
      "beat-sensitivity=%f, "
      "hard-cut-duration=%f, "
      "hard-cut-enabled=%d, "
      "hard-cut-sensitivity=%f, "
      "soft-cut-duration=%f, "
      "preset-duration=%f, "
      "mesh-size=(%lu, %lu)"
      "aspect-correction=%d, "
      "easter-egg=%f, "
      "preset-locked=%d, "
      "enable-playlist=%d, "
      "shuffle-presets=%d",
      plugin->preset_path, plugin->texture_dir_path, plugin->beat_sensitivity,
      plugin->hard_cut_duration, plugin->hard_cut_enabled,
      plugin->hard_cut_sensitivity, plugin->soft_cut_duration,
      plugin->preset_duration, plugin->mesh_width, plugin->mesh_height,
      plugin->aspect_correction, plugin->easter_egg, plugin->preset_locked,
      plugin->enable_playlist, plugin->shuffle_presets);

  // Load preset file if path is provided
  if (plugin->preset_path != NULL) {
    int added_count =
        projectm_playlist_add_path(playlist, plugin->preset_path, true, false);
    GST_INFO("Loaded preset path: %s, presets found: %d", plugin->preset_path,
             added_count);
  }

  // Set texture search path if directory path is provided
  if (plugin->texture_dir_path != NULL) {
    const gchar *texturePaths[1] = {plugin->texture_dir_path};
    projectm_set_texture_search_paths(handle, texturePaths, 1);
  }

  // Set properties
  projectm_set_beat_sensitivity(handle, plugin->beat_sensitivity);
  projectm_set_hard_cut_duration(handle, plugin->hard_cut_duration);
  projectm_set_hard_cut_enabled(handle, plugin->hard_cut_enabled);
  projectm_set_hard_cut_sensitivity(handle, plugin->hard_cut_sensitivity);
  projectm_set_soft_cut_duration(handle, plugin->soft_cut_duration);

  // Set preset duration, or set to in infinite duration if zero
  if (plugin->preset_duration > 0.0) {
    projectm_set_preset_duration(handle, plugin->preset_duration);

    // kick off the first preset
    if (projectm_playlist_size(playlist) > 1 && !plugin->preset_locked) {
      projectm_playlist_play_next(playlist, true);
    }
  } else {
    projectm_set_preset_duration(handle, 999999.0);
  }

  projectm_set_mesh_size(handle, plugin->mesh_width, plugin->mesh_height);
  projectm_set_aspect_correction(handle, plugin->aspect_correction);
  projectm_set_easter_egg(handle, plugin->easter_egg);
  projectm_set_preset_locked(handle, plugin->preset_locked);

  projectm_set_fps(handle, GST_VIDEO_INFO_FPS_N(&bscope->vinfo));
  projectm_set_window_size(handle, GST_VIDEO_INFO_WIDTH(&bscope->vinfo),
                           GST_VIDEO_INFO_HEIGHT(&bscope->vinfo));

  return handle;
}

// void projectm_render(GstProjectM *plugin, gint16 *samples, gint sample_count)
// {
//     GST_DEBUG_OBJECT(plugin, "Rendering %d samples", sample_count);

//     projectm_pcm_add_int16(plugin->handle, samples, sample_count,
//     PROJECTM_STEREO);

//     projectm_opengl_render_frame(plugin->handle);
// }
