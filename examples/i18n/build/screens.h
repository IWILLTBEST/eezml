#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_MAIN = 1,
    _SCREEN_ID_LAST = 1
};

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *panel_root;
    lv_obj_t *label_title;
    lv_obj_t *label_row_speed;
    lv_obj_t *label_val_speed;
    lv_obj_t *label_row_temp;
    lv_obj_t *label_status;
    lv_obj_t *button_btn_start;
    lv_obj_t *label_btn_start_lbl;
    lv_obj_t *button_btn_stop;
    lv_obj_t *label_btn_stop_lbl;
} objects_t;

extern objects_t objects;

void create_screen_main();
void tick_screen_main();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/