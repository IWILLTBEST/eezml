#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// enum declarations

// Flow global variables

enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_NONE
};

// Native global variables

extern int32_t get_var_led_run();
extern void set_var_led_run(int32_t value);
extern int32_t get_var_led_can();
extern void set_var_led_can(int32_t value);
extern int32_t get_var_led_rs();
extern void set_var_led_rs(int32_t value);
extern const char *get_var_rtc();
extern void set_var_rtc(const char *value);
extern int32_t get_var_speed();
extern void set_var_speed(int32_t value);
extern int32_t get_var_torque();
extern void set_var_torque(int32_t value);
extern int32_t get_var_power();
extern void set_var_power(int32_t value);
extern int32_t get_var_eff();
extern void set_var_eff(int32_t value);
extern int32_t get_var_motor_temp();
extern void set_var_motor_temp(int32_t value);
extern int32_t get_var_bus_volt();
extern void set_var_bus_volt(int32_t value);
extern int32_t get_var_out_curr();
extern void set_var_out_curr(int32_t value);
extern bool get_var_fwd_on();
extern void set_var_fwd_on(bool value);
extern bool get_var_eco_on();
extern void set_var_eco_on(bool value);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/