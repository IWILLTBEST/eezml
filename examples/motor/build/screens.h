#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_OVERVIEW = 1,
    SCREEN_ID_PARAMS = 2,
    SCREEN_ID_ALARMS = 3,
    _SCREEN_ID_LAST = 3
};

typedef struct _objects_t {
    lv_obj_t *overview;
    lv_obj_t *params;
    lv_obj_t *alarms;
    lv_obj_t *obj0;
    lv_obj_t *obj0__obj0;
    lv_obj_t *obj0__obj1;
    lv_obj_t *obj0__obj2;
    lv_obj_t *obj0__obj3;
    lv_obj_t *obj0__obj4;
    lv_obj_t *obj0__obj5;
    lv_obj_t *obj0__obj6;
    lv_obj_t *obj0__obj7;
    lv_obj_t *obj0__obj8;
    lv_obj_t *obj0__obj9;
    lv_obj_t *obj1;
    lv_obj_t *obj1__obj0;
    lv_obj_t *obj1__obj1;
    lv_obj_t *obj1__obj2;
    lv_obj_t *obj1__obj3;
    lv_obj_t *obj1__obj4;
    lv_obj_t *obj1__obj5;
    lv_obj_t *obj1__obj6;
    lv_obj_t *obj1__obj7;
    lv_obj_t *obj1__obj8;
    lv_obj_t *obj1__obj9;
    lv_obj_t *obj2;
    lv_obj_t *obj2__obj0;
    lv_obj_t *obj2__obj1;
    lv_obj_t *obj2__obj2;
    lv_obj_t *obj2__obj3;
    lv_obj_t *obj2__obj4;
    lv_obj_t *obj2__obj5;
    lv_obj_t *obj2__obj6;
    lv_obj_t *obj2__obj7;
    lv_obj_t *obj2__obj8;
    lv_obj_t *obj2__obj9;
    lv_obj_t *obj3;
    lv_obj_t *obj4;
    lv_obj_t *obj5;
    lv_obj_t *obj6;
    lv_obj_t *obj7;
    lv_obj_t *obj8;
    lv_obj_t *panel_metrics_row;
    lv_obj_t *panel_card_speed;
    lv_obj_t *obj9;
    lv_obj_t *label_val_speed;
    lv_obj_t *obj10;
    lv_obj_t *panel_card_torque;
    lv_obj_t *obj11;
    lv_obj_t *label_val_torque;
    lv_obj_t *obj12;
    lv_obj_t *panel_card_power;
    lv_obj_t *obj13;
    lv_obj_t *label_val_power;
    lv_obj_t *obj14;
    lv_obj_t *panel_card_eff;
    lv_obj_t *obj15;
    lv_obj_t *label_val_eff;
    lv_obj_t *obj16;
    lv_obj_t *panel_gauge_row;
    lv_obj_t *panel_gauge_temp;
    lv_obj_t *obj17;
    lv_obj_t *obj18;
    lv_obj_t *obj19;
    lv_obj_t *obj20;
    lv_obj_t *panel_gauge_volt;
    lv_obj_t *obj21;
    lv_obj_t *obj22;
    lv_obj_t *obj23;
    lv_obj_t *obj24;
    lv_obj_t *panel_gauge_curr;
    lv_obj_t *obj25;
    lv_obj_t *obj26;
    lv_obj_t *obj27;
    lv_obj_t *obj28;
    lv_obj_t *panel_ctrl_row;
    lv_obj_t *panel_ctrl_speed;
    lv_obj_t *obj29;
    lv_obj_t *obj30;
    lv_obj_t *obj31;
    lv_obj_t *obj32;
    lv_obj_t *label_sl_speed;
    lv_obj_t *obj33;
    lv_obj_t *obj34;
    lv_obj_t *obj35;
    lv_obj_t *panel_ctrl_run;
    lv_obj_t *obj36;
    lv_obj_t *obj37;
    lv_obj_t *obj38;
    lv_obj_t *obj39;
    lv_obj_t *obj40;
    lv_obj_t *obj41;
    lv_obj_t *obj42;
    lv_obj_t *obj43;
    lv_obj_t *obj44;
    lv_obj_t *obj45;
    lv_obj_t *obj46;
    lv_obj_t *obj47;
    lv_obj_t *obj48;
    lv_obj_t *obj49;
    lv_obj_t *obj50;
    lv_obj_t *obj51;
    lv_obj_t *obj52;
    lv_obj_t *obj53;
    lv_obj_t *obj54;
    lv_obj_t *obj55;
    lv_obj_t *obj56;
    lv_obj_t *dropdown_dd_poles;
    lv_obj_t *obj57;
    lv_obj_t *dropdown_dd_ctrl_mode;
    lv_obj_t *obj58;
    lv_obj_t *obj59;
    lv_obj_t *obj60;
    lv_obj_t *obj61;
    lv_obj_t *obj62;
    lv_obj_t *obj63;
    lv_obj_t *obj64;
    lv_obj_t *obj65;
    lv_obj_t *obj66;
    lv_obj_t *obj67;
    lv_obj_t *obj68;
    lv_obj_t *obj69;
    lv_obj_t *obj70;
    lv_obj_t *obj71;
    lv_obj_t *obj72;
    lv_obj_t *obj73;
    lv_obj_t *obj74;
    lv_obj_t *obj75;
    lv_obj_t *obj76;
    lv_obj_t *obj77;
    lv_obj_t *obj78;
    lv_obj_t *obj79;
    lv_obj_t *obj80;
    lv_obj_t *dropdown_dd_can_baud;
    lv_obj_t *obj81;
    lv_obj_t *obj82;
    lv_obj_t *obj83;
    lv_obj_t *obj84;
    lv_obj_t *dropdown_dd_protocol;
    lv_obj_t *obj85;
    lv_obj_t *obj86;
    lv_obj_t *obj87;
    lv_obj_t *obj88;
    lv_obj_t *obj89;
    lv_obj_t *obj90;
    lv_obj_t *obj91;
    lv_obj_t *obj92;
    lv_obj_t *obj93;
    lv_obj_t *obj94;
    lv_obj_t *obj95;
    lv_obj_t *obj96;
    lv_obj_t *obj97;
    lv_obj_t *obj98;
    lv_obj_t *obj99;
    lv_obj_t *obj100;
    lv_obj_t *obj101;
    lv_obj_t *obj102;
    lv_obj_t *obj103;
    lv_obj_t *obj104;
    lv_obj_t *obj105;
    lv_obj_t *obj106;
    lv_obj_t *obj107;
    lv_obj_t *obj108;
    lv_obj_t *obj109;
    lv_obj_t *obj110;
    lv_obj_t *panel_alarm_row1;
    lv_obj_t *obj111;
    lv_obj_t *obj112;
    lv_obj_t *button_ack1;
    lv_obj_t *obj113;
    lv_obj_t *obj114;
    lv_obj_t *obj115;
    lv_obj_t *obj116;
    lv_obj_t *obj117;
    lv_obj_t *obj118;
    lv_obj_t *obj119;
    lv_obj_t *obj120;
    lv_obj_t *obj121;
    lv_obj_t *obj122;
    lv_obj_t *obj123;
    lv_obj_t *obj124;
    lv_obj_t *obj125;
    lv_obj_t *obj126;
    lv_obj_t *obj127;
    lv_obj_t *obj128;
} objects_t;

extern objects_t objects;

void create_screen_overview();
void tick_screen_overview();

void create_screen_params();
void tick_screen_params();

void create_screen_alarms();
void tick_screen_alarms();

void create_user_widget_status_bar(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_status_bar(void *flowState, int startWidgetIndex);

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/