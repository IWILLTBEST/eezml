/**
 * @file eezml.c
 * @brief eezml (uixml) runtime loader — portable core implementation.
 *
 * SAX-driven: expat streams the document, we maintain a parent stack and
 * build the LVGL object tree on element start. Attribute names follow the
 * uixml schema (id/x/y/w/h/src/text/... plus lv:-namespaced style props).
 *
 * Phase 1 scope: static UI (widgets + styles + assets). The <var>/<action>/
 * the "on-..." and "bind" attributes are parsed past but not wired.
 */

#include "eezml.h"

#include <string.h>
#include <stdlib.h>
#include "expat.h"

/* ------------------------------------------------------------------ */
/* named registries                                                    */
/* ------------------------------------------------------------------ */

#define EEZML_MAX_BITMAPS 32
#define EEZML_MAX_FONTS   32
#define EEZML_MAX_IDS     64
#define EEZML_NAME_LEN    32   /* defined early: used by registries + behavior */

typedef struct {
    const char * name;
    const lv_image_dsc_t * dsc;
} eezml_bitmap_entry_t;

typedef struct {
    const char * name;
    const lv_font_t * font;
} eezml_font_entry_t;

typedef struct {
    char id[EEZML_NAME_LEN];
    lv_obj_t * obj;
} eezml_id_entry_t;

static eezml_bitmap_entry_t s_bitmaps[EEZML_MAX_BITMAPS];
static int s_bitmap_cnt;
static eezml_font_entry_t s_fonts[EEZML_MAX_FONTS];
static int s_font_cnt;
static eezml_id_entry_t s_ids[EEZML_MAX_IDS];
static int s_id_cnt;

void eezml_register_bitmap(const char * name, const lv_image_dsc_t * dsc)
{
    if (s_bitmap_cnt < EEZML_MAX_BITMAPS) {
        s_bitmaps[s_bitmap_cnt].name = name;
        s_bitmaps[s_bitmap_cnt].dsc = dsc;
        s_bitmap_cnt++;
    }
}

void eezml_register_font(const char * name, const lv_font_t * font)
{
    if (s_font_cnt < EEZML_MAX_FONTS) {
        s_fonts[s_font_cnt].name = name;
        s_fonts[s_font_cnt].font = font;
        s_font_cnt++;
    }
}

lv_obj_t * eezml_get_object(const char * id)
{
    for (int i = 0; i < s_id_cnt; i++) {
        if (strcmp(s_ids[i].id, id) == 0) return s_ids[i].obj;
    }
    return NULL;
}

/* ------------------------------------------------------------------ */
/* value helpers                                                       */
/* ------------------------------------------------------------------ */

static bool parse_color(const char * v, lv_color_t * out)
{
    /* #RRGGBB / RRGGBB */
    if (v[0] == '#') v++;
    size_t n = strlen(v);
    if (n != 6 && n != 3) return false;
    uint32_t c = 0;
    if (n == 6) {
        c = (uint32_t)strtoul(v, NULL, 16);
    } else {
        /* #abc -> aabbcc, like CSS */
        uint32_t h = (uint32_t)strtoul(v, NULL, 16);
        c = ((h & 0xF00) << 12) | ((h & 0xF00) << 8) |
            ((h & 0x0F0) << 8)  | ((h & 0x0F0) << 4) |
            ((h & 0x00F) << 4)  |  (h & 0x00F);
    }
    *out = lv_color_hex(c);
    return true;
}

static int32_t parse_coord(const char * v)
{
    if (strcmp(v, "content") == 0) return LV_SIZE_CONTENT;
    return (int32_t)strtol(v, NULL, 10);
}

static bool parse_align(const char * v, lv_align_t * out)
{
    static const struct { const char * n; lv_align_t a; } k_align[] = {
        { "DEFAULT",        LV_ALIGN_DEFAULT },
        { "TOP_LEFT",       LV_ALIGN_TOP_LEFT },
        { "TOP_MID",        LV_ALIGN_TOP_MID },
        { "TOP_RIGHT",      LV_ALIGN_TOP_RIGHT },
        { "BOTTOM_LEFT",    LV_ALIGN_BOTTOM_LEFT },
        { "BOTTOM_MID",     LV_ALIGN_BOTTOM_MID },
        { "BOTTOM_RIGHT",   LV_ALIGN_BOTTOM_RIGHT },
        { "LEFT_MID",       LV_ALIGN_LEFT_MID },
        { "RIGHT_MID",      LV_ALIGN_RIGHT_MID },
        { "CENTER",         LV_ALIGN_CENTER },
        { "OUT_TOP_LEFT",   LV_ALIGN_OUT_TOP_LEFT },
        { "OUT_TOP_MID",    LV_ALIGN_OUT_TOP_MID },
        { "OUT_TOP_RIGHT",  LV_ALIGN_OUT_TOP_RIGHT },
        { "OUT_BOTTOM_LEFT",LV_ALIGN_OUT_BOTTOM_LEFT },
        { "OUT_BOTTOM_MID", LV_ALIGN_OUT_BOTTOM_MID },
        { "OUT_BOTTOM_RIGHT",LV_ALIGN_OUT_BOTTOM_RIGHT },
        { "OUT_LEFT_TOP",   LV_ALIGN_OUT_LEFT_TOP },
        { "OUT_LEFT_MID",   LV_ALIGN_OUT_LEFT_MID },
        { "OUT_LEFT_BOTTOM",LV_ALIGN_OUT_LEFT_BOTTOM },
        { "OUT_RIGHT_TOP",  LV_ALIGN_OUT_RIGHT_TOP },
        { "OUT_RIGHT_MID",  LV_ALIGN_OUT_RIGHT_MID },
        { "OUT_RIGHT_BOTTOM",LV_ALIGN_OUT_RIGHT_BOTTOM },
    };
    for (size_t i = 0; i < sizeof(k_align) / sizeof(k_align[0]); i++) {
        if (strcmp(k_align[i].n, v) == 0) { *out = k_align[i].a; return true; }
    }
    return false;
}

static const lv_image_dsc_t * lookup_bitmap(const char * name)
{
    for (int i = 0; i < s_bitmap_cnt; i++) {
        if (strcmp(s_bitmaps[i].name, name) == 0) return s_bitmaps[i].dsc;
    }
    return NULL;
}

static const lv_font_t * lookup_font(const char * name)
{
    for (int i = 0; i < s_font_cnt; i++) {
        if (strcmp(s_fonts[i].name, name) == 0) return s_fonts[i].font;
    }
    return NULL;
}

/* ------------------------------------------------------------------ */
/* widget factory table                                                */
/* ------------------------------------------------------------------ */

typedef lv_obj_t * (*eezml_create_fn)(lv_obj_t * parent);

typedef struct {
    const char * tag;
    eezml_create_fn create;
    /* has a text/label part */
    bool is_labelish;
    /* src references a registered bitmap */
    bool is_imageish;
} eezml_widget_type_t;

static const eezml_widget_type_t k_widget_types[] = {
    { "panel",     lv_obj_create,      false, false },
    { "container", lv_obj_create,      false, false },
    { "obj",       lv_obj_create,      false, false },
    { "label",     lv_label_create,    true,  false },
    { "button",    lv_button_create,   true,  false },
    { "image",     lv_image_create,    false, true  },
    { "gif",       lv_gif_create,      false, true  },
    { "bar",       lv_bar_create,      false, false },
    { "slider",    lv_slider_create,   false, false },
    { "arc",       lv_arc_create,      false, false },
    { "led",       lv_led_create,      false, false },
    { "switch",    lv_switch_create,   false, false },
    { "checkbox",  lv_checkbox_create, true,  false },
    { "spinner",   lv_spinner_create,  false, false },
    { "textarea",  lv_textarea_create, false, false },
    { "dropdown",  lv_dropdown_create, false, false },
};

static const eezml_widget_type_t * lookup_widget(const char * tag)
{
    for (size_t i = 0; i < sizeof(k_widget_types) / sizeof(k_widget_types[0]); i++) {
        if (strcmp(k_widget_types[i].tag, tag) == 0) return &k_widget_types[i];
    }
    return NULL;
}

/* ------------------------------------------------------------------ */
/* style attribute table (lv:-namespaced props)                        */
/* ------------------------------------------------------------------ */

/* real 9.x style setters carry a trailing lv_style_selector_t; phase 1
 * always targets the default state, so wrap them into uniform signatures */
static void w_bg_color(lv_obj_t * o, lv_color_t v)        { lv_obj_set_style_bg_color(o, v, 0); }
static void w_bg_grad_color(lv_obj_t * o, lv_color_t v)   { lv_obj_set_style_bg_grad_color(o, v, 0); }
static void w_border_color(lv_obj_t * o, lv_color_t v)    { lv_obj_set_style_border_color(o, v, 0); }
static void w_text_color(lv_obj_t * o, lv_color_t v)      { lv_obj_set_style_text_color(o, v, 0); }
static void w_shadow_color(lv_obj_t * o, lv_color_t v)    { lv_obj_set_style_shadow_color(o, v, 0); }

static void w_bg_opa(lv_obj_t * o, int32_t v)             { lv_obj_set_style_bg_opa(o, (lv_opa_t)v, 0); }
static void w_border_opa(lv_obj_t * o, int32_t v)         { lv_obj_set_style_border_opa(o, (lv_opa_t)v, 0); }
static void w_shadow_opa(lv_obj_t * o, int32_t v)         { lv_obj_set_style_shadow_opa(o, (lv_opa_t)v, 0); }
static void w_opa(lv_obj_t * o, int32_t v)                { lv_obj_set_style_opa(o, (lv_opa_t)v, 0); }
static void w_bg_grad_dir(lv_obj_t * o, int32_t v)        { lv_obj_set_style_bg_grad_dir(o, (lv_grad_dir_t)v, 0); }
static void w_bg_main_stop(lv_obj_t * o, int32_t v)       { lv_obj_set_style_bg_main_stop(o, v, 0); }
static void w_bg_grad_stop(lv_obj_t * o, int32_t v)       { lv_obj_set_style_bg_grad_stop(o, v, 0); }
static void w_border_width(lv_obj_t * o, int32_t v)       { lv_obj_set_style_border_width(o, v, 0); }
static void w_pad_left(lv_obj_t * o, int32_t v)           { lv_obj_set_style_pad_left(o, v, 0); }
static void w_pad_right(lv_obj_t * o, int32_t v)          { lv_obj_set_style_pad_right(o, v, 0); }
static void w_pad_top(lv_obj_t * o, int32_t v)            { lv_obj_set_style_pad_top(o, v, 0); }
static void w_pad_bottom(lv_obj_t * o, int32_t v)         { lv_obj_set_style_pad_bottom(o, v, 0); }
static void w_pad_row(lv_obj_t * o, int32_t v)            { lv_obj_set_style_pad_row(o, v, 0); }
static void w_pad_column(lv_obj_t * o, int32_t v)         { lv_obj_set_style_pad_column(o, v, 0); }
static void w_shadow_width(lv_obj_t * o, int32_t v)       { lv_obj_set_style_shadow_width(o, v, 0); }
static void w_shadow_spread(lv_obj_t * o, int32_t v)      { lv_obj_set_style_shadow_spread(o, v, 0); }
static void w_radius(lv_obj_t * o, int32_t v)             { lv_obj_set_style_radius(o, v, 0); }
static void w_text_align(lv_obj_t * o, int32_t v)         { lv_obj_set_style_text_align(o, (lv_text_align_t)v, 0); }

typedef enum {
    STYLE_INT,
    STYLE_COLOR,
    STYLE_SPECIAL,
} style_kind_t;

typedef void (*style_int_fn)(lv_obj_t *, int32_t);
typedef void (*style_color_fn)(lv_obj_t *, lv_color_t);

typedef struct {
    const char * attr;
    style_kind_t kind;
    style_int_fn int_fn;
    style_color_fn color_fn;
} style_prop_t;

static const style_prop_t k_style_props[] = {
    { "bg",                STYLE_COLOR, NULL, w_bg_color },
    { "bg-color",          STYLE_COLOR, NULL, w_bg_color },
    { "bg-opa",            STYLE_INT,   w_bg_opa, NULL },
    { "bg-grad-color",     STYLE_COLOR, NULL, w_bg_grad_color },
    { "bg-grad-dir",       STYLE_INT,   w_bg_grad_dir, NULL },
    { "bg-main-stop",      STYLE_INT,   w_bg_main_stop, NULL },
    { "bg-grad-stop",      STYLE_INT,   w_bg_grad_stop, NULL },
    { "border-color",      STYLE_COLOR, NULL, w_border_color },
    { "border-width",      STYLE_INT,   w_border_width, NULL },
    { "border-opa",        STYLE_INT,   w_border_opa, NULL },
    { "text-color",        STYLE_COLOR, NULL, w_text_color },
    { "pad-left",          STYLE_INT,   w_pad_left, NULL },
    { "pad-right",         STYLE_INT,   w_pad_right, NULL },
    { "pad-top",           STYLE_INT,   w_pad_top, NULL },
    { "pad-bottom",        STYLE_INT,   w_pad_bottom, NULL },
    { "pad-row",           STYLE_INT,   w_pad_row, NULL },
    { "pad-column",        STYLE_INT,   w_pad_column, NULL },
    { "shadow-color",      STYLE_COLOR, NULL, w_shadow_color },
    { "shadow-width",      STYLE_INT,   w_shadow_width, NULL },
    { "shadow-spread",     STYLE_INT,   w_shadow_spread, NULL },
    { "shadow-opa",        STYLE_INT,   w_shadow_opa, NULL },
    { "radius",            STYLE_INT,   w_radius, NULL },
    { "opa",               STYLE_INT,   w_opa, NULL },
    { "text-align",        STYLE_INT,   w_text_align, NULL },
    { "text-font",         STYLE_SPECIAL, NULL, NULL },
    { "align",             STYLE_SPECIAL, NULL, NULL },
    { "x",                 STYLE_SPECIAL, NULL, NULL },
    { "y",                 STYLE_SPECIAL, NULL, NULL },
    { "width",             STYLE_SPECIAL, NULL, NULL },
    { "height",            STYLE_SPECIAL, NULL, NULL },
};

static const style_prop_t * lookup_style_prop(const char * name)
{
    for (size_t i = 0; i < sizeof(k_style_props) / sizeof(k_style_props[0]); i++) {
        if (strcmp(k_style_props[i].attr, name) == 0) return &k_style_props[i];
    }
    return NULL;
}

/* ================================================================== */
/* Phase 2: variables, actions, bindings, native actions              */
/* ================================================================== */

#define EEZML_MAX_VARS     16
#define EEZML_MAX_ACTIONS  16
#define EEZML_MAX_STEPS    24
#define EEZML_MAX_NATIVES  16

typedef struct {
    char name[EEZML_NAME_LEN];
    lv_subject_t subject;
    char sbuf[64];             /* string variable buffer */
    char sprev[64];
    uint8_t is_string;
} eezml_var_t;

typedef struct {
    char verb[20];
    char target[EEZML_NAME_LEN];
    char a[48];                /* prop / var / native / state / flag / screen */
    char b[64];                /* value / text / ... */
    int32_t n_from, n_to, n_time, n_delay, n_repeat;
    uint8_t playback;
} eezml_step_t;

typedef struct {
    char name[EEZML_NAME_LEN];
    int step_cnt;
    eezml_step_t steps[EEZML_MAX_STEPS];
} eezml_action_t;

typedef struct {
    char name[EEZML_NAME_LEN];
    eezml_native_fn fn;
    void * user_data;
} eezml_native_t;

static eezml_var_t     s_vars[EEZML_MAX_VARS];
static int             s_var_cnt;
static eezml_action_t  s_actions[EEZML_MAX_ACTIONS];
static int             s_action_cnt;
static eezml_native_t  s_natives[EEZML_MAX_NATIVES];
static int             s_native_cnt;
static eezml_change_screen_fn s_change_screen_fn;
static void *          s_change_screen_user;

/* ---- variable snapshot (hot reload state carry-over) ------------ */
#define EEZML_MAX_SNAP 16
typedef struct {
    char name[EEZML_NAME_LEN];
    uint8_t is_string;
    int32_t i;
    char s[64];
} eezml_snap_t;
static eezml_snap_t s_snap[EEZML_MAX_SNAP];
static int s_snap_cnt;

/* ---- variables & subjects --------------------------------------- */

static eezml_var_t * lookup_var(const char * name)
{
    for (int i = 0; i < s_var_cnt; i++) {
        if (strcmp(s_vars[i].name, name) == 0) return &s_vars[i];
    }
    return NULL;
}

int32_t eezml_get_var_int(const char * name)
{
    eezml_var_t * v = lookup_var(name);
    if (!v || v->is_string) return 0;
    return v->subject.value.num;
}

void eezml_set_var_int(const char * name, int32_t value)
{
    eezml_var_t * v = lookup_var(name);
    if (v && !v->is_string) lv_subject_set_int(&v->subject, value);
}

void eezml_set_var_string(const char * name, const char * value)
{
    eezml_var_t * v = lookup_var(name);
    if (v && v->is_string) lv_subject_set_string(&v->subject, value);
}

static void var_declare(const char * name, const char * type, const char * dflt)
{
    if (!name || s_var_cnt >= EEZML_MAX_VARS) return;
    /* hot reload: restore the pre-unload value for a same-named variable */
    for (int si = 0; si < s_snap_cnt; si++) {
        if (strcmp(s_snap[si].name, name) == 0) {
            type = s_snap[si].is_string ? "string" : "integer";
            dflt = s_snap[si].is_string ? s_snap[si].s : NULL;
            if (!s_snap[si].is_string) {
                /* declare with default 0, then apply the snapshot value */
                eezml_var_t * v = &s_vars[s_var_cnt];
                memset(v, 0, sizeof(*v));
                strncpy(v->name, name, EEZML_NAME_LEN - 1);
                lv_subject_init_int(&v->subject, 0);
                lv_subject_set_int(&v->subject, s_snap[si].i);
                s_var_cnt++;
                return;
            }
            break;
        }
    }
    eezml_var_t * v = &s_vars[s_var_cnt];
    memset(v, 0, sizeof(*v));
    strncpy(v->name, name, EEZML_NAME_LEN - 1);
    if (type && strcmp(type, "string") == 0) {
        v->is_string = 1;
        lv_subject_init_string(&v->subject, v->sbuf, v->sprev, sizeof(v->sbuf),
                               dflt ? dflt : "");
    }
#if LV_USE_FLOAT
    else if (type && (strcmp(type, "double") == 0 || strcmp(type, "float") == 0)) {
        lv_subject_init_float(&v->subject, dflt ? (float)atof(dflt) : 0.0f);
    }
#endif
    else {
        lv_subject_init_int(&v->subject, dflt ? (int32_t)atol(dflt) : 0);
    }
    s_var_cnt++;
}

/* ---- anim property callbacks ------------------------------------ */

static void anim_set_x(lv_obj_t * o, int32_t v)      { lv_obj_set_x(o, v); }
static void anim_set_y(lv_obj_t * o, int32_t v)      { lv_obj_set_y(o, v); }
static void anim_set_w(lv_obj_t * o, int32_t v)      { lv_obj_set_width(o, v); }
static void anim_set_h(lv_obj_t * o, int32_t v)      { lv_obj_set_height(o, v); }
static void anim_set_opa(lv_obj_t * o, int32_t v)    { lv_obj_set_style_opa(o, (lv_opa_t)v, 0); }
static void anim_set_rot(lv_obj_t * o, int32_t v)    { lv_obj_set_style_transform_rotation(o, v, 0); }

typedef struct { const char * prop; void (*fn)(lv_obj_t *, int32_t); } anim_prop_t;
static const anim_prop_t k_anim_props[] = {
    { "x", anim_set_x }, { "y", anim_set_y },
    { "width", anim_set_w }, { "w", anim_set_w },
    { "height", anim_set_h }, { "h", anim_set_h },
    { "opacity", anim_set_opa },
    { "rotation", anim_set_rot },
};

/* ---- state / flag name tables ----------------------------------- */

typedef struct { const char * n; lv_state_t s; } state_name_t;
static const state_name_t k_states[] = {
    { "checked", LV_STATE_CHECKED }, { "disabled", LV_STATE_DISABLED },
    { "focused", LV_STATE_FOCUSED }, { "pressed", LV_STATE_PRESSED },
};
typedef struct { const char * n; lv_obj_flag_t f; } flag_name_t;
static const flag_name_t k_flags[] = {
    { "hidden", LV_OBJ_FLAG_HIDDEN }, { "clickable", LV_OBJ_FLAG_CLICKABLE },
    { "checkable", LV_OBJ_FLAG_CHECKABLE },
};

/* ---- step execution ---------------------------------------------- */

typedef struct {
    int act_idx, step_idx;
    lv_timer_t * timer;
} eezml_cont_t;

static void run_action_from(int act_idx, int step_idx);

static void cont_cb(lv_timer_t * t)
{
    eezml_cont_t * c = (eezml_cont_t *)lv_timer_get_user_data(t);
    int act = c->act_idx, step = c->step_idx;
    lv_free(c);
    lv_timer_del(t);
    run_action_from(act, step);
}

static void exec_step(int act_idx, int idx)
{
    eezml_action_t * a = &s_actions[act_idx];
    if (idx < 0 || idx >= a->step_cnt) return;
    eezml_step_t * s = &a->steps[idx];
    lv_obj_t * obj = s->target[0] ? eezml_get_object(s->target) : NULL;

    if (strcmp(s->verb, "anim") == 0) {
        if (!obj) return;
        for (size_t i = 0; i < sizeof(k_anim_props) / sizeof(k_anim_props[0]); i++) {
            if (strcmp(k_anim_props[i].prop, s->a) == 0) {
                lv_anim_t an;
                lv_anim_init(&an);
                an.var = obj;
                an.exec_cb = (lv_anim_exec_xcb_t)k_anim_props[i].fn;
                an.start_value = s->n_from;
                an.end_value = s->n_to;
                an.duration = s->n_time > 0 ? s->n_time : 400;
                an.act_time = -(int32_t)(s->n_delay);
                if (s->n_repeat < 0) an.repeat_cnt = LV_ANIM_REPEAT_INFINITE;
                else if (s->n_repeat > 0) an.repeat_cnt = s->n_repeat;
                if (s->playback) {
                    an.reverse_duration = an.duration;
                    an.reverse_delay = 0;
                }
                lv_anim_start(&an);
                return;
            }
        }
    } else if (strcmp(s->verb, "set") == 0) {
        eezml_var_t * v = lookup_var(s->a);
        if (v) {
            if (v->is_string) lv_subject_set_string(&v->subject, s->b);
#if LV_USE_FLOAT
            else if (v->subject.type == LV_SUBJECT_TYPE_FLOAT)
                lv_subject_set_float(&v->subject, (float)atof(s->b[0] ? s->b : "0"));
#endif
            else lv_subject_set_int(&v->subject, s->b[0] ? (int32_t)atol(s->b) : 0);
        }
    } else if (strcmp(s->verb, "label-set-text") == 0) {
        if (obj) lv_label_set_text(obj, s->b);
    } else if (strcmp(s->verb, "obj-set-y") == 0) {
        if (obj) lv_obj_set_y(obj, s->n_from);
    } else if (strcmp(s->verb, "obj-add-state") == 0 || strcmp(s->verb, "obj-clear-state") == 0) {
        if (!obj) return;
        for (size_t i = 0; i < sizeof(k_states) / sizeof(k_states[0]); i++) {
            if (strcmp(k_states[i].n, s->a) == 0) {
                if (s->verb[4] == 'a') lv_obj_add_state(obj, k_states[i].s);
                else lv_obj_remove_state(obj, k_states[i].s);
                return;
            }
        }
    } else if (strcmp(s->verb, "obj-add-flag") == 0 || strcmp(s->verb, "obj-clear-flag") == 0) {
        if (!obj) return;
        for (size_t i = 0; i < sizeof(k_flags) / sizeof(k_flags[0]); i++) {
            if (strcmp(k_flags[i].n, s->a) == 0) {
                if (s->verb[4] == 'a') lv_obj_add_flag(obj, k_flags[i].f);
                else lv_obj_remove_flag(obj, k_flags[i].f);
                return;
            }
        }
    } else if (strcmp(s->verb, "delay") == 0) {
        /* schedule the remaining steps after n_time ms */
        eezml_cont_t * c = (eezml_cont_t *)lv_malloc(sizeof(eezml_cont_t));
        if (c) {
            c->act_idx = act_idx;
            c->step_idx = idx + 1;
            c->timer = lv_timer_create(cont_cb, s->n_time > 0 ? s->n_time : 100, c);
            if (!c->timer) { lv_free(c); return; }
            lv_timer_set_repeat_count(c->timer, 1);
        }
        return; /* the rest of the sequence resumes in the timer */
    } else if (strcmp(s->verb, "call") == 0) {
        for (int i = 0; i < s_native_cnt; i++) {
            if (strcmp(s_natives[i].name, s->a) == 0) {
                s_natives[i].fn(s_natives[i].user_data);
                return;
            }
        }
    } else if (strcmp(s->verb, "change-screen") == 0) {
        if (s_change_screen_fn) s_change_screen_fn(s->a, s_change_screen_user);
    }
    /* unknown verbs are skipped */
}

static void run_action_from(int act_idx, int step_idx)
{
    eezml_action_t * a = &s_actions[act_idx];
    for (int i = step_idx; i < a->step_cnt; i++) {
        if (strcmp(a->steps[i].verb, "delay") == 0) {
            exec_step(act_idx, i);      /* hands the tail to a timer */
            return;
        }
        exec_step(act_idx, i);
    }
}

void eezml_run_action(const char * name)
{
    for (int i = 0; i < s_action_cnt; i++) {
        if (strcmp(s_actions[i].name, name) == 0) {
            run_action_from(i, 0);
            return;
        }
    }
}

void eezml_register_native(const char * name, eezml_native_fn fn, void * user_data)
{
    if (s_native_cnt < EEZML_MAX_NATIVES) {
        strncpy(s_natives[s_native_cnt].name, name, EEZML_NAME_LEN - 1);
        s_natives[s_native_cnt].fn = fn;
        s_natives[s_native_cnt].user_data = user_data;
        s_native_cnt++;
    }
}

void eezml_set_change_screen_handler(eezml_change_screen_fn fn, void * user_data)
{
    s_change_screen_fn = fn;
    s_change_screen_user = user_data;
}

/* ---- event bridge ------------------------------------------------ */

#define EEZML_MAX_EVTS 64
typedef struct { char action[EEZML_NAME_LEN]; uint8_t used; } eezml_evt_t;
static eezml_evt_t s_evts[EEZML_MAX_EVTS];

static void event_cb(lv_event_t * e)
{
    eezml_evt_t * ev = (eezml_evt_t *)lv_event_get_user_data(e);
    if (ev) eezml_run_action(ev->action);
}

static const struct { const char * attr; lv_event_code_t code; } k_events[] = {
    { "clicked", LV_EVENT_CLICKED },
    { "pressed", LV_EVENT_PRESSED },
    { "released", LV_EVENT_RELEASED },
    { "long-pressed", LV_EVENT_LONG_PRESSED },
    { "value-changed", LV_EVENT_VALUE_CHANGED },
    { "focused", LV_EVENT_FOCUSED },
    { "defocused", LV_EVENT_DEFOCUSED },
};

/* ---- bind wiring -------------------------------------------------- */

static void label_set_int_text(lv_obj_t * obj, int32_t v)
{
    lv_label_set_text_fmt(obj, "%" LV_PRId32, v);
}
static void bind_label(lv_obj_t * obj, lv_subject_t * subj)
{
    if (subj->type == LV_SUBJECT_TYPE_STRING)
        lv_obj_bind_string(obj, subj, (lv_obj_set_string_t)lv_label_set_text);
    else if (subj->type == LV_SUBJECT_TYPE_INT)
        lv_obj_bind_int(obj, subj, label_set_int_text);
}
static void bind_arc(lv_obj_t * obj, lv_subject_t * subj)
{
    lv_obj_bind_int(obj, subj, (lv_obj_set_int_t)lv_arc_set_value);
}
static void bar_set_value_2(lv_obj_t * o, int32_t v)  { lv_bar_set_value(o, v, LV_ANIM_OFF); }
static void slider_set_value_2(lv_obj_t * o, int32_t v){ lv_slider_set_value(o, v, LV_ANIM_OFF); }
static void led_set_brightness_2(lv_obj_t * o, int32_t v){ lv_led_set_brightness(o, (uint8_t)v); }
static void bind_bar(lv_obj_t * obj, lv_subject_t * subj)
{
    lv_obj_bind_int(obj, subj, bar_set_value_2);
}
static void bind_slider(lv_obj_t * obj, lv_subject_t * subj)
{
    lv_obj_bind_int(obj, subj, slider_set_value_2);
}
static void bind_led(lv_obj_t * obj, lv_subject_t * subj)
{
    lv_obj_bind_int(obj, subj, led_set_brightness_2);
}

static void wire_bind(lv_obj_t * obj, const char * tag, const char * var_name)
{
    eezml_var_t * v = lookup_var(var_name);
    if (!v) return; /* variables must be declared before their consumers */
    lv_subject_t * s = &v->subject;
    if (strcmp(tag, "label") == 0)       bind_label(obj, s);
    else if (strcmp(tag, "arc") == 0)    bind_arc(obj, s);
    else if (strcmp(tag, "bar") == 0)    bind_bar(obj, s);
    else if (strcmp(tag, "slider") == 0) bind_slider(obj, s);
    else if (strcmp(tag, "led") == 0)    bind_led(obj, s);
}

/* ------------------------------------------------------------------ */
/* parse context (SAX)                                                 */
/* ------------------------------------------------------------------ */

#define MAX_PARENTS 32

typedef struct {
    lv_obj_t * parents[MAX_PARENTS];
    int depth;
    lv_obj_t * root;            /* first created object (screen body root) */
    int in_screen;              /* inside <screen> */
    char screen_name[64];
    /* phase 2: <action> collection */
    eezml_action_t * cur_action;
    int action_open;
} eezml_ctx_t;

/* declarative step verbs (mirrors the uixml _STEP_VERBS set) */
static const char * const k_step_verbs[] = {
    "change-screen", "anim", "label-set-text", "obj-set-y",
    "obj-add-state", "obj-clear-state", "obj-add-flag", "obj-clear-flag",
    "set", "delay", "call",
};

static int is_step_verb(const char * tag)
{
    for (size_t i = 0; i < sizeof(k_step_verbs) / sizeof(k_step_verbs[0]); i++) {
        if (strcmp(k_step_verbs[i], tag) == 0) return 1;
    }
    return 0;
}

static void set_plain_style(lv_obj_t * obj, const char * name, const char * value)
{
    lv_color_t c;
    if (strcmp(name, "color") == 0 && parse_color(value, &c)) {
        lv_obj_set_style_text_color(obj, c, 0);
    } else if (strcmp(name, "bg") == 0 && parse_color(value, &c)) {
        lv_obj_set_style_bg_color(obj, c, 0);
    } else if (strcmp(name, "bgOpa") == 0) {
        lv_obj_set_style_bg_opa(obj, (lv_opa_t)strtol(value, NULL, 10), 0);
    } else if (strcmp(name, "radius") == 0) {
        lv_obj_set_style_radius(obj, (int32_t)strtol(value, NULL, 10), 0);
    } else if (strcmp(name, "align") == 0) {
        lv_align_t a;
        if (parse_align(value, &a)) lv_obj_set_align(obj, a);
    }
}

static void apply_lv_attr(lv_obj_t * obj, const char * name, const char * value)
{
    /* name is the local part after "lv:" */
    const style_prop_t * sp = lookup_style_prop(name);
    if (!sp) return; /* unknown lv: prop — skip in phase 1 */

    switch (sp->kind) {
    case STYLE_INT: {
        int32_t v = (int32_t)strtol(value, NULL, 10);
        sp->int_fn(obj, v);
        break;
    }
    case STYLE_COLOR: {
        lv_color_t c;
        if (parse_color(value, &c)) sp->color_fn(obj, c);
        break;
    }
    case STYLE_SPECIAL:
        if (strcmp(name, "align") == 0) {
            lv_align_t a;
            if (parse_align(value, &a)) lv_obj_set_align(obj, a);
        } else if (strcmp(name, "x") == 0) {
            lv_obj_set_style_x(obj, (int32_t)strtol(value, NULL, 10), 0);
        } else if (strcmp(name, "y") == 0) {
            lv_obj_set_style_y(obj, (int32_t)strtol(value, NULL, 10), 0);
        } else if (strcmp(name, "width") == 0) {
            lv_obj_set_width(obj, parse_coord(value));
        } else if (strcmp(name, "height") == 0) {
            lv_obj_set_height(obj, parse_coord(value));
        } else if (strcmp(name, "text-font") == 0) {
            const lv_font_t * f = lookup_font(value);
            if (f) lv_obj_set_style_text_font(obj, f, 0);
        }
        break;
    }
}

static void XMLCALL on_start(void * userData, const XML_Char * name,
                             const XML_Char ** attrs)
{
    eezml_ctx_t * ctx = (eezml_ctx_t *)userData;

    /* structural elements: no object created */
    if (strcmp(name, "ui") == 0) return;
    if (strcmp(name, "project") == 0) return;
    if (strcmp(name, "screen") == 0) {
        ctx->in_screen = 1;
        ctx->screen_name[0] = '\0';
        for (int i = 0; attrs[i]; i += 2) {
            if (strcmp(attrs[i], "name") == 0) {
                strncpy(ctx->screen_name, attrs[i + 1], sizeof(ctx->screen_name) - 1);
                break;
            }
        }
        return;
    }
    /* phase 2: behavioral elements */
    if (strcmp(name, "var") == 0) {
        const char * vn = NULL, * vt = NULL, * vd = NULL;
        for (int i = 0; attrs[i]; i += 2) {
            if (strcmp(attrs[i], "name") == 0) vn = attrs[i + 1];
            else if (strcmp(attrs[i], "type") == 0) vt = attrs[i + 1];
            else if (strcmp(attrs[i], "default") == 0) vd = attrs[i + 1];
        }
        var_declare(vn, vt, vd);
        return;
    }
    if (strcmp(name, "action") == 0) {
        if (s_action_cnt < EEZML_MAX_ACTIONS) {
            ctx->cur_action = &s_actions[s_action_cnt];
            memset(ctx->cur_action, 0, sizeof(eezml_action_t));
            for (int i = 0; attrs[i]; i += 2) {
                if (strcmp(attrs[i], "name") == 0) {
                    strncpy(ctx->cur_action->name, attrs[i + 1], EEZML_NAME_LEN - 1);
                    break;
                }
            }
            ctx->action_open = 1;
        }
        return;
    }
    if (ctx->action_open && is_step_verb(name)) {
        eezml_action_t * a = ctx->cur_action;
        if (a->step_cnt >= EEZML_MAX_STEPS) return;
        eezml_step_t * s = &a->steps[a->step_cnt];
        memset(s, 0, sizeof(*s));
        strncpy(s->verb, name, sizeof(s->verb) - 1);
        for (int i = 0; attrs[i]; i += 2) {
            const char * an = attrs[i], * av = attrs[i + 1];
            if (strcmp(an, "target") == 0) strncpy(s->target, av, sizeof(s->target) - 1);
            else if (!strcmp(an, "prop") || !strcmp(an, "var") || !strcmp(an, "native") ||
                     !strcmp(an, "state") || !strcmp(an, "flag") || !strcmp(an, "screen"))
                strncpy(s->a, av, sizeof(s->a) - 1);
            else if (!strcmp(an, "value") || !strcmp(an, "text"))
                strncpy(s->b, av, sizeof(s->b) - 1);
            else if (strcmp(an, "from") == 0) s->n_from = (int32_t)atol(av);
            else if (strcmp(an, "to") == 0) s->n_to = (int32_t)atol(av);
            else if (strcmp(an, "y") == 0) s->n_from = (int32_t)atol(av);
            else if (strcmp(an, "time") == 0) s->n_time = (int32_t)atol(av);
            else if (strcmp(an, "delay") == 0) s->n_delay = (int32_t)atol(av);
            else if (strcmp(an, "repeat") == 0) s->n_repeat = (int32_t)atol(av);
            else if (strcmp(an, "playback") == 0 &&
                     (av[0] == 't' || av[0] == '1')) s->playback = 1;
        }
        a->step_cnt++;
        return;
    }
    /* still skipped: trigger/step wrappers, user-widget definitions */
    if (strcmp(name, "trigger") == 0 || strcmp(name, "step") == 0) return;
    if (strcmp(name, "widget") == 0) return;

    const eezml_widget_type_t * wt = lookup_widget(name);
    if (!wt) return; /* unknown tag — skip (children still processed) */

    lv_obj_t * parent = ctx->depth > 0 ? ctx->parents[ctx->depth - 1] : NULL;
    if (!parent) return;

    lv_obj_t * obj = wt->create(parent);
    if (!obj) return;
    if (!ctx->root) ctx->root = obj;

    /* push parent stack */
    if (ctx->depth >= MAX_PARENTS) return;
    ctx->parents[ctx->depth++] = obj;

    for (int i = 0; attrs[i]; i += 2) {
        const char * an = attrs[i];
        const char * av = attrs[i + 1];

        if (strncmp(an, "lv:", 3) == 0) {
            apply_lv_attr(obj, an + 3, av);
        } else if (strcmp(an, "id") == 0) {
            if (s_id_cnt < EEZML_MAX_IDS) {
                strncpy(s_ids[s_id_cnt].id, av, EEZML_NAME_LEN - 1);
                s_ids[s_id_cnt].obj = obj;
                s_id_cnt++;
            }
        } else if (strcmp(an, "x") == 0) {
            lv_obj_set_x(obj, (int32_t)strtol(av, NULL, 10));
        } else if (strcmp(an, "y") == 0) {
            lv_obj_set_y(obj, (int32_t)strtol(av, NULL, 10));        } else if (strcmp(an, "w") == 0) {
            lv_obj_set_width(obj, parse_coord(av));
        } else if (strcmp(an, "h") == 0) {
            lv_obj_set_height(obj, parse_coord(av));
        } else if (strcmp(an, "text") == 0 && wt->is_labelish) {
            lv_obj_t * label = NULL;
            if (strcmp(wt->tag, "button") == 0) {
                /* 9.x buttons don't auto-create a label child anymore */
                label = lv_obj_get_child(obj, 0);
                if (!label) {
                    label = lv_label_create(obj);
                    lv_obj_center(label);
                }
                if (label) lv_label_set_text(label, av);
            } else if (strcmp(wt->tag, "checkbox") == 0) {
                lv_checkbox_set_text(obj, av);
            } else {
                lv_label_set_text(obj, av);
            }
        } else if (strcmp(an, "src") == 0 && wt->is_imageish) {
            const lv_image_dsc_t * dsc = lookup_bitmap(av);
            if (dsc) {
                if (strcmp(wt->tag, "gif") == 0) lv_gif_set_src(obj, dsc);
                else lv_image_set_src(obj, dsc);
            }
        } else if (strcmp(an, "font") == 0) {
            const lv_font_t * f = lookup_font(av);
            if (f) lv_obj_set_style_text_font(obj, f, 0);
        } else if (strncmp(an, "on-", 3) == 0) {
            for (size_t ei = 0; ei < sizeof(k_events) / sizeof(k_events[0]); ei++) {
                if (strcmp(an + 3, k_events[ei].attr) == 0) {
                    eezml_evt_t * ev = NULL;
                    for (int si = 0; si < EEZML_MAX_EVTS; si++) {
                        if (!s_evts[si].used) { ev = &s_evts[si]; break; }
                    }
                    if (ev) {
                        ev->used = 1;
                        strncpy(ev->action, av, EEZML_NAME_LEN - 1);
                        lv_obj_add_event_cb(obj, event_cb, k_events[ei].code, ev);
                    }
                    break;
                }
            }
        } else if (strcmp(an, "bind") == 0) {
            wire_bind(obj, name, av);
        } else {
            set_plain_style(obj, an, av);
        }
    }

}

static void XMLCALL on_end(void * userData, const XML_Char * name)
{
    eezml_ctx_t * ctx = (eezml_ctx_t *)userData;
    if (ctx->action_open && strcmp(name, "action") == 0) {
        ctx->action_open = 0;
        if (ctx->cur_action && ctx->cur_action->name[0]) s_action_cnt++;
        ctx->cur_action = NULL;
        return;
    }
    if (ctx->action_open) return; /* inside an action: step elements self-close */
    if (strcmp(name, "screen") == 0) {
        ctx->in_screen = 0;
        return;
    }
    if (lookup_widget(name) && ctx->depth > 1) {
        ctx->depth--;
    }
}

/* ---- hot reload support ------------------------------------------ */

#define EEZML_MAX_TOPLEVEL 16

static lv_obj_t * s_top_objs[EEZML_MAX_TOPLEVEL];   /* objects we created */
static int        s_top_cnt;
static lv_obj_t * s_last_parent;                    /* reload target */


static void snapshot_vars(void)
{
    s_snap_cnt = 0;
    for (int i = 0; i < s_var_cnt && s_snap_cnt < EEZML_MAX_SNAP; i++) {
        eezml_snap_t * sn = &s_snap[s_snap_cnt++];
        strncpy(sn->name, s_vars[i].name, EEZML_NAME_LEN - 1);
        sn->is_string = s_vars[i].is_string;
        if (s_vars[i].is_string) {
            strncpy(sn->s, (const char *)s_vars[i].subject.value.pointer, sizeof(sn->s) - 1);
        } else {
            sn->i = s_vars[i].subject.value.num;
        }
    }
}

void eezml_unload(void)
{
    snapshot_vars();
    /* deleting the objects also unbinds their observers */
    for (int i = 0; i < s_top_cnt; i++) {
        if (s_top_objs[i]) lv_obj_delete(s_top_objs[i]);
    }
    s_top_cnt = 0;
    memset(s_ids, 0, sizeof(s_ids));
    s_id_cnt = 0;
    memset(s_evts, 0, sizeof(s_evts));
    for (int i = 0; i < s_var_cnt; i++) lv_subject_delete(&s_vars[i].subject);
    s_var_cnt = 0;
    memset(s_actions, 0, sizeof(s_actions));
    s_action_cnt = 0;
    /* s_natives survives: C-side assets outlive documents */
}

/* syntax-only dry parse: reports whether the document is well-formed XML
 * without touching any state (handlers NULL) */
static int xml_is_wellformed(const char * xml)
{
    XML_Parser p = XML_ParserCreate(NULL);
    if (!p) return 0;
    int ok = XML_Parse(p, xml, (int)strlen(xml), 1) == XML_STATUS_OK;
    XML_ParserFree(p);
    return ok;
}

lv_obj_t * eezml_reload(const char * xml)
{
    if (!s_last_parent || !xml) return NULL;
    /* keep the old UI alive unless the new document parses cleanly */
    if (!xml_is_wellformed(xml)) return NULL;
    eezml_unload();
    return eezml_create(s_last_parent, xml);
}

/* ------------------------------------------------------------------ */
/* public entry                                                        */
/* ------------------------------------------------------------------ */

lv_obj_t * eezml_create(lv_obj_t * parent, const char * xml)
{
    if (!parent || !xml) return NULL;

    eezml_ctx_t ctx;
    memset(&ctx, 0, sizeof(ctx));

    /* reset id table per document (names are document-scoped) */
    memset(s_ids, 0, sizeof(s_ids));
    s_id_cnt = 0;

    /* hot reload bookkeeping */
    s_last_parent = parent;
    int base_child_cnt = lv_obj_get_child_count(parent);

    XML_Parser p = XML_ParserCreate(NULL);
    if (!p) return NULL;
    XML_SetUserData(p, &ctx);
    XML_SetElementHandler(p, on_start, on_end);

    /* the document root <ui> is the natural parent; widgets without an
     * enclosing <screen> attach directly to `parent` via depth==0 fallback,
     * so seed the stack with it. */
    ctx.parents[ctx.depth++] = parent;

    lv_obj_t * result = NULL;
    if (XML_Parse(p, xml, (int)strlen(xml), 1) == XML_STATUS_OK) {
        result = ctx.root ? ctx.root : parent;
        /* register every toplevel object we created under parent */
        int now = lv_obj_get_child_count(parent);
        for (int i = base_child_cnt; i < now && s_top_cnt < EEZML_MAX_TOPLEVEL; i++) {
            s_top_objs[s_top_cnt++] = lv_obj_get_child(parent, i);
        }
    }
    XML_ParserFree(p);
    /* snapshot consumed */
    s_snap_cnt = 0;
    return result;
}
