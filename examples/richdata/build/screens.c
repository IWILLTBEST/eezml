#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;

static const char *screen_names[] = { "main", "controls", "settings" };
static const char *object_names[] = { "main", "controls", "settings", "panel_root", "label_title", "label_lbl_mode", "roller_mode", "label_lbl_chart", "chart_bus", "label_lbl_tbl", "table_events", "label_hint", "panel_root2", "label_title2", "label_lbl_scale", "scale_rpm", "calendar_cal", "label_lbl_count", "spinbox_count", "textarea_input", "keyboard_kb", "panel_root3", "tabview_cfg", "label_t1a", "slider_bright", "label_t1b", "label_t2a", "label_t2b", "switch_dhcp" };

screen_controls_state_t screen_controls_state;

// Global state variables

lv_style_t scale_rpm_section_main_style;
static bool scale_rpm_section_main_style_initialized;
lv_style_t scale_rpm_section_main_style1;
static bool scale_rpm_section_main_style1_initialized;
lv_style_t scale_rpm_section_main_style2;
static bool scale_rpm_section_main_style2_initialized;

//
// Event handlers
//

lv_obj_t *tick_value_change_obj;

static void event_handler_cb_main_roller_mode(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target_obj(e);
        if (tick_value_change_obj != ta) {
            int32_t value = lv_roller_get_selected(ta);
            assignIntegerProperty(flowState, 4, 3, value, "Failed to assign Selected in Roller widget");
        }
    }
    if (event == LV_EVENT_VALUE_CHANGED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, -1, 3, e);
    }
}

static void event_handler_cb_controls_spinbox_count(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target_obj(e);
        if (tick_value_change_obj != ta) {
            int32_t value = lv_spinbox_get_value(ta);
            assignIntegerProperty(flowState, 7, 3, value, "Failed to assign Value in Spinbox widget");
        }
    }
}

static void event_handler_cb_settings_tabview_cfg(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target_obj(e);
        if (tick_value_change_obj != ta) {
            int32_t value = lv_tabview_get_tab_active(ta);
            assignIntegerProperty(flowState, 2, 3, value, "Failed to assign Active tab in Tabview widget");
        }
    }
    if (event == LV_EVENT_VALUE_CHANGED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, -1, 3, e);
    }
}

//
// Screens
//

void create_screen_main() {
    void *flowState = getFlowState(0, 0);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 640);
    {
        lv_obj_t *parent_obj = obj;
        {
            // panel_root
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_root = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 480, 640);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x101828), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_title
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_title = obj;
                    lv_obj_set_pos(obj, 24, 20);
                    lv_obj_set_size(obj, 300, 26);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xe8effa), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Rich Data Demo");
                }
                {
                    // label_lbl_mode
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_lbl_mode = obj;
                    lv_obj_set_pos(obj, 24, 64);
                    lv_obj_set_size(obj, 226, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x8fa0bc), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Control mode (roller)");
                }
                {
                    // roller_mode
                    lv_obj_t *obj = lv_roller_create(parent_obj);
                    objects.roller_mode = obj;
                    lv_obj_set_pos(obj, 24, 90);
                    lv_obj_set_size(obj, 170, 92);
                    lv_roller_set_options(obj, "Auto\nManual\nService\nBootstrap", LV_ROLLER_MODE_NORMAL);
                    lv_obj_add_event_cb(obj, event_handler_cb_main_roller_mode, LV_EVENT_ALL, flowState);
                }
                {
                    // label_lbl_chart
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_lbl_chart = obj;
                    lv_obj_set_pos(obj, 24, 200);
                    lv_obj_set_size(obj, 300, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x8fa0bc), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Bus current (chart)");
                }
                {
                    // chart_bus
                    lv_obj_t *obj = lv_chart_create(parent_obj);
                    objects.chart_bus = obj;
                    lv_obj_set_pos(obj, 24, 226);
                    lv_obj_set_size(obj, 432, 160);
                }
                {
                    // label_lbl_tbl
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_lbl_tbl = obj;
                    lv_obj_set_pos(obj, 24, 404);
                    lv_obj_set_size(obj, 300, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x8fa0bc), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Event log (table)");
                }
                {
                    // table_events
                    lv_obj_t *obj = lv_table_create(parent_obj);
                    objects.table_events = obj;
                    lv_obj_set_pos(obj, 24, 430);
                    lv_obj_set_size(obj, 432, 150);
                }
                {
                    // label_hint
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_hint = obj;
                    lv_obj_set_pos(obj, 24, 600);
                    lv_obj_set_size(obj, 466, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x5a6a86), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "chart/table structure: runtime C via ui_ext.h");
                }
            }
        }
    }
    
    tick_screen_main();
}

void tick_screen_main() {
    void *flowState = getFlowState(0, 0);
    (void)flowState;
    {
        if (!(lv_obj_get_state(objects.roller_mode) & LV_STATE_EDITED)) {
            int32_t new_val = evalIntegerProperty(flowState, 4, 3, "Failed to evaluate Selected in Roller widget");
            int32_t cur_val = lv_roller_get_selected(objects.roller_mode);
            if (new_val != cur_val) {
                tick_value_change_obj = objects.roller_mode;
                lv_roller_set_selected(objects.roller_mode, new_val, LV_ANIM_OFF);
                tick_value_change_obj = NULL;
            }
        }
    }
}

void create_screen_controls() {
    screen_controls_state_t *state = &screen_controls_state;
    (void)state;
    void *flowState = getFlowState(0, 1);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.controls = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 640);
    {
        lv_obj_t *parent_obj = obj;
        {
            // panel_root2
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_root2 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 480, 640);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x101828), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_title2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_title2 = obj;
                    lv_obj_set_pos(obj, 24, 16);
                    lv_obj_set_size(obj, 300, 26);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xe8effa), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Controls Demo");
                }
                {
                    // label_lbl_scale
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_lbl_scale = obj;
                    lv_obj_set_pos(obj, 24, 52);
                    lv_obj_set_size(obj, 200, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x8fa0bc), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "RPM scale");
                }
                {
                    // scale_rpm
                    lv_obj_t *obj = lv_scale_create(parent_obj);
                    objects.scale_rpm = obj;
                    lv_obj_set_pos(obj, 24, 74);
                    lv_obj_set_size(obj, 180, 180);
                    lv_scale_set_mode(obj, LV_SCALE_MODE_ROUND_INNER);
                    lv_scale_set_range(obj, 0, 3000);
                    lv_scale_set_angle_range(obj, 270);
                    lv_scale_set_rotation(obj, 135);
                    lv_scale_set_total_tick_count(obj, 11);
                    lv_scale_set_major_tick_every(obj, 5);
                    lv_scale_set_label_show(obj, true);
                    {
                        state->scale_section = lv_scale_add_section(obj);
                        lv_scale_section_set_range(state->scale_section, 0, 2200);
                        {
                            if (!scale_rpm_section_main_style_initialized) {
                                lv_style_init(&scale_rpm_section_main_style);
                                scale_rpm_section_main_style_initialized = true;
                                lv_style_set_arc_width(&scale_rpm_section_main_style, 8);
                                lv_style_set_arc_color(&scale_rpm_section_main_style, lv_color_hex(0x3a4b66));
                            }
                            lv_scale_set_section_style_main(obj, state->scale_section, &scale_rpm_section_main_style);
                        }
                    }
                    {
                        state->scale_section1 = lv_scale_add_section(obj);
                        lv_scale_section_set_range(state->scale_section1, 2200, 2600);
                        {
                            if (!scale_rpm_section_main_style1_initialized) {
                                lv_style_init(&scale_rpm_section_main_style1);
                                scale_rpm_section_main_style1_initialized = true;
                                lv_style_set_arc_width(&scale_rpm_section_main_style1, 8);
                                lv_style_set_arc_color(&scale_rpm_section_main_style1, lv_color_hex(0xf2b84b));
                            }
                            lv_scale_set_section_style_main(obj, state->scale_section1, &scale_rpm_section_main_style1);
                        }
                    }
                    {
                        state->scale_section2 = lv_scale_add_section(obj);
                        lv_scale_section_set_range(state->scale_section2, 2600, 3000);
                        {
                            if (!scale_rpm_section_main_style2_initialized) {
                                lv_style_init(&scale_rpm_section_main_style2);
                                scale_rpm_section_main_style2_initialized = true;
                                lv_style_set_arc_width(&scale_rpm_section_main_style2, 8);
                                lv_style_set_arc_color(&scale_rpm_section_main_style2, lv_color_hex(0xe5484d));
                            }
                            lv_scale_set_section_style_main(obj, state->scale_section2, &scale_rpm_section_main_style2);
                        }
                    }
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // calendar_cal
                    lv_obj_t *obj = lv_calendar_create(parent_obj);
                    objects.calendar_cal = obj;
                    lv_obj_set_pos(obj, 240, 74);
                    lv_obj_set_size(obj, 210, 240);
                    lv_calendar_add_header_arrow(obj);
                    lv_calendar_set_today_date(obj, 2026, 9, 1);
                    lv_calendar_set_month_shown(obj, 2026, 9);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                }
                {
                    // label_lbl_count
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_lbl_count = obj;
                    lv_obj_set_pos(obj, 24, 286);
                    lv_obj_set_size(obj, 200, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x8fa0bc), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Pulse count");
                }
                {
                    // spinbox_count
                    lv_obj_t *obj = lv_spinbox_create(parent_obj);
                    objects.spinbox_count = obj;
                    lv_obj_set_pos(obj, 24, 308);
                    lv_obj_set_size(obj, 130, 46);
                    lv_spinbox_set_digit_format(obj, 4, 0);
                    lv_spinbox_set_range(obj, 0, 9999);
                    lv_spinbox_set_rollover(obj, false);
                    lv_spinbox_set_step(obj, 1);
                    lv_obj_add_event_cb(obj, event_handler_cb_controls_spinbox_count, LV_EVENT_ALL, flowState);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                }
                {
                    // textarea_input
                    lv_obj_t *obj = lv_textarea_create(parent_obj);
                    objects.textarea_input = obj;
                    lv_obj_set_pos(obj, 24, 374);
                    lv_obj_set_size(obj, 432, 44);
                    lv_textarea_set_max_length(obj, 128);
                    lv_textarea_set_one_line(obj, true);
                    lv_textarea_set_password_mode(obj, false);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                }
                {
                    // keyboard_kb
                    lv_obj_t *obj = lv_keyboard_create(parent_obj);
                    objects.keyboard_kb = obj;
                    lv_obj_set_pos(obj, 24, 426);
                    lv_obj_set_size(obj, 432, 190);
                    lv_keyboard_set_mode(obj, LV_KEYBOARD_MODE_NUMBER);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                }
            }
        }
    }
    lv_keyboard_set_textarea(objects.keyboard_kb, objects.textarea_input);
    
    tick_screen_controls();
}

void tick_screen_controls() {
    screen_controls_state_t *state = &screen_controls_state;
    (void)state;
    void *flowState = getFlowState(0, 1);
    (void)flowState;
    {
        int32_t new_val = evalIntegerProperty(flowState, 7, 3, "Failed to evaluate Value in Spinbox widget");
        int32_t cur_val = lv_spinbox_get_value(objects.spinbox_count);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.spinbox_count;
            lv_spinbox_set_value(objects.spinbox_count, new_val);
            tick_value_change_obj = NULL;
        }
    }
}

void create_screen_settings() {
    void *flowState = getFlowState(0, 2);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.settings = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 640);
    {
        lv_obj_t *parent_obj = obj;
        {
            // panel_root3
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_root3 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 480, 640);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x101828), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // tabview_cfg
                    lv_obj_t *obj = lv_tabview_create(parent_obj);
                    objects.tabview_cfg = obj;
                    lv_obj_set_pos(obj, 16, 16);
                    lv_obj_set_size(obj, 448, 420);
                    lv_tabview_set_tab_bar_position(obj, LV_DIR_TOP);
                    lv_tabview_set_tab_bar_size(obj, 44);
                    lv_obj_add_event_cb(obj, event_handler_cb_settings_tabview_cfg, LV_EVENT_ALL, flowState);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Display");
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // label_t1a
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.label_t1a = obj;
                                    lv_obj_set_pos(obj, 16, 16);
                                    lv_obj_set_size(obj, 200, 21);
                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x8fa0bc), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "Brightness");
                                }
                                {
                                    // slider_bright
                                    lv_obj_t *obj = lv_slider_create(parent_obj);
                                    objects.slider_bright = obj;
                                    lv_obj_set_pos(obj, 16, 44);
                                    lv_obj_set_size(obj, 320, 12);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                }
                                {
                                    // label_t1b
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.label_t1b = obj;
                                    lv_obj_set_pos(obj, 16, 76);
                                    lv_obj_set_size(obj, 200, 21);
                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x5ee6c4), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "Theme: Dark");
                                }
                            }
                        }
                        {
                            lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Network");
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // label_t2a
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.label_t2a = obj;
                                    lv_obj_set_pos(obj, 16, 16);
                                    lv_obj_set_size(obj, 300, 21);
                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x8fa0bc), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "Host: 192.168.1.10");
                                }
                                {
                                    // label_t2b
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.label_t2b = obj;
                                    lv_obj_set_pos(obj, 16, 44);
                                    lv_obj_set_size(obj, 300, 21);
                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x8fa0bc), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "Port: 502");
                                }
                                {
                                    // switch_dhcp
                                    lv_obj_t *obj = lv_switch_create(parent_obj);
                                    objects.switch_dhcp = obj;
                                    lv_obj_set_pos(obj, 340, 12);
                                    lv_obj_set_size(obj, 50, 25);
                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    tick_screen_settings();
}

void tick_screen_settings() {
    void *flowState = getFlowState(0, 2);
    (void)flowState;
    {
        int32_t new_val = evalIntegerProperty(flowState, 2, 3, "Failed to evaluate Active tab in Tabview widget");
        int32_t cur_val = lv_tabview_get_tab_active(objects.tabview_cfg);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.tabview_cfg;
            lv_tabview_set_active(objects.tabview_cfg, new_val, LV_ANIM_OFF);
            tick_value_change_obj = NULL;
        }
    }
}

typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
    tick_screen_controls,
    tick_screen_settings,
};
void tick_screen(int screen_index) {
    if (screen_index >= 0 && screen_index < 3) {
        tick_screen_funcs[screen_index]();
    }
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen(screenId - 1);
}

//
// Fonts
//

ext_font_desc_t fonts[] = {
    { "demo_16", &ui_font_demo_16 },
    { "demo_20", &ui_font_demo_20 },
#if LV_FONT_MONTSERRAT_8
    { "MONTSERRAT_8", &lv_font_montserrat_8 },
#endif
#if LV_FONT_MONTSERRAT_10
    { "MONTSERRAT_10", &lv_font_montserrat_10 },
#endif
#if LV_FONT_MONTSERRAT_12
    { "MONTSERRAT_12", &lv_font_montserrat_12 },
#endif
#if LV_FONT_MONTSERRAT_14
    { "MONTSERRAT_14", &lv_font_montserrat_14 },
#endif
#if LV_FONT_MONTSERRAT_16
    { "MONTSERRAT_16", &lv_font_montserrat_16 },
#endif
#if LV_FONT_MONTSERRAT_18
    { "MONTSERRAT_18", &lv_font_montserrat_18 },
#endif
#if LV_FONT_MONTSERRAT_20
    { "MONTSERRAT_20", &lv_font_montserrat_20 },
#endif
#if LV_FONT_MONTSERRAT_22
    { "MONTSERRAT_22", &lv_font_montserrat_22 },
#endif
#if LV_FONT_MONTSERRAT_24
    { "MONTSERRAT_24", &lv_font_montserrat_24 },
#endif
#if LV_FONT_MONTSERRAT_26
    { "MONTSERRAT_26", &lv_font_montserrat_26 },
#endif
#if LV_FONT_MONTSERRAT_28
    { "MONTSERRAT_28", &lv_font_montserrat_28 },
#endif
#if LV_FONT_MONTSERRAT_30
    { "MONTSERRAT_30", &lv_font_montserrat_30 },
#endif
#if LV_FONT_MONTSERRAT_32
    { "MONTSERRAT_32", &lv_font_montserrat_32 },
#endif
#if LV_FONT_MONTSERRAT_34
    { "MONTSERRAT_34", &lv_font_montserrat_34 },
#endif
#if LV_FONT_MONTSERRAT_36
    { "MONTSERRAT_36", &lv_font_montserrat_36 },
#endif
#if LV_FONT_MONTSERRAT_38
    { "MONTSERRAT_38", &lv_font_montserrat_38 },
#endif
#if LV_FONT_MONTSERRAT_40
    { "MONTSERRAT_40", &lv_font_montserrat_40 },
#endif
#if LV_FONT_MONTSERRAT_42
    { "MONTSERRAT_42", &lv_font_montserrat_42 },
#endif
#if LV_FONT_MONTSERRAT_44
    { "MONTSERRAT_44", &lv_font_montserrat_44 },
#endif
#if LV_FONT_MONTSERRAT_46
    { "MONTSERRAT_46", &lv_font_montserrat_46 },
#endif
#if LV_FONT_MONTSERRAT_48
    { "MONTSERRAT_48", &lv_font_montserrat_48 },
#endif
};

//
//
//

void create_screens() {
    
    eez_flow_init_fonts(fonts, sizeof(fonts) / sizeof(ext_font_desc_t));

// Set default LVGL theme
    lv_display_t *dispp = lv_display_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_display_set_theme(dispp, theme);
    
    // Initialize screens
    eez_flow_init_screen_names(screen_names, sizeof(screen_names) / sizeof(const char *));
    eez_flow_init_object_names(object_names, sizeof(object_names) / sizeof(const char *));
    
    // Create screens
    create_screen_main();
    create_screen_controls();
    create_screen_settings();
}