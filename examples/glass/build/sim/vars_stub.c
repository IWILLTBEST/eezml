#include "lvgl/lvgl.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

static int32_t s_var_speed;
int32_t get_var_speed() { return s_var_speed; }
void set_var_speed(int32_t v) { s_var_speed = v; }

static int32_t s_var_temp;
int32_t get_var_temp() { return s_var_temp; }
void set_var_temp(int32_t v) { s_var_temp = v; }

static int32_t s_var_volt;
int32_t get_var_volt() { return s_var_volt; }
void set_var_volt(int32_t v) { s_var_volt = v; }

static double s_var_curr;
double get_var_curr() { return s_var_curr; }
void set_var_curr(double v) { s_var_curr = v; }

static int32_t s_var_power;
int32_t get_var_power() { return s_var_power; }
void set_var_power(int32_t v) { s_var_power = v; }

static int32_t s_var_heartbeat;
int32_t get_var_heartbeat() { return s_var_heartbeat; }
void set_var_heartbeat(int32_t v) { s_var_heartbeat = v; }

static int32_t s_var_warning;
int32_t get_var_warning() { return s_var_warning; }
void set_var_warning(int32_t v) { s_var_warning = v; }

static int32_t s_var_fault;
int32_t get_var_fault() { return s_var_fault; }
void set_var_fault(int32_t v) { s_var_fault = v; }
