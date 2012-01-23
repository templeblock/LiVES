// effects.h
// LiVES (lives-exe)
// (c) G. Finch 2003 - 2012
// Released under the GPL 3 or later
// see file ../COPYING for licensing details

#ifndef HAS_LIVES_EFFECTS_H
#define HAS_LIVES_EFFECTS_H

#if HAVE_SYSTEM_WEED
#include <weed/weed.h>
#else
#include "../libweed/weed.h"
#endif

// general effects
typedef enum {
  LIVES_FX_CAT_NONE=0,
  LIVES_FX_CAT_GENERATOR,
  LIVES_FX_CAT_DATA_GENERATOR,
  LIVES_FX_CAT_TRANSITION,
  LIVES_FX_CAT_AV_TRANSITION,
  LIVES_FX_CAT_VIDEO_TRANSITION,
  LIVES_FX_CAT_AUDIO_TRANSITION,
  LIVES_FX_CAT_EFFECT,
  LIVES_FX_CAT_AUDIO_EFFECT,
  LIVES_FX_CAT_UTILITY,
  LIVES_FX_CAT_COMPOSITOR,
  LIVES_FX_CAT_AUDIO_MIXER,
  LIVES_FX_CAT_TAP,
  LIVES_FX_CAT_SPLITTER,
  LIVES_FX_CAT_CONVERTER,
  LIVES_FX_CAT_AUDIO_VOL,
  LIVES_FX_CAT_ANALYSER,
  LIVES_FX_CAT_VIDEO_ANALYSER,
  LIVES_FX_CAT_AUDIO_ANALYSER
} lives_fx_cat_t;


gchar *lives_fx_cat_to_text(lives_fx_cat_t cat, gboolean plural);


#include "effects-weed.h"

gboolean do_effect(lives_rfx_t *rfx, gboolean is_preview); ///< defined as extern in paramwindow.c

void on_render_fx_activate (GtkMenuItem *menuitem, lives_rfx_t *rfx);

///////////////// real time effects

// render
void on_realfx_activate (GtkMenuItem *, gpointer rfx);
gboolean on_realfx_activate_inner(gint type, lives_rfx_t *rfx);

lives_render_error_t realfx_progress (gboolean reset);

// key callbacks

gboolean textparm_callback (GtkAccelGroup *group, GObject *obj, guint keyval, GdkModifierType mod, gpointer user_data);

gboolean grabkeys_callback (GtkAccelGroup *, GObject *, guint, GdkModifierType, gpointer user_data); ///< for accel groups
gboolean grabkeys_callback_hook (GtkToggleButton *button, gpointer user_data); ///< for widgets

gboolean rte_on_off_callback (GtkAccelGroup *, GObject *, guint, GdkModifierType, gpointer user_data); ///< for accel groups
gboolean rte_on_off_callback_hook (GtkToggleButton *, gpointer user_data); ///< for widgets

gboolean rtemode_callback (GtkAccelGroup *, GObject *, guint, GdkModifierType, gpointer user_data); ///< for accel groups
gboolean rtemode_callback_hook (GtkToggleButton *, gpointer user_data); ///< for widgets

gboolean swap_fg_bg_callback (GtkAccelGroup *, GObject *, guint, GdkModifierType, gpointer user_data);

weed_plant_t *get_blend_layer(weed_timecode_t tc);

weed_plant_t *on_rte_apply (weed_plant_t *main_layer, int opwidth, int opheight, weed_timecode_t tc);


void deinterlace_frame(weed_plant_t *layer, weed_timecode_t tc);


#endif
