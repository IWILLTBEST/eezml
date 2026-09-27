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

extern int32_t get_var_speed();
extern void set_var_speed(int32_t value);
extern int32_t get_var_temp();
extern void set_var_temp(int32_t value);
extern int32_t get_var_volt();
extern void set_var_volt(int32_t value);
extern double get_var_curr();
extern void set_var_curr(double value);
extern int32_t get_var_power();
extern void set_var_power(int32_t value);
extern int32_t get_var_heartbeat();
extern void set_var_heartbeat(int32_t value);
extern int32_t get_var_warning();
extern void set_var_warning(int32_t value);
extern int32_t get_var_fault();
extern void set_var_fault(int32_t value);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/