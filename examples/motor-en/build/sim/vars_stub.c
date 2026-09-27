#include "lvgl/lvgl.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

static int32_t s_var_led_run;
int32_t get_var_led_run() { return s_var_led_run; }
void set_var_led_run(int32_t v) { s_var_led_run = v; }

static int32_t s_var_led_can;
int32_t get_var_led_can() { return s_var_led_can; }
void set_var_led_can(int32_t v) { s_var_led_can = v; }

static int32_t s_var_led_rs;
int32_t get_var_led_rs() { return s_var_led_rs; }
void set_var_led_rs(int32_t v) { s_var_led_rs = v; }

static char s_var_rtc[128];
const char *get_var_rtc() { return s_var_rtc; }
void set_var_rtc(const char *v) { strncpy(s_var_rtc, v, 127); s_var_rtc[127] = 0; }

static int32_t s_var_speed;
int32_t get_var_speed() { return s_var_speed; }
void set_var_speed(int32_t v) { s_var_speed = v; }

static int32_t s_var_torque;
int32_t get_var_torque() { return s_var_torque; }
void set_var_torque(int32_t v) { s_var_torque = v; }

static int32_t s_var_power;
int32_t get_var_power() { return s_var_power; }
void set_var_power(int32_t v) { s_var_power = v; }

static int32_t s_var_eff;
int32_t get_var_eff() { return s_var_eff; }
void set_var_eff(int32_t v) { s_var_eff = v; }

static int32_t s_var_motor_temp;
int32_t get_var_motor_temp() { return s_var_motor_temp; }
void set_var_motor_temp(int32_t v) { s_var_motor_temp = v; }

static int32_t s_var_bus_volt;
int32_t get_var_bus_volt() { return s_var_bus_volt; }
void set_var_bus_volt(int32_t v) { s_var_bus_volt = v; }

static int32_t s_var_out_curr;
int32_t get_var_out_curr() { return s_var_out_curr; }
void set_var_out_curr(int32_t v) { s_var_out_curr = v; }

static bool s_var_fwd_on;
bool get_var_fwd_on() { return s_var_fwd_on; }
void set_var_fwd_on(bool v) { s_var_fwd_on = v; }

static bool s_var_eco_on;
bool get_var_eco_on() { return s_var_eco_on; }
void set_var_eco_on(bool v) { s_var_eco_on = v; }
