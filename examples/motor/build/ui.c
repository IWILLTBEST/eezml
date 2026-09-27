#include "ui.h"
#include "screens.h"
#include "images.h"
#include "flow_def.h"
#include "actions.h"

ActionExecFunc actions[] = {
    action_on_speed,
    action_on_torque,
    action_on_motor_temp,
    action_on_bus_volt,
    action_on_out_curr,
    action_on_fwd,
    action_on_eco,
    action_on_poles,
    action_on_ctrl_mode,
    action_on_can_baud,
    action_on_protocol,
    action_ack_alarm,
};

void ui_init() {
    eez_flow_init(assets, sizeof(assets), (lv_obj_t **)&objects, sizeof(objects), images, sizeof(images), actions);
}

void ui_tick() {
    eez_flow_tick();
    tick_screen(g_currentScreen);
}