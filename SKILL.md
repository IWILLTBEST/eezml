---
name: ir2eez
description: Generate EEZ Studio LVGL .eez-project files from a declarative IR JSON. Use when the user asks to create embedded GUI screens/pages/HMI for EEZ Studio or LVGL, design device UI (dashboards, navigation bars, settings pages), bind variables to widgets, generate EEZ Flow actions (button click → change screen / set variable / selection highlight / partial view switching), or convert an HTML mockup into an EEZ project. Also covers the MCP server for remote AI control of EEZ Studio.
---

# ir2eez — AI-generated EEZ Studio LVGL projects

> 中文版（内容等价）：[SKILL.zh-CN.md](SKILL.zh-CN.md)

Turn UI requirements into IR JSON, compile to a `.eez-project`, and the user opens it in EEZ Studio for visual editing.

## Access paths

| Path | For | Notes |
|------|-----|-------|
| **MCP Server** (main route) | Claude Desktop / Cursor / ZCode / any MCP client | [eez-studio-mcp repo](https://github.com/IWILLTBEST/eez-studio-mcp): 47 tools + subscribable resources + progress notifications; install & config in its README |
| **DeepSeek Harness** | dsh Web UI / Studio toolbar AI button | Same MCP/HTTP bridge |

The built-in LLM agent panel has been removed (dual-track maintenance cost; Studio only ships the bridge).

Both routes share one HTTP bridge (`127.0.0.1:17620`, provided by the eez-studio-mcp extension or built into the [IWILLTBEST/studio](https://github.com/IWILLTBEST/studio) fork).

## Toolchain

- Tool directory: this repo's root (where `ir2eez.py` lives)
- Compiler: `ir2eez.py` (built-in IR validation + artifact self-check + glyph-coverage check; **non-zero exit code = failure, nothing written to disk**)
- Format docs: `IR_SCHEMA.md` (**must-read before writing IR**)
- MCP server: in the [eez-studio-mcp repo](https://github.com/IWILLTBEST/eez-studio-mcp) (`eez_mcp_server.py` / `mcp-server.mjs`, install per its README)
- Font tool: `font_tool.py` (compile / scan-html / list / show)
- Python: `python`
- EEZ Studio source: https://github.com/IWILLTBEST/studio (fork + built-in ai-agent bridge, GPL-3.0; the extension API is on upstream master, not yet in an official release)
- Examples: `motor.uixml` (motor controller, 3 screens with per-screen navigation) / `richdata.uixml` + `make_richdata.py`

## Workflow

1. **Read the format docs**: read `IR_SCHEMA.md` + one example
2. **Write the IR** (or a generator script) → 3. **compile** (check the exit code) → 4. **fix reported errors** → 5. **self-review the preview** → 6. **deliver**

```
D:/.../python.exe ir2eez.py <input.uixml> -o <output.eez-project>; echo exit=$?   # legacy .ir.json still accepted
```

## Hard rules

| Rule | Why |
|------|-----|
| Coordinates are always **relative to the parent container**; user-widget children are relative to the widget itself | LVGL semantics |
| **Cards / metric tiles / setting groups / gauge areas → each must be wrapped in a panel** (bg #1C2333 + radius 8) | Unwrapped regions look visually scattered |
| **Dynamic-text labels must carry a semantic `id`**; the compiler auto-prefixes types (label_/button_/panel_…); flow targets use the short id and map automatically | Firmware locates objects by identifier |
| **Identifiers are unique across the whole project** on normal pages; purely decorative components get no id | EEZ checks globally |
| **Identifiers must be all lowercase** | EEZ build stores UnderscoreLowerCase; uppercase fails at indexOf |
| **Identifier scope**: user-widget components are visible only to that page's flow; top-level actions can reference ids on normal pages only | Violations fail with not found |
| **Components that need cross-screen state sync (nav-bar highlighting etc.) must NOT be user widgets** — use a standalone panel per screen with the indicator fixed in the right spot | user-widget instances hold per-screen state; editing one doesn't sync to other instances |
| User-widget definitions use explicit absolute x/y, no flex; no ScreenWidget/Panel root | The compiler already handles it |
| Nest with `container`/`panel` (compiler zeroes padding/border) | The LVGL theme gives lv_obj 16-24px default padding |
| LED binds only `brightness` (0-255); shadow_width:0 is added automatically | EEZ limitation |
| dropdown needs an explicit `h` (26/28); options in English; font icon lists always carry 0xF077-0xF078 | The expanded list renders in montserrat — CJK becomes boxes; missing arrow glyphs |
| Corner radius: buttons 6; cards 8; small items 4 | Match the mockup |
| Dividers use `{"type":"line"}` | ppa32 style |
| Fonts match the mockup (msyh.ttc split into ttf); bpp=8; **the charset must cover ALL text in the final IR** (not only the mockup HTML scan) | Hand-written copy isn't in the HTML scan set → tofu boxes |
| label height/width auto-backstopped by the compiler (line-height × lines / longest-line estimate) | Prevents clipping / unwanted wrapping |
| **Manual coordinate alignment — centering formula**: elements centered inside a panel use `x = (parent_width - w) / 2`; elements stacked on arcs/graphics use `x = reference_center - w/2` (e.g. a 90-wide title on a 306-wide panel → x=108; a 140-wide arc centered at 153 → 80-wide value x=113) | flex is unreliable and unintuitive; always use explicit coordinates |
| **Text centering must use `align: "center"` (an IR field) → compiled to `text_align`**; never write localStyles `align` directly — that is **object alignment** (LV_ALIGN_CENTER moves the object to the parent's center and turns left/top into center offsets; the loaded layout shifts wholesale) | text_align centers text lines only, objects stay put |
| Numeric labels bound to variables: fixed-width box + `align: "center"`, so runtime digit-width changes stay centered (default left alignment drifts) | Rendered digit width ≠ the estimate |
| Modifications go through the IR and a recompile; EEZ-side manual edits get overwritten | One-way generation |

## Absorbed field experience (TrailCurrent eezstudio skill)

Source: [trailcurrentoss/TrailCurrentClaudeSkills · eezstudio/SKILL.md](https://github.com/trailcurrentoss/TrailCurrentClaudeSkills/blob/main/eezstudio/SKILL.md) (MIT; Trap numbers are its original numbering). That skill takes the "one-off script edits the .eez-project offline + human eyeball on the EEZ canvas" route; the traps below have been absorbed into this toolchain — most are mechanically solved by the compiler/MCP loop, the rest are must-checks when hand-editing JSON / using patch_project_json.

1. **Switches/checkboxes must carry `CHECKABLE`** (Trap 21): without it there's a press ripple but the state never toggles and VALUE_CHANGED never fires. EEZ's old default flags (`oldDefaultFlags`: CLICKABLE|PRESS_LOCK...) and hand-written generators are the likeliest to miss it; current EEZ new-object defaults include it. Signature: a C-side `lv_switch_create` switch toggles fine while an EEZ/IR-built one doesn't = hit. The `ir2eez.py` switch/checkbox builders write it explicitly (checkbox once missed, fixed 2026-08).
2. **Self-laying-out runtime widgets need style pinning** (Trap 13): `lv_keyboard / lv_list / lv_buttonmatrix / lv_dropdown / lv_roller / lv_tabview` internal layouts ignore left/top/width/height from JSON (the keyboard anchors bottom-center at half-screen height), **and the canvas reproduces it** (canvas emulates LVGL behavior). The right fix is NOT `lv_obj_set_pos` on the C side (the canvas doesn't run C) but localStyles MAIN.DEFAULT with `align: TOP_LEFT` + `min_width/max_width/min_height/max_height` pinning the authorized size — effective on canvas and device alike. IR's dropdown already forces an explicit h; when IR grows keyboard/list/tabview they must follow the pinning pattern.
3. **Decorative children swallow clicks** (Trap 15/16): LVGL hit-testing stops at the topmost clickable descendant and events don't bubble — a button's label, or the icon/text inside a clickable row, intercepts the click (symptom: "the button only reacts at a corner"). Rule: pure-display children get `clickableFlag: false`; the row's click target goes on the **last child** of the row container (highest z-order) with `bg_opa: 0` (transparent background; don't tint it — a theme change would betray it).
4. **Monospace numeric boxes: ceil the width + longMode CLIP** (Trap 20): adv_w/16 carries sub-pixels (e.g. 10.8125px/char); a box 0.25px short WRAPs to a second line and gets clipped (shows "13." and drops "4", looking like a font bug). Rule: `width = ceil(len × char_width) + 1`, numeric labels use `longMode: "CLIP"`. ir2eez's width backstop already adds +16 padding, covering most cases; fixed-width numeric boxes can add explicit CLIP as double insurance.
5. **Centralize colors, no scattered raw hex** (its "non-negotiable rule"): scattered hex everywhere is what betrays you on reskin/retheme. Its discipline: new colors must first be proposed as named tokens (light/dark values each + theme-invariant or not). IR equivalent: colors live centrally in the themes section; MCP `set_theme_color` changes one place and takes effect globally; same rule when hand-editing JSON — no new raw hex in localStyles.
6. **The canvas must honestly represent the device** (Trap 14): for labels the firmware fills at runtime, leave IR text empty or "-" — never fake placeholders like "Network 1..8". What the canvas shows must match the device in its unfilled state; the canvas IS the device preview.
7. **Injected widget JSON must carry every default field explicitly (label longMode is the proven case)**: hand-written/AI-generated injected widgets bypass classInfo defaults — palette-created labels default `longMode: "WRAP"`, injected ones lack it → codegen emits `lv_label_set_long_mode(obj, LV_LABEL_LONG_undefined)` and **compilation fails** (canvas/check both stay silent; only a real build exposes it; proven by Denis on examples #5, 2026-09-09 — Chart/Table/List/Menu/TileView all hit). Rule: every injected label gets an explicit `longMode` (WRAP/CLIP/DOTS/SCROLL/SCROLL_CIRCULAR as needed); likewise widgetFlags, tile `direction` (bitmask semantics — free grid navigation needs "ALL", a single "RIGHT" locks swiping), msgbox button closeButton, and every other defaulted field. **check 0/0 ≠ compilable: the C output must be truly verified (after the build lands, grep undefined / read screens.c).**

## Interaction patterns

- **Selection highlight**: `states: {"CHECKED": {"bg","color"}}` + `objAddState/objClearState` actions
- **Partial view switching**: two panels, one `hidden: true`, `objAddFlag/objClearFlag` (HIDDEN) mutual exclusion
- **Scroll area**: viewport panel + `scrollable: true` (auto SCROLLABLE + CLICKABLE); overflowing children scroll
- **Screen-change animation**: changeScreen's `fade` (MOVE_LEFT/FADE_IN etc.) + `speed`
- **Object movement**: `objSetY` action (e.g. moving an indicator bar) — but it doesn't cross-sync inside a user widget
- **Component position**: `objSetX/objSetY` actions (target + x/y params)
- **Glassmorphism**: `bgOpa:130-170` + `radius:16` + `lv:{border_width:1,border_opa:40,shadow_width:26,shadow_spread:3,shadow_opa:130}`; background gradient `lv:{bg_grad_color,bg_grad_dir:VER}`; copy examples/glass
- **Property animation**: `{"op":"lvgl","action":"anim","target":id,"prop":"x|y|w|h|opacity|img_zoom|img_angle","from":..,"to":..,"time":ms,"ease":"ease_out","repeat":N}` — repeat 0 = once / N = N times / -1 = infinite loop (breathing lamp: opacity + repeat:-1); compiles to animX and 6 sibling actions + `lv_anim_set_repeat_count`. repeat replays only, no yoyo; the wasm simulator ignores repeat until rebuilt (plays once), the firmware export honors it fully. Entrance = anim ease_out, exit = animX slide-out, emphasis = animOpacity pulse
- **Native actions (firmware interface)**: declare `{"name": "on_xxx"}` in IR without steps → compiles to `implementationType: "native"`; sliders/arcs/switches/dropdowns bind `value_changed`, buttons bind `clicked`. The compile also emits `action.h` (same directory as the output project): value-change style `void on_xxx(int32_t value)` (slider/arc = current value, switch = 0/1, dropdown = option index), click style `void xxx(void)`. The firmware includes it and implements the callbacks to finish the port

## MCP server setup

Server and bridge-extension install steps, config samples and the full 47-tool capability list: see the **[eez-studio-mcp](https://github.com/IWILLTBEST/eez-studio-mcp)** repo README (the single home of the MCP line). Quick start (Claude Desktop):

```json
// %APPDATA%/Claude/claude_desktop_config.json
{
  "mcpServers": {
    "eez-studio": {
      "command": "python",
      "args": ["<path to the eez-studio-mcp repo>/eez_mcp_server.py"]
    }
  }
}
```

## Font pipeline

```
# Scan glyphs (HTML + IR JSON + generator script merged BEFORE compiling!)
cat mockup.html xxx.ir.json make.py > _symsrc.html
font_tool.py compile --src fonts/msyh.ttf --name <name>_<size> --size <size> --bpp 8 \
  --range 32-127 --symbols-from-html _symsrc.html \
  --icon-font "font/fontawesome-free-6.7.2-web/webfonts/fa-solid-900.ttf:<codepoints>" \
  --icon-font "font/fontawesome-free-6.7.2-web/webfonts/fa-brands-400.ttf:0xF293-0xF294"
```

## Working with the user

- **Do not recompile while the project is open in EEZ Studio** (EEZ caches the old version; one save overwrites)
- Manual changes to keep → describe them → fold them into the IR → recompile (IR is the single source)
- IR edit strategy: small changes via the built-in edit (surgical), full rewrites via `write_ir`
- On delivery, state the verification points (which effects must be checked on the device)

## Before screenshot comparison: variable defaults

- A bound label renders its **design-time canvas previewValue** (the compiler evaluates the variable default), not the runtime expression — a wrong `default` shows the variable name or 0 in the screenshot and visual comparison false-positives
- Before screenshotting, check that the IR `variables`' `default` matches the expected display (aliases `value` / `init` also accepted, but don't mix)

## Visual regression (golden-baseline compare)

- Once the layout is final, `visual_baseline` (or `python tools/visreg.py baseline --name X --project P --screen S`) stores the golden at `golden/<name>.png`
- Before every IR-change delivery run `visual_check`: compile → check 0/0 → visual_check must hit the golden; over tolerance = fail, returning changedPixels/changedPct/bbox, with `golden/<name>.diff.png` marking changed pixels in red
- Tolerances: `delta` (per-channel diff, default 12, anti-aliasing immunity), `pct` (changed-pixel percentage, default 0.1)
- Goldens are bound to the Studio version / font rendering: re-baseline after upgrading Studio, changing fonts or themes
- Needs python + PIL/numpy; interpreter overridable via `EEZ_VISREG_PYTHON`

## Variable runtime semantics

- `native: true` variables: the firmware implements `get_var_xxx()/set_var_xxx()`
- Bindings poll every tick: the main loop calls `ui_tick()`; changing a variable refreshes all bound spots automatically
- Actions without steps = native stubs implemented by the firmware

## Reference implementation: motor controller (model new projects on this)

Mockup `motor_ui.html` → IR `motor.ir.json` → compiled `out_motor.eez-project` + `action.h`. 1024×600, three screens (overview/params/alarms) + StatusBar user widget.

**Three-layer interaction architecture** (the standard porting pattern):
- **Data downstream**: 13 global variables bound to widgets (metric-card labels / gauge-arc value / LED brightness / switch checkedState / clock text) — the firmware changes a variable, the UI refreshes every tick
- **Page navigation**: 3 flow actions (nav_overview/params/alarms → changeScreen) bound to nav-bar button clicks; each screen gets its own nav bar + indicator fixed in place (cross-screen highlight does NOT go through variables)
- **Input upstream**: 12 native actions (on_speed/on_torque/on_motor_temp/on_bus_volt/on_out_curr/on_fwd/on_eco/on_poles/on_ctrl_mode/on_can_baud/on_protocol + ack_alarm), 24 value_changed/clicked wirings; the firmware includes `action.h`, implements the callbacks, and the port is done

**Naming conventions**: variables = nouns (bus_volt); value-change actions = `on_<variable>` (fwd_on → on_fwd, dropping _on); click actions = verb phrases (ack_alarm); dynamic labels must carry a semantic id (label_val_speed).

**Layout pattern**: page = top StatusBar (user widget) + left slim nav bar (64px) + right content area (metric-card row / gauge row / control row, each wrapped in a panel #151B28+radius8); manual centering formula `x = reference_center - w/2`; numeric labels get fixed-width boxes + `align:"center"`.
