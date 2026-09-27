#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;

static const char *screen_names[] = { "overview", "params", "alarms" };
static const char *object_names[] = { "overview", "params", "alarms", "obj0", "obj0__obj0", "obj0__obj1", "obj0__obj2", "obj0__obj3", "obj0__obj4", "obj0__obj5", "obj0__obj6", "obj0__obj7", "obj0__obj8", "obj0__obj9", "obj1", "obj1__obj0", "obj1__obj1", "obj1__obj2", "obj1__obj3", "obj1__obj4", "obj1__obj5", "obj1__obj6", "obj1__obj7", "obj1__obj8", "obj1__obj9", "obj2", "obj2__obj0", "obj2__obj1", "obj2__obj2", "obj2__obj3", "obj2__obj4", "obj2__obj5", "obj2__obj6", "obj2__obj7", "obj2__obj8", "obj2__obj9", "obj3", "obj4", "obj5", "obj6", "obj7", "obj8", "panel_metrics_row", "panel_card_speed", "obj9", "label_val_speed", "obj10", "panel_card_torque", "obj11", "label_val_torque", "obj12", "panel_card_power", "obj13", "label_val_power", "obj14", "panel_card_eff", "obj15", "label_val_eff", "obj16", "panel_gauge_row", "panel_gauge_temp", "obj17", "obj18", "obj19", "obj20", "panel_gauge_volt", "obj21", "obj22", "obj23", "obj24", "panel_gauge_curr", "obj25", "obj26", "obj27", "obj28", "panel_ctrl_row", "panel_ctrl_speed", "obj29", "obj30", "obj31", "obj32", "label_sl_speed", "obj33", "obj34", "obj35", "panel_ctrl_run", "obj36", "obj37", "obj38", "obj39", "obj40", "obj41", "obj42", "obj43", "obj44", "obj45", "obj46", "obj47", "obj48", "obj49", "obj50", "obj51", "obj52", "obj53", "obj54", "obj55", "obj56", "dropdown_dd_poles", "obj57", "dropdown_dd_ctrl_mode", "obj58", "obj59", "obj60", "obj61", "obj62", "obj63", "obj64", "obj65", "obj66", "obj67", "obj68", "obj69", "obj70", "obj71", "obj72", "obj73", "obj74", "obj75", "obj76", "obj77", "obj78", "obj79", "obj80", "dropdown_dd_can_baud", "obj81", "obj82", "obj83", "obj84", "dropdown_dd_protocol", "obj85", "obj86", "obj87", "obj88", "obj89", "obj90", "obj91", "obj92", "obj93", "obj94", "obj95", "obj96", "obj97", "obj98", "obj99", "obj100", "obj101", "obj102", "obj103", "obj104", "obj105", "obj106", "obj107", "obj108", "obj109", "obj110", "panel_alarm_row1", "obj111", "obj112", "button_ack1", "obj113", "obj114", "obj115", "obj116", "obj117", "obj118", "obj119", "obj120", "obj121", "obj122", "obj123", "obj124", "obj125", "obj126", "obj127", "obj128" };

//
// Event handlers
//

lv_obj_t *tick_value_change_obj;

static void event_handler_cb_overview_obj4(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, -1, 4, e);
    }
}

static void event_handler_cb_overview_obj5(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, -1, 5, e);
    }
}

static void event_handler_cb_overview_obj6(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, -1, 6, e);
    }
}

static void event_handler_cb_overview_obj18(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target_obj(e);
        if (tick_value_change_obj != ta) {
            int32_t value = lv_arc_get_value(ta);
            assignIntegerProperty(flowState, 31, 3, value, "Failed to assign Value in Arc widget");
        }
    }
    if (event == LV_EVENT_VALUE_CHANGED) {
        e->user_data = (void *)0;
        action_on_motor_temp(e);
    }
}

static void event_handler_cb_overview_obj22(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target_obj(e);
        if (tick_value_change_obj != ta) {
            int32_t value = lv_arc_get_value(ta);
            assignIntegerProperty(flowState, 36, 3, value, "Failed to assign Value in Arc widget");
        }
    }
    if (event == LV_EVENT_VALUE_CHANGED) {
        e->user_data = (void *)0;
        action_on_bus_volt(e);
    }
}

static void event_handler_cb_overview_obj26(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target_obj(e);
        if (tick_value_change_obj != ta) {
            int32_t value = lv_arc_get_value(ta);
            assignIntegerProperty(flowState, 41, 3, value, "Failed to assign Value in Arc widget");
        }
    }
    if (event == LV_EVENT_VALUE_CHANGED) {
        e->user_data = (void *)0;
        action_on_out_curr(e);
    }
}

static void event_handler_cb_overview_obj32(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target_obj(e);
        if (tick_value_change_obj != ta) {
            int32_t value = lv_slider_get_value(ta);
            assignIntegerProperty(flowState, 49, 3, value, "Failed to assign Value in Slider widget");
        }
    }
    if (event == LV_EVENT_VALUE_CHANGED) {
        e->user_data = (void *)0;
        action_on_speed(e);
    }
}

static void event_handler_cb_overview_obj34(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target_obj(e);
        if (tick_value_change_obj != ta) {
            int32_t value = lv_slider_get_value(ta);
            assignIntegerProperty(flowState, 52, 3, value, "Failed to assign Value in Slider widget");
        }
    }
    if (event == LV_EVENT_VALUE_CHANGED) {
        e->user_data = (void *)0;
        action_on_torque(e);
    }
}

static void event_handler_cb_overview_obj39(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target_obj(e);
        if (tick_value_change_obj != ta) {
            bool value = lv_obj_has_state(ta, LV_STATE_CHECKED);
            assignBooleanProperty(flowState, 58, 3, value, "Failed to assign Checked state");
        }
    }
    if (event == LV_EVENT_VALUE_CHANGED) {
        e->user_data = (void *)0;
        action_on_fwd(e);
    }
}

static void event_handler_cb_overview_obj42(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target_obj(e);
        if (tick_value_change_obj != ta) {
            bool value = lv_obj_has_state(ta, LV_STATE_CHECKED);
            assignBooleanProperty(flowState, 61, 3, value, "Failed to assign Checked state");
        }
    }
    if (event == LV_EVENT_VALUE_CHANGED) {
        e->user_data = (void *)0;
        action_on_eco(e);
    }
}

static void event_handler_cb_params_obj45(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, -1, 4, e);
    }
}

static void event_handler_cb_params_obj46(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, -1, 5, e);
    }
}

static void event_handler_cb_params_obj47(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, -1, 6, e);
    }
}

static void event_handler_cb_params_dropdown_dd_poles(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        e->user_data = (void *)0;
        action_on_poles(e);
    }
}

static void event_handler_cb_params_dropdown_dd_ctrl_mode(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        e->user_data = (void *)0;
        action_on_ctrl_mode(e);
    }
}

static void event_handler_cb_params_obj59(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target_obj(e);
        if (tick_value_change_obj != ta) {
            int32_t value = lv_slider_get_value(ta);
            assignIntegerProperty(flowState, 25, 3, value, "Failed to assign Value in Slider widget");
        }
    }
    if (event == LV_EVENT_VALUE_CHANGED) {
        e->user_data = (void *)0;
        action_on_torque(e);
    }
}

static void event_handler_cb_params_obj65(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target_obj(e);
        if (tick_value_change_obj != ta) {
            int32_t value = lv_slider_get_value(ta);
            assignIntegerProperty(flowState, 31, 3, value, "Failed to assign Value in Slider widget");
        }
    }
    if (event == LV_EVENT_VALUE_CHANGED) {
        e->user_data = (void *)0;
        action_on_motor_temp(e);
    }
}

static void event_handler_cb_params_obj68(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target_obj(e);
        if (tick_value_change_obj != ta) {
            int32_t value = lv_slider_get_value(ta);
            assignIntegerProperty(flowState, 34, 3, value, "Failed to assign Value in Slider widget");
        }
    }
    if (event == LV_EVENT_VALUE_CHANGED) {
        e->user_data = (void *)0;
        action_on_bus_volt(e);
    }
}

static void event_handler_cb_params_obj71(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target_obj(e);
        if (tick_value_change_obj != ta) {
            int32_t value = lv_slider_get_value(ta);
            assignIntegerProperty(flowState, 37, 3, value, "Failed to assign Value in Slider widget");
        }
    }
    if (event == LV_EVENT_VALUE_CHANGED) {
        e->user_data = (void *)0;
        action_on_motor_temp(e);
    }
}

static void event_handler_cb_params_obj74(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target_obj(e);
        if (tick_value_change_obj != ta) {
            bool value = lv_obj_has_state(ta, LV_STATE_CHECKED);
            assignBooleanProperty(flowState, 40, 3, value, "Failed to assign Checked state");
        }
    }
    if (event == LV_EVENT_VALUE_CHANGED) {
        e->user_data = (void *)0;
        action_on_fwd(e);
    }
}

static void event_handler_cb_params_obj76(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target_obj(e);
        if (tick_value_change_obj != ta) {
            bool value = lv_obj_has_state(ta, LV_STATE_CHECKED);
            assignBooleanProperty(flowState, 42, 3, value, "Failed to assign Checked state");
        }
    }
    if (event == LV_EVENT_VALUE_CHANGED) {
        e->user_data = (void *)0;
        action_on_eco(e);
    }
}

static void event_handler_cb_params_dropdown_dd_can_baud(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        e->user_data = (void *)0;
        action_on_can_baud(e);
    }
}

static void event_handler_cb_params_obj82(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target_obj(e);
        if (tick_value_change_obj != ta) {
            int32_t value = lv_slider_get_value(ta);
            assignIntegerProperty(flowState, 49, 3, value, "Failed to assign Value in Slider widget");
        }
    }
    if (event == LV_EVENT_VALUE_CHANGED) {
        e->user_data = (void *)0;
        action_on_out_curr(e);
    }
}

static void event_handler_cb_params_dropdown_dd_protocol(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        e->user_data = (void *)0;
        action_on_protocol(e);
    }
}

static void event_handler_cb_params_obj86(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target_obj(e);
        if (tick_value_change_obj != ta) {
            int32_t value = lv_slider_get_value(ta);
            assignIntegerProperty(flowState, 54, 3, value, "Failed to assign Value in Slider widget");
        }
    }
    if (event == LV_EVENT_VALUE_CHANGED) {
        e->user_data = (void *)0;
        action_on_speed(e);
    }
}

static void event_handler_cb_alarms_obj89(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, -1, 4, e);
    }
}

static void event_handler_cb_alarms_obj90(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, -1, 5, e);
    }
}

static void event_handler_cb_alarms_obj91(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, -1, 6, e);
    }
}

static void event_handler_cb_alarms_button_ack1(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        action_ack_alarm(e);
    }
    if (event == LV_EVENT_LONG_PRESSED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, -1, 7, e);
    }
}

static void event_handler_cb_alarms_obj116(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        action_ack_alarm(e);
    }
}

static void event_handler_cb_alarms_obj120(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        action_ack_alarm(e);
    }
}

static void event_handler_cb_alarms_obj124(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        action_ack_alarm(e);
    }
}

static void event_handler_cb_alarms_obj128(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        action_ack_alarm(e);
    }
}

//
// Screens
//

void create_screen_overview() {
    void *flowState = getFlowState(0, 0);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.overview = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 1024, 600);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj0 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 1024, 36);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            create_user_widget_status_bar(obj, getFlowState(flowState, 0), 4);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj3 = obj;
            lv_obj_set_pos(obj, 0, 36);
            lv_obj_set_size(obj, 64, 564);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x151b28), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.obj4 = obj;
                    lv_obj_set_pos(obj, 6, 6);
                    lv_obj_set_size(obj, 52, 52);
                    lv_obj_add_event_cb(obj, event_handler_cb_overview_obj4, LV_EVENT_ALL, flowState);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x1b2c42), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x2e86de), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "\n总览");
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.obj5 = obj;
                    lv_obj_set_pos(obj, 6, 68);
                    lv_obj_set_size(obj, 52, 52);
                    lv_obj_add_event_cb(obj, event_handler_cb_overview_obj5, LV_EVENT_ALL, flowState);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x151b28), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "\n参数");
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.obj6 = obj;
                    lv_obj_set_pos(obj, 6, 130);
                    lv_obj_set_size(obj, 52, 52);
                    lv_obj_add_event_cb(obj, event_handler_cb_overview_obj6, LV_EVENT_ALL, flowState);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x151b28), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "\n报警");
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj7 = obj;
                    lv_obj_set_pos(obj, 0, 6);
                    lv_obj_set_size(obj, 4, 52);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x2e86de), LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_line_create(parent_obj);
                    objects.obj8 = obj;
                    lv_obj_set_pos(obj, 63, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    static lv_point_precise_t line_points[] = {
                        { 0, 0 },
                        { 1, 564 }
                    };
                    lv_line_set_points(obj, line_points, 2);
                    lv_line_set_y_invert(obj, true);
                    lv_obj_set_style_line_color(obj, lv_color_hex(0x2a3040), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_line_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // panel_metrics_row
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_metrics_row = obj;
            lv_obj_set_pos(obj, 72, 44);
            lv_obj_set_size(obj, 944, 80);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x0a0e14), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // panel_card_speed
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.panel_card_speed = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 227, 80);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x1c2333), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj9 = obj;
                            lv_obj_set_pos(obj, 12, 8);
                            lv_obj_set_size(obj, 120, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "转速 SPEED");
                        }
                        {
                            // label_val_speed
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.label_val_speed = obj;
                            lv_obj_set_pos(obj, 12, 30);
                            lv_obj_set_size(obj, 150, 30);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x0abde3), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj10 = obj;
                            lv_obj_set_pos(obj, 12, 58);
                            lv_obj_set_size(obj, 50, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "RPM");
                        }
                    }
                }
                {
                    // panel_card_torque
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.panel_card_torque = obj;
                    lv_obj_set_pos(obj, 239, 0);
                    lv_obj_set_size(obj, 227, 80);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x1c2333), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj11 = obj;
                            lv_obj_set_pos(obj, 12, 8);
                            lv_obj_set_size(obj, 130, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "扭矩 TORQUE");
                        }
                        {
                            // label_val_torque
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.label_val_torque = obj;
                            lv_obj_set_pos(obj, 12, 30);
                            lv_obj_set_size(obj, 150, 30);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x0abde3), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj12 = obj;
                            lv_obj_set_pos(obj, 12, 58);
                            lv_obj_set_size(obj, 50, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "N·m");
                        }
                    }
                }
                {
                    // panel_card_power
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.panel_card_power = obj;
                    lv_obj_set_pos(obj, 478, 0);
                    lv_obj_set_size(obj, 227, 80);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x1c2333), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj13 = obj;
                            lv_obj_set_pos(obj, 12, 8);
                            lv_obj_set_size(obj, 120, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "功率 POWER");
                        }
                        {
                            // label_val_power
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.label_val_power = obj;
                            lv_obj_set_pos(obj, 12, 30);
                            lv_obj_set_size(obj, 150, 30);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x0abde3), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj14 = obj;
                            lv_obj_set_pos(obj, 12, 58);
                            lv_obj_set_size(obj, 50, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "kW");
                        }
                    }
                }
                {
                    // panel_card_eff
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.panel_card_eff = obj;
                    lv_obj_set_pos(obj, 717, 0);
                    lv_obj_set_size(obj, 227, 80);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x1c2333), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj15 = obj;
                            lv_obj_set_pos(obj, 12, 8);
                            lv_obj_set_size(obj, 158, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "效率 EFFICIENCY");
                        }
                        {
                            // label_val_eff
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.label_val_eff = obj;
                            lv_obj_set_pos(obj, 12, 30);
                            lv_obj_set_size(obj, 150, 30);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x0abde3), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj16 = obj;
                            lv_obj_set_pos(obj, 12, 58);
                            lv_obj_set_size(obj, 30, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "%");
                        }
                    }
                }
            }
        }
        {
            // panel_gauge_row
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_gauge_row = obj;
            lv_obj_set_pos(obj, 72, 136);
            lv_obj_set_size(obj, 944, 240);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x0a0e14), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // panel_gauge_temp
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.panel_gauge_temp = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 306, 240);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x151b28), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj17 = obj;
                            lv_obj_set_pos(obj, 108, 8);
                            lv_obj_set_size(obj, 90, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "电机温度");
                        }
                        {
                            lv_obj_t *obj = lv_arc_create(parent_obj);
                            objects.obj18 = obj;
                            lv_obj_set_pos(obj, 83, 40);
                            lv_obj_set_size(obj, 140, 140);
                            lv_arc_set_range(obj, 0, 150);
                            lv_obj_add_event_cb(obj, event_handler_cb_overview_obj18, LV_EVENT_ALL, flowState);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj19 = obj;
                            lv_obj_set_pos(obj, 113, 90);
                            lv_obj_set_size(obj, 80, 28);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xf6b93b), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj20 = obj;
                            lv_obj_set_pos(obj, 133, 118);
                            lv_obj_set_size(obj, 40, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "°C");
                        }
                    }
                }
                {
                    // panel_gauge_volt
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.panel_gauge_volt = obj;
                    lv_obj_set_pos(obj, 318, 0);
                    lv_obj_set_size(obj, 306, 240);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x151b28), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj21 = obj;
                            lv_obj_set_pos(obj, 108, 8);
                            lv_obj_set_size(obj, 90, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "母线电压");
                        }
                        {
                            lv_obj_t *obj = lv_arc_create(parent_obj);
                            objects.obj22 = obj;
                            lv_obj_set_pos(obj, 83, 40);
                            lv_obj_set_size(obj, 140, 140);
                            lv_arc_set_range(obj, 0, 900);
                            lv_obj_add_event_cb(obj, event_handler_cb_overview_obj22, LV_EVENT_ALL, flowState);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj23 = obj;
                            lv_obj_set_pos(obj, 103, 90);
                            lv_obj_set_size(obj, 100, 28);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x2e86de), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj24 = obj;
                            lv_obj_set_pos(obj, 123, 118);
                            lv_obj_set_size(obj, 60, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "V DC");
                        }
                    }
                }
                {
                    // panel_gauge_curr
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.panel_gauge_curr = obj;
                    lv_obj_set_pos(obj, 636, 0);
                    lv_obj_set_size(obj, 306, 240);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x151b28), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj25 = obj;
                            lv_obj_set_pos(obj, 108, 8);
                            lv_obj_set_size(obj, 90, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "输出电流");
                        }
                        {
                            lv_obj_t *obj = lv_arc_create(parent_obj);
                            objects.obj26 = obj;
                            lv_obj_set_pos(obj, 83, 40);
                            lv_obj_set_size(obj, 140, 140);
                            lv_arc_set_range(obj, 0, 30);
                            lv_obj_add_event_cb(obj, event_handler_cb_overview_obj26, LV_EVENT_ALL, flowState);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj27 = obj;
                            lv_obj_set_pos(obj, 103, 90);
                            lv_obj_set_size(obj, 100, 28);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x2e86de), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj28 = obj;
                            lv_obj_set_pos(obj, 138, 118);
                            lv_obj_set_size(obj, 30, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "A");
                        }
                    }
                }
            }
        }
        {
            // panel_ctrl_row
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_ctrl_row = obj;
            lv_obj_set_pos(obj, 72, 388);
            lv_obj_set_size(obj, 944, 204);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x0a0e14), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // panel_ctrl_speed
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.panel_ctrl_speed = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 466, 204);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x151b28), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj29 = obj;
                            lv_obj_set_pos(obj, 12, 8);
                            lv_obj_set_size(obj, 80, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "速度控制");
                        }
                        {
                            lv_obj_t *obj = lv_line_create(parent_obj);
                            objects.obj30 = obj;
                            lv_obj_set_pos(obj, 12, 28);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            static lv_point_precise_t line_points[] = {
                                { 0, 0 },
                                { 442, 1 }
                            };
                            lv_line_set_points(obj, line_points, 2);
                            lv_line_set_y_invert(obj, true);
                            lv_obj_set_style_line_color(obj, lv_color_hex(0x2a3040), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_line_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj31 = obj;
                            lv_obj_set_pos(obj, 12, 44);
                            lv_obj_set_size(obj, 50, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "目标");
                        }
                        {
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.obj32 = obj;
                            lv_obj_set_pos(obj, 70, 46);
                            lv_obj_set_size(obj, 280, 18);
                            lv_slider_set_range(obj, 0, 3000);
                            lv_obj_add_event_cb(obj, event_handler_cb_overview_obj32, LV_EVENT_ALL, flowState);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                        }
                        {
                            // label_sl_speed
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.label_sl_speed = obj;
                            lv_obj_set_pos(obj, 360, 44);
                            lv_obj_set_size(obj, 60, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x0abde3), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj33 = obj;
                            lv_obj_set_pos(obj, 12, 80);
                            lv_obj_set_size(obj, 50, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "加速");
                        }
                        {
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.obj34 = obj;
                            lv_obj_set_pos(obj, 70, 82);
                            lv_obj_set_size(obj, 280, 18);
                            lv_slider_set_range(obj, 1, 30);
                            lv_obj_add_event_cb(obj, event_handler_cb_overview_obj34, LV_EVENT_ALL, flowState);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj35 = obj;
                            lv_obj_set_pos(obj, 360, 80);
                            lv_obj_set_size(obj, 56, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x0abde3), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "5.0s");
                        }
                    }
                }
                {
                    // panel_ctrl_run
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.panel_ctrl_run = obj;
                    lv_obj_set_pos(obj, 478, 0);
                    lv_obj_set_size(obj, 466, 204);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x151b28), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj36 = obj;
                            lv_obj_set_pos(obj, 12, 8);
                            lv_obj_set_size(obj, 80, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "运行控制");
                        }
                        {
                            lv_obj_t *obj = lv_line_create(parent_obj);
                            objects.obj37 = obj;
                            lv_obj_set_pos(obj, 12, 28);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            static lv_point_precise_t line_points[] = {
                                { 0, 0 },
                                { 442, 1 }
                            };
                            lv_line_set_points(obj, line_points, 2);
                            lv_line_set_y_invert(obj, true);
                            lv_obj_set_style_line_color(obj, lv_color_hex(0x2a3040), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_line_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj38 = obj;
                            lv_obj_set_pos(obj, 12, 50);
                            lv_obj_set_size(obj, 50, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "正转");
                        }
                        {
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.obj39 = obj;
                            lv_obj_set_pos(obj, 70, 46);
                            lv_obj_set_size(obj, 50, 24);
                            lv_obj_add_event_cb(obj, event_handler_cb_overview_obj39, LV_EVENT_ALL, flowState);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj40 = obj;
                            lv_obj_set_pos(obj, 130, 50);
                            lv_obj_set_size(obj, 64, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x27ae60), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "运行中");
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj41 = obj;
                            lv_obj_set_pos(obj, 12, 90);
                            lv_obj_set_size(obj, 50, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "节能");
                        }
                        {
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.obj42 = obj;
                            lv_obj_set_pos(obj, 70, 86);
                            lv_obj_set_size(obj, 50, 24);
                            lv_obj_add_event_cb(obj, event_handler_cb_overview_obj42, LV_EVENT_ALL, flowState);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj43 = obj;
                            lv_obj_set_pos(obj, 130, 90);
                            lv_obj_set_size(obj, 50, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "关闭");
                        }
                    }
                }
            }
        }
    }
    
    tick_screen_overview();
}

void tick_screen_overview() {
    void *flowState = getFlowState(0, 0);
    (void)flowState;
    tick_user_widget_status_bar(getFlowState(flowState, 0), 4);
    {
        const char *new_val = evalTextProperty(flowState, 14, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.label_val_speed);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.label_val_speed;
            lv_label_set_text(objects.label_val_speed, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 18, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.label_val_torque);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.label_val_torque;
            lv_label_set_text(objects.label_val_torque, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 22, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.label_val_power);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.label_val_power;
            lv_label_set_text(objects.label_val_power, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 26, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.label_val_eff);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.label_val_eff;
            lv_label_set_text(objects.label_val_eff, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 31, 3, "Failed to evaluate Value in Arc widget");
        int32_t cur_val = lv_arc_get_value(objects.obj18);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj18;
            lv_arc_set_value(objects.obj18, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 32, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj19);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj19;
            lv_label_set_text(objects.obj19, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 36, 3, "Failed to evaluate Value in Arc widget");
        int32_t cur_val = lv_arc_get_value(objects.obj22);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj22;
            lv_arc_set_value(objects.obj22, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 37, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj23);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj23;
            lv_label_set_text(objects.obj23, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 41, 3, "Failed to evaluate Value in Arc widget");
        int32_t cur_val = lv_arc_get_value(objects.obj26);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj26;
            lv_arc_set_value(objects.obj26, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 42, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj27);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj27;
            lv_label_set_text(objects.obj27, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 49, 3, "Failed to evaluate Value in Slider widget");
        int32_t cur_val = lv_slider_get_value(objects.obj32);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj32;
            lv_slider_set_value(objects.obj32, new_val, LV_ANIM_OFF);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 50, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.label_sl_speed);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.label_sl_speed;
            lv_label_set_text(objects.label_sl_speed, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 52, 3, "Failed to evaluate Value in Slider widget");
        int32_t cur_val = lv_slider_get_value(objects.obj34);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj34;
            lv_slider_set_value(objects.obj34, new_val, LV_ANIM_OFF);
            tick_value_change_obj = NULL;
        }
    }
    {
        bool new_val = evalBooleanProperty(flowState, 58, 3, "Failed to evaluate Checked state");
        bool cur_val = lv_obj_has_state(objects.obj39, LV_STATE_CHECKED);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj39;
            if (new_val) {
                lv_obj_add_state(objects.obj39, LV_STATE_CHECKED);
            } else {
                lv_obj_remove_state(objects.obj39, LV_STATE_CHECKED);
            }
            tick_value_change_obj = NULL;
        }
    }
    {
        bool new_val = evalBooleanProperty(flowState, 61, 3, "Failed to evaluate Checked state");
        bool cur_val = lv_obj_has_state(objects.obj42, LV_STATE_CHECKED);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj42;
            if (new_val) {
                lv_obj_add_state(objects.obj42, LV_STATE_CHECKED);
            } else {
                lv_obj_remove_state(objects.obj42, LV_STATE_CHECKED);
            }
            tick_value_change_obj = NULL;
        }
    }
}

void create_screen_params() {
    void *flowState = getFlowState(0, 1);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.params = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 1024, 600);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj1 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 1024, 36);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            create_user_widget_status_bar(obj, getFlowState(flowState, 0), 15);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj44 = obj;
            lv_obj_set_pos(obj, 0, 36);
            lv_obj_set_size(obj, 64, 564);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x151b28), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.obj45 = obj;
                    lv_obj_set_pos(obj, 6, 6);
                    lv_obj_set_size(obj, 52, 52);
                    lv_obj_add_event_cb(obj, event_handler_cb_params_obj45, LV_EVENT_ALL, flowState);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x151b28), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "\n总览");
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.obj46 = obj;
                    lv_obj_set_pos(obj, 6, 68);
                    lv_obj_set_size(obj, 52, 52);
                    lv_obj_add_event_cb(obj, event_handler_cb_params_obj46, LV_EVENT_ALL, flowState);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x1b2c42), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x2e86de), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "\n参数");
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.obj47 = obj;
                    lv_obj_set_pos(obj, 6, 130);
                    lv_obj_set_size(obj, 52, 52);
                    lv_obj_add_event_cb(obj, event_handler_cb_params_obj47, LV_EVENT_ALL, flowState);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x151b28), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "\n报警");
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj48 = obj;
                    lv_obj_set_pos(obj, 0, 68);
                    lv_obj_set_size(obj, 4, 52);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x2e86de), LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_line_create(parent_obj);
                    objects.obj49 = obj;
                    lv_obj_set_pos(obj, 63, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    static lv_point_precise_t line_points[] = {
                        { 0, 0 },
                        { 1, 564 }
                    };
                    lv_line_set_points(obj, line_points, 2);
                    lv_line_set_y_invert(obj, true);
                    lv_obj_set_style_line_color(obj, lv_color_hex(0x2a3040), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_line_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj50 = obj;
            lv_obj_set_pos(obj, 72, 44);
            lv_obj_set_size(obj, 304, 548);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x151b28), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj51 = obj;
                    lv_obj_set_pos(obj, 12, 10);
                    lv_obj_set_size(obj, 200, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x2e86de), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "电机参数 / Motor");
                }
                {
                    lv_obj_t *obj = lv_line_create(parent_obj);
                    objects.obj52 = obj;
                    lv_obj_set_pos(obj, 12, 32);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    static lv_point_precise_t line_points[] = {
                        { 0, 0 },
                        { 280, 1 }
                    };
                    lv_line_set_points(obj, line_points, 2);
                    lv_line_set_y_invert(obj, true);
                    lv_obj_set_style_line_color(obj, lv_color_hex(0x2a3040), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_line_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj53 = obj;
                    lv_obj_set_pos(obj, 12, 46);
                    lv_obj_set_size(obj, 80, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "额定功率");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 110, 46);
                    lv_obj_set_size(obj, 80, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "7.5 kW");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj54 = obj;
                    lv_obj_set_pos(obj, 12, 72);
                    lv_obj_set_size(obj, 80, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "额定电压");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 110, 72);
                    lv_obj_set_size(obj, 96, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "380 V AC");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj55 = obj;
                    lv_obj_set_pos(obj, 12, 98);
                    lv_obj_set_size(obj, 80, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "额定电流");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 110, 98);
                    lv_obj_set_size(obj, 76, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "16.5 A");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj56 = obj;
                    lv_obj_set_pos(obj, 12, 124);
                    lv_obj_set_size(obj, 50, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "极数");
                }
                {
                    // dropdown_dd_poles
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    objects.dropdown_dd_poles = obj;
                    lv_obj_set_pos(obj, 110, 120);
                    lv_obj_set_size(obj, 100, 26);
                    lv_dropdown_set_options_static(obj, "4P\n2P\n6P");
                    lv_dropdown_set_selected(obj, 0);
                    lv_obj_add_event_cb(obj, event_handler_cb_params_dropdown_dd_poles, LV_EVENT_ALL, flowState);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj57 = obj;
                    lv_obj_set_pos(obj, 12, 156);
                    lv_obj_set_size(obj, 80, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "控制模式");
                }
                {
                    // dropdown_dd_ctrl_mode
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    objects.dropdown_dd_ctrl_mode = obj;
                    lv_obj_set_pos(obj, 110, 152);
                    lv_obj_set_size(obj, 120, 26);
                    lv_dropdown_set_options_static(obj, "V/F\nVector\nServo");
                    lv_dropdown_set_selected(obj, 0);
                    lv_obj_add_event_cb(obj, event_handler_cb_params_dropdown_dd_ctrl_mode, LV_EVENT_ALL, flowState);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj58 = obj;
                    lv_obj_set_pos(obj, 12, 190);
                    lv_obj_set_size(obj, 80, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "载波频率");
                }
                {
                    lv_obj_t *obj = lv_slider_create(parent_obj);
                    objects.obj59 = obj;
                    lv_obj_set_pos(obj, 110, 192);
                    lv_obj_set_size(obj, 130, 18);
                    lv_slider_set_range(obj, 2, 16);
                    lv_obj_add_event_cb(obj, event_handler_cb_params_obj59, LV_EVENT_ALL, flowState);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj60 = obj;
                    lv_obj_set_pos(obj, 250, 190);
                    lv_obj_set_size(obj, 66, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x0abde3), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "8 kHz");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj61 = obj;
            lv_obj_set_pos(obj, 386, 44);
            lv_obj_set_size(obj, 304, 548);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x151b28), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj62 = obj;
                    lv_obj_set_pos(obj, 12, 10);
                    lv_obj_set_size(obj, 220, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x2e86de), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "保护参数 / Protection");
                }
                {
                    lv_obj_t *obj = lv_line_create(parent_obj);
                    objects.obj63 = obj;
                    lv_obj_set_pos(obj, 12, 32);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    static lv_point_precise_t line_points[] = {
                        { 0, 0 },
                        { 280, 1 }
                    };
                    lv_line_set_points(obj, line_points, 2);
                    lv_line_set_y_invert(obj, true);
                    lv_obj_set_style_line_color(obj, lv_color_hex(0x2a3040), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_line_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj64 = obj;
                    lv_obj_set_pos(obj, 12, 46);
                    lv_obj_set_size(obj, 80, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "过流保护");
                }
                {
                    lv_obj_t *obj = lv_slider_create(parent_obj);
                    objects.obj65 = obj;
                    lv_obj_set_pos(obj, 110, 48);
                    lv_obj_set_size(obj, 130, 18);
                    lv_slider_set_range(obj, 50, 200);
                    lv_obj_add_event_cb(obj, event_handler_cb_params_obj65, LV_EVENT_ALL, flowState);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj66 = obj;
                    lv_obj_set_pos(obj, 250, 46);
                    lv_obj_set_size(obj, 56, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x0abde3), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "150%");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj67 = obj;
                    lv_obj_set_pos(obj, 12, 80);
                    lv_obj_set_size(obj, 80, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "过压保护");
                }
                {
                    lv_obj_t *obj = lv_slider_create(parent_obj);
                    objects.obj68 = obj;
                    lv_obj_set_pos(obj, 110, 82);
                    lv_obj_set_size(obj, 130, 18);
                    lv_slider_set_range(obj, 600, 900);
                    lv_obj_add_event_cb(obj, event_handler_cb_params_obj68, LV_EVENT_ALL, flowState);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj69 = obj;
                    lv_obj_set_pos(obj, 250, 80);
                    lv_obj_set_size(obj, 56, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x0abde3), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "800V");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj70 = obj;
                    lv_obj_set_pos(obj, 12, 114);
                    lv_obj_set_size(obj, 80, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "过温阈值");
                }
                {
                    lv_obj_t *obj = lv_slider_create(parent_obj);
                    objects.obj71 = obj;
                    lv_obj_set_pos(obj, 110, 116);
                    lv_obj_set_size(obj, 130, 18);
                    lv_slider_set_range(obj, 60, 120);
                    lv_obj_add_event_cb(obj, event_handler_cb_params_obj71, LV_EVENT_ALL, flowState);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj72 = obj;
                    lv_obj_set_pos(obj, 250, 114);
                    lv_obj_set_size(obj, 56, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x0abde3), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "90°C");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj73 = obj;
                    lv_obj_set_pos(obj, 12, 150);
                    lv_obj_set_size(obj, 80, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "失速保护");
                }
                {
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.obj74 = obj;
                    lv_obj_set_pos(obj, 110, 146);
                    lv_obj_set_size(obj, 50, 24);
                    lv_obj_add_event_cb(obj, event_handler_cb_params_obj74, LV_EVENT_ALL, flowState);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj75 = obj;
                    lv_obj_set_pos(obj, 12, 186);
                    lv_obj_set_size(obj, 80, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "缺相检测");
                }
                {
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.obj76 = obj;
                    lv_obj_set_pos(obj, 110, 182);
                    lv_obj_set_size(obj, 50, 24);
                    lv_obj_add_event_cb(obj, event_handler_cb_params_obj76, LV_EVENT_ALL, flowState);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj77 = obj;
            lv_obj_set_pos(obj, 700, 44);
            lv_obj_set_size(obj, 304, 548);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x151b28), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj78 = obj;
                    lv_obj_set_pos(obj, 12, 10);
                    lv_obj_set_size(obj, 220, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x2e86de), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "通信 / Communication");
                }
                {
                    lv_obj_t *obj = lv_line_create(parent_obj);
                    objects.obj79 = obj;
                    lv_obj_set_pos(obj, 12, 32);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    static lv_point_precise_t line_points[] = {
                        { 0, 0 },
                        { 280, 1 }
                    };
                    lv_line_set_points(obj, line_points, 2);
                    lv_line_set_y_invert(obj, true);
                    lv_obj_set_style_line_color(obj, lv_color_hex(0x2a3040), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_line_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj80 = obj;
                    lv_obj_set_pos(obj, 12, 46);
                    lv_obj_set_size(obj, 104, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "CAN 波特率");
                }
                {
                    // dropdown_dd_can_baud
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    objects.dropdown_dd_can_baud = obj;
                    lv_obj_set_pos(obj, 120, 42);
                    lv_obj_set_size(obj, 90, 26);
                    lv_dropdown_set_options_static(obj, "250K\n500K\n1M");
                    lv_dropdown_set_selected(obj, 1);
                    lv_obj_add_event_cb(obj, event_handler_cb_params_dropdown_dd_can_baud, LV_EVENT_ALL, flowState);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj81 = obj;
                    lv_obj_set_pos(obj, 12, 78);
                    lv_obj_set_size(obj, 88, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "CAN 节点");
                }
                {
                    lv_obj_t *obj = lv_slider_create(parent_obj);
                    objects.obj82 = obj;
                    lv_obj_set_pos(obj, 120, 80);
                    lv_obj_set_size(obj, 120, 18);
                    lv_slider_set_range(obj, 1, 127);
                    lv_obj_add_event_cb(obj, event_handler_cb_params_obj82, LV_EVENT_ALL, flowState);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj83 = obj;
                    lv_obj_set_pos(obj, 250, 78);
                    lv_obj_set_size(obj, 76, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x0abde3), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Node 3");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj84 = obj;
                    lv_obj_set_pos(obj, 12, 110);
                    lv_obj_set_size(obj, 66, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "RS485");
                }
                {
                    // dropdown_dd_protocol
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    objects.dropdown_dd_protocol = obj;
                    lv_obj_set_pos(obj, 120, 106);
                    lv_obj_set_size(obj, 140, 26);
                    lv_dropdown_set_options_static(obj, "Modbus RTU\nCustom");
                    lv_dropdown_set_selected(obj, 0);
                    lv_obj_add_event_cb(obj, event_handler_cb_params_dropdown_dd_protocol, LV_EVENT_ALL, flowState);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj85 = obj;
                    lv_obj_set_pos(obj, 12, 146);
                    lv_obj_set_size(obj, 80, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "心跳间隔");
                }
                {
                    lv_obj_t *obj = lv_slider_create(parent_obj);
                    objects.obj86 = obj;
                    lv_obj_set_pos(obj, 120, 148);
                    lv_obj_set_size(obj, 120, 18);
                    lv_slider_set_range(obj, 100, 5000);
                    lv_obj_add_event_cb(obj, event_handler_cb_params_obj86, LV_EVENT_ALL, flowState);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj87 = obj;
                    lv_obj_set_pos(obj, 250, 146);
                    lv_obj_set_size(obj, 56, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x0abde3), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "1.0s");
                }
            }
        }
    }
    
    tick_screen_params();
}

void tick_screen_params() {
    void *flowState = getFlowState(0, 1);
    (void)flowState;
    tick_user_widget_status_bar(getFlowState(flowState, 0), 15);
    {
        int32_t new_val = evalIntegerProperty(flowState, 25, 3, "Failed to evaluate Value in Slider widget");
        int32_t cur_val = lv_slider_get_value(objects.obj59);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj59;
            lv_slider_set_value(objects.obj59, new_val, LV_ANIM_OFF);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 31, 3, "Failed to evaluate Value in Slider widget");
        int32_t cur_val = lv_slider_get_value(objects.obj65);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj65;
            lv_slider_set_value(objects.obj65, new_val, LV_ANIM_OFF);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 34, 3, "Failed to evaluate Value in Slider widget");
        int32_t cur_val = lv_slider_get_value(objects.obj68);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj68;
            lv_slider_set_value(objects.obj68, new_val, LV_ANIM_OFF);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 37, 3, "Failed to evaluate Value in Slider widget");
        int32_t cur_val = lv_slider_get_value(objects.obj71);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj71;
            lv_slider_set_value(objects.obj71, new_val, LV_ANIM_OFF);
            tick_value_change_obj = NULL;
        }
    }
    {
        bool new_val = evalBooleanProperty(flowState, 40, 3, "Failed to evaluate Checked state");
        bool cur_val = lv_obj_has_state(objects.obj74, LV_STATE_CHECKED);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj74;
            if (new_val) {
                lv_obj_add_state(objects.obj74, LV_STATE_CHECKED);
            } else {
                lv_obj_remove_state(objects.obj74, LV_STATE_CHECKED);
            }
            tick_value_change_obj = NULL;
        }
    }
    {
        bool new_val = evalBooleanProperty(flowState, 42, 3, "Failed to evaluate Checked state");
        bool cur_val = lv_obj_has_state(objects.obj76, LV_STATE_CHECKED);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj76;
            if (new_val) {
                lv_obj_add_state(objects.obj76, LV_STATE_CHECKED);
            } else {
                lv_obj_remove_state(objects.obj76, LV_STATE_CHECKED);
            }
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 49, 3, "Failed to evaluate Value in Slider widget");
        int32_t cur_val = lv_slider_get_value(objects.obj82);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj82;
            lv_slider_set_value(objects.obj82, new_val, LV_ANIM_OFF);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 54, 3, "Failed to evaluate Value in Slider widget");
        int32_t cur_val = lv_slider_get_value(objects.obj86);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj86;
            lv_slider_set_value(objects.obj86, new_val, LV_ANIM_OFF);
            tick_value_change_obj = NULL;
        }
    }
}

void create_screen_alarms() {
    void *flowState = getFlowState(0, 2);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.alarms = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 1024, 600);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj2 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 1024, 36);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            create_user_widget_status_bar(obj, getFlowState(flowState, 0), 26);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj88 = obj;
            lv_obj_set_pos(obj, 0, 36);
            lv_obj_set_size(obj, 64, 564);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x151b28), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.obj89 = obj;
                    lv_obj_set_pos(obj, 6, 6);
                    lv_obj_set_size(obj, 52, 52);
                    lv_obj_add_event_cb(obj, event_handler_cb_alarms_obj89, LV_EVENT_ALL, flowState);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x151b28), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "\n总览");
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.obj90 = obj;
                    lv_obj_set_pos(obj, 6, 68);
                    lv_obj_set_size(obj, 52, 52);
                    lv_obj_add_event_cb(obj, event_handler_cb_alarms_obj90, LV_EVENT_ALL, flowState);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x151b28), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "\n参数");
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.obj91 = obj;
                    lv_obj_set_pos(obj, 6, 130);
                    lv_obj_set_size(obj, 52, 52);
                    lv_obj_add_event_cb(obj, event_handler_cb_alarms_obj91, LV_EVENT_ALL, flowState);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x1b2c42), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x2e86de), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "\n报警");
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj92 = obj;
                    lv_obj_set_pos(obj, 0, 130);
                    lv_obj_set_size(obj, 4, 52);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x2e86de), LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_line_create(parent_obj);
                    objects.obj93 = obj;
                    lv_obj_set_pos(obj, 63, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    static lv_point_precise_t line_points[] = {
                        { 0, 0 },
                        { 1, 564 }
                    };
                    lv_line_set_points(obj, line_points, 2);
                    lv_line_set_y_invert(obj, true);
                    lv_obj_set_style_line_color(obj, lv_color_hex(0x2a3040), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_line_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj94 = obj;
            lv_obj_set_pos(obj, 72, 44);
            lv_obj_set_size(obj, 300, 56);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x1c2333), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_led_create(parent_obj);
                    objects.obj95 = obj;
                    lv_obj_set_pos(obj, 12, 24);
                    lv_obj_set_size(obj, 8, 8);
                    lv_led_set_color(obj, lv_color_hex(0xe74c3c));
                    lv_led_set_brightness(obj, 255);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj96 = obj;
                    lv_obj_set_pos(obj, 28, 14);
                    lv_obj_set_size(obj, 40, 28);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xe74c3c), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "2");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj97 = obj;
                    lv_obj_set_pos(obj, 80, 20);
                    lv_obj_set_size(obj, 50, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "严重");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj98 = obj;
            lv_obj_set_pos(obj, 384, 44);
            lv_obj_set_size(obj, 300, 56);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x1c2333), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_led_create(parent_obj);
                    objects.obj99 = obj;
                    lv_obj_set_pos(obj, 12, 24);
                    lv_obj_set_size(obj, 8, 8);
                    lv_led_set_color(obj, lv_color_hex(0xf6b93b));
                    lv_led_set_brightness(obj, 255);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj100 = obj;
                    lv_obj_set_pos(obj, 28, 14);
                    lv_obj_set_size(obj, 40, 28);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xf6b93b), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "3");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj101 = obj;
                    lv_obj_set_pos(obj, 80, 20);
                    lv_obj_set_size(obj, 50, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "警告");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj102 = obj;
            lv_obj_set_pos(obj, 696, 44);
            lv_obj_set_size(obj, 300, 56);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x1c2333), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_led_create(parent_obj);
                    objects.obj103 = obj;
                    lv_obj_set_pos(obj, 12, 24);
                    lv_obj_set_size(obj, 8, 8);
                    lv_led_set_color(obj, lv_color_hex(0x27ae60));
                    lv_led_set_brightness(obj, 255);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj104 = obj;
                    lv_obj_set_pos(obj, 28, 14);
                    lv_obj_set_size(obj, 50, 28);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x27ae60), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "12");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj105 = obj;
                    lv_obj_set_pos(obj, 90, 20);
                    lv_obj_set_size(obj, 50, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "正常");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj106 = obj;
            lv_obj_set_pos(obj, 72, 112);
            lv_obj_set_size(obj, 924, 480);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x151b28), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj107 = obj;
                    lv_obj_set_pos(obj, 40, 8);
                    lv_obj_set_size(obj, 60, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "时间");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj108 = obj;
                    lv_obj_set_pos(obj, 120, 8);
                    lv_obj_set_size(obj, 60, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "描述");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj109 = obj;
                    lv_obj_set_pos(obj, 850, 8);
                    lv_obj_set_size(obj, 50, 21);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "操作");
                }
                {
                    lv_obj_t *obj = lv_line_create(parent_obj);
                    objects.obj110 = obj;
                    lv_obj_set_pos(obj, 10, 26);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    static lv_point_precise_t line_points[] = {
                        { 0, 0 },
                        { 904, 1 }
                    };
                    lv_line_set_points(obj, line_points, 2);
                    lv_line_set_y_invert(obj, true);
                    lv_obj_set_style_line_color(obj, lv_color_hex(0x2a3040), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_line_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // panel_alarm_row1
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.panel_alarm_row1 = obj;
                    lv_obj_set_pos(obj, 10, 34);
                    lv_obj_set_size(obj, 904, 40);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x1c2333), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_led_create(parent_obj);
                            objects.obj111 = obj;
                            lv_obj_set_pos(obj, 10, 16);
                            lv_obj_set_size(obj, 8, 8);
                            lv_led_set_color(obj, lv_color_hex(0xe74c3c));
                            lv_led_set_brightness(obj, 255);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj112 = obj;
                            lv_obj_set_pos(obj, 30, 12);
                            lv_obj_set_size(obj, 96, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "14:22:15");
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 110, 12);
                            lv_obj_set_size(obj, 300, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "电机过流 152% (阈值 150%)");
                        }
                        {
                            // button_ack1
                            lv_obj_t *obj = lv_button_create(parent_obj);
                            objects.button_ack1 = obj;
                            lv_obj_set_pos(obj, 820, 8);
                            lv_obj_set_size(obj, 70, 24);
                            lv_obj_add_event_cb(obj, event_handler_cb_alarms_button_ack1, LV_EVENT_ALL, flowState);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_set_style_radius(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0x1c2333), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "确认");
                                }
                            }
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj113 = obj;
                    lv_obj_set_pos(obj, 10, 82);
                    lv_obj_set_size(obj, 904, 40);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x1c2333), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_led_create(parent_obj);
                            objects.obj114 = obj;
                            lv_obj_set_pos(obj, 10, 16);
                            lv_obj_set_size(obj, 8, 8);
                            lv_led_set_color(obj, lv_color_hex(0xe74c3c));
                            lv_led_set_brightness(obj, 255);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj115 = obj;
                            lv_obj_set_pos(obj, 30, 12);
                            lv_obj_set_size(obj, 96, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "13:05:42");
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 110, 12);
                            lv_obj_set_size(obj, 300, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "母线过压 815V (阈值 800V)");
                        }
                        {
                            lv_obj_t *obj = lv_button_create(parent_obj);
                            objects.obj116 = obj;
                            lv_obj_set_pos(obj, 820, 8);
                            lv_obj_set_size(obj, 70, 24);
                            lv_obj_add_event_cb(obj, event_handler_cb_alarms_obj116, LV_EVENT_ALL, flowState);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_set_style_radius(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0x1c2333), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "确认");
                                }
                            }
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj117 = obj;
                    lv_obj_set_pos(obj, 10, 130);
                    lv_obj_set_size(obj, 904, 40);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x1c2333), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_led_create(parent_obj);
                            objects.obj118 = obj;
                            lv_obj_set_pos(obj, 10, 16);
                            lv_obj_set_size(obj, 8, 8);
                            lv_led_set_color(obj, lv_color_hex(0xf6b93b));
                            lv_led_set_brightness(obj, 255);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj119 = obj;
                            lv_obj_set_pos(obj, 30, 12);
                            lv_obj_set_size(obj, 96, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "12:30:18");
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 110, 12);
                            lv_obj_set_size(obj, 250, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "电机温度偏高 85°C");
                        }
                        {
                            lv_obj_t *obj = lv_button_create(parent_obj);
                            objects.obj120 = obj;
                            lv_obj_set_pos(obj, 820, 8);
                            lv_obj_set_size(obj, 70, 24);
                            lv_obj_add_event_cb(obj, event_handler_cb_alarms_obj120, LV_EVENT_ALL, flowState);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_set_style_radius(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0x1c2333), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "确认");
                                }
                            }
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj121 = obj;
                    lv_obj_set_pos(obj, 10, 178);
                    lv_obj_set_size(obj, 904, 40);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x1c2333), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_led_create(parent_obj);
                            objects.obj122 = obj;
                            lv_obj_set_pos(obj, 10, 16);
                            lv_obj_set_size(obj, 8, 8);
                            lv_led_set_color(obj, lv_color_hex(0xf6b93b));
                            lv_led_set_brightness(obj, 255);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj123 = obj;
                            lv_obj_set_pos(obj, 30, 12);
                            lv_obj_set_size(obj, 96, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "11:15:33");
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 110, 12);
                            lv_obj_set_size(obj, 250, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "CAN 通信延迟 15ms");
                        }
                        {
                            lv_obj_t *obj = lv_button_create(parent_obj);
                            objects.obj124 = obj;
                            lv_obj_set_pos(obj, 820, 8);
                            lv_obj_set_size(obj, 70, 24);
                            lv_obj_add_event_cb(obj, event_handler_cb_alarms_obj124, LV_EVENT_ALL, flowState);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_set_style_radius(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0x1c2333), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "确认");
                                }
                            }
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj125 = obj;
                    lv_obj_set_pos(obj, 10, 226);
                    lv_obj_set_size(obj, 904, 40);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x1c2333), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_led_create(parent_obj);
                            objects.obj126 = obj;
                            lv_obj_set_pos(obj, 10, 16);
                            lv_obj_set_size(obj, 8, 8);
                            lv_led_set_color(obj, lv_color_hex(0xf6b93b));
                            lv_led_set_brightness(obj, 255);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj127 = obj;
                            lv_obj_set_pos(obj, 30, 12);
                            lv_obj_set_size(obj, 96, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "09:00:07");
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 110, 12);
                            lv_obj_set_size(obj, 200, 21);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "RS485 未连接");
                        }
                        {
                            lv_obj_t *obj = lv_button_create(parent_obj);
                            objects.obj128 = obj;
                            lv_obj_set_pos(obj, 820, 8);
                            lv_obj_set_size(obj, 70, 24);
                            lv_obj_add_event_cb(obj, event_handler_cb_alarms_obj128, LV_EVENT_ALL, flowState);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_set_style_radius(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0x1c2333), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "确认");
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    tick_screen_alarms();
}

void tick_screen_alarms() {
    void *flowState = getFlowState(0, 2);
    (void)flowState;
    tick_user_widget_status_bar(getFlowState(flowState, 0), 26);
}

void create_user_widget_status_bar(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex) {
    (void)flowState;
    (void)startWidgetIndex;
    lv_obj_t *obj = parent_obj;
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 0] = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 1024, 36);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x151b28), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 1] = obj;
            lv_obj_set_pos(obj, 12, 7);
            lv_obj_set_size(obj, 237, 26);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_text_font(obj, &ui_font_demo_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0x2e86de), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "MotorDrive MD-200");
        }
        {
            lv_obj_t *obj = lv_led_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 2] = obj;
            lv_obj_set_pos(obj, 210, 14);
            lv_obj_set_size(obj, 8, 8);
            lv_led_set_color(obj, lv_color_hex(0x27ae60));
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 3] = obj;
            lv_obj_set_pos(obj, 222, 10);
            lv_obj_set_size(obj, 48, 21);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "运行");
        }
        {
            lv_obj_t *obj = lv_led_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 4] = obj;
            lv_obj_set_pos(obj, 268, 14);
            lv_obj_set_size(obj, 8, 8);
            lv_led_set_color(obj, lv_color_hex(0x27ae60));
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 5] = obj;
            lv_obj_set_pos(obj, 280, 10);
            lv_obj_set_size(obj, 46, 21);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "CAN");
        }
        {
            lv_obj_t *obj = lv_led_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 6] = obj;
            lv_obj_set_pos(obj, 330, 14);
            lv_obj_set_size(obj, 8, 8);
            lv_led_set_color(obj, lv_color_hex(0xf6b93b));
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 7] = obj;
            lv_obj_set_pos(obj, 342, 10);
            lv_obj_set_size(obj, 66, 21);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0x7a8499), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "RS485");
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 8] = obj;
            lv_obj_set_pos(obj, 700, 10);
            lv_obj_set_size(obj, 138, 21);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xe0e4ea), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "输出: 45.2 Hz");
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 9] = obj;
            lv_obj_set_pos(obj, 830, 10);
            lv_obj_set_size(obj, 176, 21);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_text_font(obj, &ui_font_demo_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xe0e4ea), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "");
        }
    }
}

void tick_user_widget_status_bar(void *flowState, int startWidgetIndex) {
    (void)flowState;
    (void)startWidgetIndex;
    {
        int32_t new_val = evalIntegerProperty(flowState, 2, 3, "Failed to evaluate Brightness in Led widget");
        if (new_val < 0) new_val = 0;
        else if (new_val > 255) new_val = 255;
        int32_t cur_val = lv_led_get_brightness(((lv_obj_t **)&objects)[startWidgetIndex + 2]);
        if (new_val != cur_val) {
            tick_value_change_obj = ((lv_obj_t **)&objects)[startWidgetIndex + 2];
            lv_led_set_brightness(((lv_obj_t **)&objects)[startWidgetIndex + 2], new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 4, 3, "Failed to evaluate Brightness in Led widget");
        if (new_val < 0) new_val = 0;
        else if (new_val > 255) new_val = 255;
        int32_t cur_val = lv_led_get_brightness(((lv_obj_t **)&objects)[startWidgetIndex + 4]);
        if (new_val != cur_val) {
            tick_value_change_obj = ((lv_obj_t **)&objects)[startWidgetIndex + 4];
            lv_led_set_brightness(((lv_obj_t **)&objects)[startWidgetIndex + 4], new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 6, 3, "Failed to evaluate Brightness in Led widget");
        if (new_val < 0) new_val = 0;
        else if (new_val > 255) new_val = 255;
        int32_t cur_val = lv_led_get_brightness(((lv_obj_t **)&objects)[startWidgetIndex + 6]);
        if (new_val != cur_val) {
            tick_value_change_obj = ((lv_obj_t **)&objects)[startWidgetIndex + 6];
            lv_led_set_brightness(((lv_obj_t **)&objects)[startWidgetIndex + 6], new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 9, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(((lv_obj_t **)&objects)[startWidgetIndex + 9]);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = ((lv_obj_t **)&objects)[startWidgetIndex + 9];
            lv_label_set_text(((lv_obj_t **)&objects)[startWidgetIndex + 9], new_val);
            tick_value_change_obj = NULL;
        }
    }
}

typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_overview,
    tick_screen_params,
    tick_screen_alarms,
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
    create_screen_overview();
    create_screen_params();
    create_screen_alarms();
}