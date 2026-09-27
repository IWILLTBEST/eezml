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

#ifdef __cplusplus
}
#endif

#endif /* EEZML_H */
