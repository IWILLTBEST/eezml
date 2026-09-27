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
    SCREEN_ID_CONTROLS = 2,
    SCREEN_ID_SETTINGS = 3,
    _SCREEN_ID_LAST = 3
};

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *controls;
    lv_obj_t *settings;
    lv_obj_t *panel_root;
    lv_obj_t *label_title;
    lv_obj_t *label_lbl_mode;
    lv_obj_t *roller_mode;
    lv_obj_t *label_lbl_chart;
    lv_obj_t *chart_bus;
    lv_obj_t *label_lbl_tbl;
    lv_obj_t *table_events;
    lv_obj_t *label_hint;
    lv_obj_t *panel_root2;
    lv_obj_t *label_title2;
    lv_obj_t *label_lbl_scale;
    lv_obj_t *scale_rpm;
    lv_obj_t *calendar_cal;
    lv_obj_t *label_lbl_count;
    lv_obj_t *spinbox_count;
    lv_obj_t *textarea_input;
    lv_obj_t *keyboard_kb;
    lv_obj_t *panel_root3;
    lv_obj_t *tabview_cfg;
    lv_obj_t *label_t1a;
    lv_obj_t *slider_bright;
    lv_obj_t *label_t1b;
    lv_obj_t *label_t2a;
    lv_obj_t *label_t2b;
    lv_obj_t *switch_dhcp;
} objects_t;

extern objects_t objects;

typedef struct {
    lv_scale_section_t *scale_section;
    lv_scale_section_t *scale_section1;
    lv_scale_section_t *scale_section2;
} screen_controls_state_t;

extern screen_controls_state_t screen_controls_state;

void create_screen_main();
void tick_screen_main();

void create_screen_controls();
void tick_screen_controls();

void create_screen_settings();
void tick_screen_settings();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

// Global state variables

extern lv_style_t scale_rpm_section_main_style;
extern lv_style_t scale_rpm_section_main_style1;
extern lv_style_t scale_rpm_section_main_style2;

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/