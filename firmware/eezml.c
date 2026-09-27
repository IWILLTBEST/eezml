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

typedef struct {
    const char * name;
    const lv_image_dsc_t * dsc;
} eezml_bitmap_entry_t;

typedef struct {
    const char * name;
    const lv_font_t * font;
} eezml_font_entry_t;

typedef struct {
    const char * id;
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
} eezml_ctx_t;

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
    /* phase 1: behavioral elements are parsed past */
    if (strcmp(name, "var") == 0 || strcmp(name, "action") == 0 ||
        strcmp(name, "trigger") == 0 || strcmp(name, "step") == 0 ||
        strcmp(name, "anim") == 0) {
        return;
    }
    /* user-widget definition for later phases */
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
                s_ids[s_id_cnt].id = lv_malloc(strlen(av) + 1);
                if (s_ids[s_id_cnt].id) {
                    strcpy((char *)s_ids[s_id_cnt].id, av);
                    s_ids[s_id_cnt].obj = obj;
                    s_id_cnt++;
                }
            }
        } else if (strcmp(an, "x") == 0) {
            lv_obj_set_x(obj, (int32_t)strtol(av, NULL, 10));
        } else if (strcmp(an, "y") == 0) {
            lv_obj_set_y(obj, (int32_t)strtol(av, NULL, 10));        } else if (strcmp(an, "w") == 0) {
            lv_obj_set_width(obj, parse_coord(av));
        } else if (strcmp(an, "h") == 0) {
            lv_obj_set_height(obj, parse_coord(av));
        } else if (strcmp(an, "text") == 0 && wt->is_labelish) {
            lv_obj_t * label = obj;
            if (wt->tag[0] == 'b') { /* button: label is child */
                label = lv_obj_get_child(obj, 0);
            }
            if (label) lv_label_set_text(label, av);
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
            /* phase 2: actions */
        } else if (strcmp(an, "bind") == 0) {
            /* phase 2: subjects */
        } else {
            set_plain_style(obj, an, av);
        }
    }

}

static void XMLCALL on_end(void * userData, const XML_Char * name)
{
    eezml_ctx_t * ctx = (eezml_ctx_t *)userData;
    if (strcmp(name, "screen") == 0) {
        ctx->in_screen = 0;
        return;
    }
    if (lookup_widget(name) && ctx->depth > 1) {
        ctx->depth--;
    }
}

/* ------------------------------------------------------------------ */
/* public entry                                                        */
/* ------------------------------------------------------------------ */

lv_obj_t * eezml_create(lv_obj_t * parent, const char * xml)
{
    if (!parent || !xml) return NULL;

    eezml_ctx_t ctx;
    memset(&ctx, 0, sizeof(ctx));

    /* reset id table per document (names are document-scoped in phase 1) */
    for (int i = 0; i < s_id_cnt; i++) {
        lv_free((void *)s_ids[i].id);
    }
    s_id_cnt = 0;

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
    }
    XML_ParserFree(p);
    return result;
}
