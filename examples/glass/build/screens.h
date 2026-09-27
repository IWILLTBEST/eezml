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
    lv_obj_t *panel_bg;
    lv_obj_t *label_title;
    lv_obj_t *label_subtitle;
    lv_obj_t *panel_card_speed;
    lv_obj_t *label_icon_speed;
    lv_obj_t *label_title_speed;
    lv_obj_t *label_val_speed;
    lv_obj_t *label_unit_speed;
    lv_obj_t *panel_card_temp;
    lv_obj_t *label_icon_temp;
    lv_obj_t *label_title_temp;
    lv_obj_t *label_val_temp;
    lv_obj_t *label_unit_temp;
    lv_obj_t *panel_card_volt;
    lv_obj_t *label_icon_volt;
    lv_obj_t *label_title_volt;
    lv_obj_t *label_val_volt;
    lv_obj_t *label_unit_volt;
    lv_obj_t *panel_card_curr;
    lv_obj_t *label_icon_curr;
    lv_obj_t *label_title_curr;
    lv_obj_t *label_val_curr;
    lv_obj_t *label_unit_curr;
    lv_obj_t *panel_gauge;
    lv_obj_t *label_gauge_title;
    lv_obj_t *arc_power;
    lv_obj_t *label_val_power;
    lv_obj_t *label_unit_power;
    lv_obj_t *label_gauge_hint;
    lv_obj_t *panel_status;
    lv_obj_t *label_status_title;
    lv_obj_t *led_heart;
    lv_obj_t *label_lbl_heart;
    lv_obj_t *led_warn;
    lv_obj_t *label_lbl_warn;
    lv_obj_t *led_err;
    lv_obj_t *label_lbl_err;
    lv_obj_t *label_lbl_breath;
    lv_obj_t *panel_action;
    lv_obj_t *label_action_title;
    lv_obj_t *button_replay;
    lv_obj_t *label_action_hint;
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