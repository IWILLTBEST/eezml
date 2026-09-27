#include "lvgl/lvgl.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

static int32_t s_var_speed_val;
int32_t get_var_speed_val() { return s_var_speed_val; }
void set_var_speed_val(int32_t v) { s_var_speed_val = v; }
