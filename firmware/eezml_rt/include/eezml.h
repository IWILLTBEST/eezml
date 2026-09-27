/**
 * @file eezml.h
 * @brief eezml (uixml) runtime loader — portable core.
 *
 * Parses an in-memory eezml/uixml document and builds the LVGL object tree.
 * Platform agnostic: no OS, no file system. The caller decides where the
 * XML text comes from (embedded array, file system, network...).
 */

#ifndef EEZML_H
#define EEZML_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

/** Create the object tree described by an eezml document.
 *
 * @param parent  parent object (pass the active screen or any container)
 * @param xml     NUL-terminated eezml document text
 * @return        the created root object of the document, or NULL on error
 */
lv_obj_t * eezml_create(lv_obj_t * parent, const char * xml);

/** Register a named bitmap/GIF so that XML src="name" can reference it. */
void eezml_register_bitmap(const char * name, const lv_image_dsc_t * dsc);

/** Register a named font so that XML font="name" can reference it. */
void eezml_register_font(const char * name, const lv_font_t * font);

/** Look up a created object by its XML id attribute (per-document scope). */
lv_obj_t * eezml_get_object(const char * id);

/* ------------------------------------------------------------------ */
/* Phase 2: variables, actions, bindings, native actions               */
/* ------------------------------------------------------------------ */

/** Variable access from C business code (drives `bind="..."` UI updates).
 *  String variables are copied into the internal buffer. */
int32_t     eezml_get_var_int(const char * name);
void        eezml_set_var_int(const char * name, int32_t value);
void        eezml_set_var_string(const char * name, const char * value);

/** Trigger a declarative <action name="..."> from C. */
void        eezml_run_action(const char * name);

/** Native (C) action registry: XML <call native="name"/> lands here. */
typedef void (* eezml_native_fn)(void * user_data);
void        eezml_register_native(const char * name, eezml_native_fn fn,
                                  void * user_data);

/** Screen-change hook: <change-screen target="..."/> calls this handler.
 *  The platform decides how screens are organized and switched. */
typedef void (* eezml_change_screen_fn)(const char * target, void * user_data);
void        eezml_set_change_screen_handler(eezml_change_screen_fn fn,
                                            void * user_data);

#ifdef __cplusplus
}
#endif

#endif /* EEZML_H */
