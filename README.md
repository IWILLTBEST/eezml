# eezml

[中文版](README.zh-CN.md)

A complete toolchain for **EEZ Studio → XML → runtime-built UI on firmware** (own format, benchmarked against LVGL Pro's XML dynamic loading).

```
EEZ Studio project ──eezml_export.py──> eezml (uixml XML)
                                               │
                                         EMBED / file system
                                               ▼
                                 firmware/eezml_rt runtime kernel
                                 (expat SAX + object-tree build, portable C)
                                               │
                            ┌──────────────────┴──────────────────┐
                            ▼                                     ▼
                     boot: eezml_create()              hot reload: push XML over
                                                              UART/USB, rebuild
                                                              the live object tree
```

## Screenshots

All screens below were generated from the toolchain and captured through the
[MCP screenshot tool](https://github.com/IWILLTBEST/eez-studio-mcp) — no manual touch.

**Motor controller** (`examples/motor-en`, 3 screens; Chinese variant in `examples/motor`):

| overview | params | alarms |
|---|---|---|
| ![overview](docs/img/motor-en-overview.png) | ![params](docs/img/motor-en-params.png) | ![alarms](docs/img/motor-en-alarms.png) |

**Glassmorphism showcase** (`examples/glass` — translucent cards, shadows, gradient bg, staggered entrance animation):

![glass](docs/img/glass-dashboard.png)

**i18n, one source two languages** (`examples/i18n` — switch `strings.default` and recompile):

| English | 中文 |
|---|---|
| ![en](docs/img/i18n-en.png) | ![zh](docs/img/i18n-zh.png) |

**Rich data demo** (`examples/richdata` — roller, gauge, calendar, spinbox, keyboard, tabview):

| main | controls | settings |
|---|---|---|
| ![main](docs/img/richdata.png) | ![controls](docs/img/richdata-controls.png) | ![settings](docs/img/richdata-settings.png) |

## What's inside

| Path | Description |
|---|---|
| `eezml_export.py` | .eez-project → uixml XML + assets manifest exporter |
| `uixml.py` | uixml format core (lossless XML↔IR round-trip) |
| `ir2eez.py` / `eez2ir.py` | IR ↔ EEZ project conversion (Import back / project parsing) |
| `generator.py` | html2eez (HTML → EEZ project) |
| `migrate_uixml.py` | JSON IR → uixml migration tool |
| `SKILL.md` / `SKILL.zh-CN.md` | AI manual: step-by-step workflow to generate EEZ projects (EN / ZH, equivalent) |
| `firmware/eezml_rt/` | **portable runtime kernel** (pure C + lvgl + bundled expat, zero platform dependencies; non-ESP platforms just add the sources to their build) |
| `firmware/eezml_uart/` | ESP-IDF port: hot-reload transport (UART / USB-Serial/JTAG) + flash persistence; other platforms reimplement the same small header contract |
| `tools/` | push tool (below), A-WASM simulator build, golden CI, visual regression |
| `vscode/` | VS Code extension (dual-mode preview / Import / Run) |
| `examples/` | glass / i18n / motor / richdata / phase2-demo examples |
| `golden/` | golden regression data |

## Quick start

```bash
# export eezml from an EEZ Studio project
python eezml_export.py your.eez-project -o out_dir

# firmware side (ESP-IDF):
#   put eezml_rt + eezml_uart under components/, then in main:
#   eezml_register_bitmap("bulb", &img_bulb);
#   eezml_create(lv_screen_active(), xml_text);   // XML from EMBED or persisted
#   eezml_uart_start();                            // hot-reload transport
```

The phase-2 behavior engine (variables / bindings / actions) is shown in
`examples/phase2-demo.uixml`: declare `<var>` → `bind="count"` wires an
lv_observer → `on-clicked` events → declarative `<anim>` / `<call native>`
steps → the C side drives the UI with `eezml_get/set_var_int`.

## Hot reload + persistence

Push a document to a **running** device — no rebuild, no reflash, the live
object tree is torn down and rebuilt in about a second:

```bash
# close idf.py monitor first (the port is exclusive)
python tools/push.py --port COM5 doc.uixml             # volatile (this session only)
python tools/push.py --port COM5 doc.uixml --persist   # store in flash, survives reboot
python tools/push.py --port COM5 --wipe                # forget it, back to the compiled-in UI
```

- Wire protocol: `EEZML BEGIN <name> <size> [persist]` + raw XML + `EEZML END <crc32>`.
  Corrupt packets are rejected whole; a syntactically bad document fails a dry
  run first, so the old UI stays untouched.
- Variables are snapshotted across a reload — counters and bound state survive.
- With `--persist` the document is written to the `storage` flash partition
  (raw `esp_partition` access, no filesystem mounted); on boot the firmware
  loads the persisted document first and falls back to the compiled-in (EMBED)
  one. `--wipe` returns to factory.
- Portability: only `firmware/eezml_uart/` knows about ESP hardware. The
  kernel (`eezml_rt`), the protocol and the record format are platform-neutral;
  porting to another MCU means rewriting the small `store/load/wipe` contract
  against that platform's flash API.

## Roadmap

- **Phase 1 (done)**: static UI (widgets + styles + assets) loaded at runtime; verified on hardware — dual-GIF screen equivalent to the C export
- **Phase 2 (done)**: declarative actions (12 step verbs) + `bind` data binding (lv_observer) + native C action registry + event bridge; verified on hardware
- **Phase 3a (done)**: hot reload over UART/USB — push XML to a running device, live rebuild with CRC + dry-run protection
- **Phase 3b (done)**: persistence — `--persist` survives reboot, EMBED fallback, `--wipe`
- **Next**: widget-def runtime reuse, standalone assets generator, multi-document slots, "export & push" button in EEZ Studio

## License

- Toolchain code in this repo: as declared in the repo
- `firmware/eezml_rt/expat/`: Expat MIT (copyright headers kept verbatim, from the LVGL 9.4 bundled copy)
