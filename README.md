# eezml

EEZ Studio → XML → 固件运行时构建 UI 的完整工具链（自有格式，对标 LVGL Pro 的 XML 动态加载能力）。

```
EEZ Studio 工程 ──eezml_export.py──> eezml (uixml XML)
                                          │
                                    EMBED / 文件系统
                                          ▼
                              firmware/eezml_rt 运行时内核
                              （expat SAX + 对象树构建，纯 C 可移植）
```

## 组成

| 目录/文件 | 说明 |
|---|---|
| `eezml_export.py` | .eez-project → uixml XML + assets 清单导出器 |
| `uixml.py` | uixml 格式核心（XML↔IR 双向无损） |
| `ir2eez.py` / `eez2ir.py` | IR ↔ EEZ 工程转换（Import 回流 / 工程解析） |
| `generator.py` | html2eez（HTML → EEZ 工程） |
| `migrate_uixml.py` | JSON IR → uixml 迁移工具 |
| `firmware/eezml_rt/` | **可移植运行时内核**（纯 C + lvgl + bundled expat，零平台依赖；ESP-IDF 之外的平台直接把源文件加入构建） |
| `vscode/` | VS Code 扩展（双模式预览 / Import / Run） |
| `tools/` | A-WASM 模拟器构建、金标准 CI、视觉回归 |
| `examples/` | glass / i18n / motor / richdata 示例 |
| `golden/` | 金标准回归数据 |

## 快速上手

```bash
# 导出 eezml
python eezml_export.py your.eez-project -o out_dir

# 固件侧（ESP-IDF）
#   components/ 下放 eezml_rt，main 里：
#   eezml_register_bitmap("bulb", &img_bulb);
#   eezml_create(lv_screen_active(), xml_text);
```

## 路线图

- **一期（已完成）**：静态 UI（部件+样式+assets）运行时加载，实机验收=双 GIF 画面与 C 导出等价
- **二期（进行中）**：声明式 action（anim/set/if/call）+ `bind` 数据绑定（lv_observer）+ 原生 C action 注册表
- **三期**：热重载（UART/USB 推 XML 秒级重建）、组件层运行时复用（widget-def）、assets 独立生成器

## 许可

- 本仓工具链代码：随本仓声明
- `firmware/eezml_rt/expat/`：Expat MIT（原样保留版权头，源自 LVGL 9.4 bundled 副本）
