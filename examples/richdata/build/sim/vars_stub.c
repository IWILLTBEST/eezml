#include "lvgl/lvgl.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

static int32_t s_var_mode_idx;
int32_t get_var_mode_idx() { return s_var_mode_idx; }
void set_var_mode_idx(int32_t v) { s_var_mode_idx = v; }

static int32_t s_var_pulse_count;
int32_t get_var_pulse_count() { return s_var_pulse_count; }
void set_var_pulse_count(int32_t v) { s_var_pulse_count = v; }
