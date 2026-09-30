# eezml

[English](README.md)

EEZ Studio → XML → 固件运行时构建 UI 的完整工具链（自有格式，对标 LVGL Pro 的 XML 动态加载能力）。

```
EEZ Studio 工程 ──eezml_export.py──> eezml (uixml XML)
                                          │
                                    EMBED / 文件系统
                                          ▼
                              firmware/eezml_rt 运行时内核
                              （expat SAX + 对象树构建，纯 C 可移植）
                                          │
                     ┌────────────────────┴────────────────────┐
                     ▼                                         ▼
              启动：eezml_create()                   热重载：串口/USB 推 XML，
                                                     运行中重建对象树
```

## 截图

以下屏幕全部由本工具链生成、经 [MCP screenshot 工具](https://github.com/IWILLTBEST/eez-studio-mcp)拍摄——零手工。

**电机控制器**（`examples/motor-en` 三屏；中文版在 `examples/motor`）：

| 总览 | 参数 | 报警 |
|---|---|---|
| ![overview](docs/img/motor-en-overview.png) | ![params](docs/img/motor-en-params.png) | ![alarms](docs/img/motor-en-alarms.png) |

**玻璃拟态展示**（`examples/glass`——半透明卡片、阴影、渐变底、入场动画编排）：

![glass](docs/img/glass-dashboard.png)

**国际化，一份源两种语言**（`examples/i18n`——切换 `strings.default` 重编译即可）：

| English | 中文 |
|---|---|
| ![en](docs/img/i18n-en.png) | ![zh](docs/img/i18n-zh.png) |

**富数据演示**（`examples/richdata`——roller、仪表、日历、spinbox、键盘、tabview）：

| 主屏 | 控件 | 设置 |
|---|---|---|
| ![main](docs/img/richdata.png) | ![controls](docs/img/richdata-controls.png) | ![settings](docs/img/richdata-settings.png) |

## 组成

| 目录/文件 | 说明 |
|---|---|
| `eezml_export.py` | .eez-project → uixml XML + assets 清单导出器 |
| `uixml.py` | uixml 格式核心（XML↔IR 双向无损） |
| `ir2eez.py` / `eez2ir.py` | IR ↔ EEZ 工程转换（Import 回流 / 工程解析） |
| `generator.py` | html2eez（HTML → EEZ 工程） |
| `migrate_uixml.py` | JSON IR → uixml 迁移工具 |
| `SKILL.md` / `SKILL.zh-CN.md` | AI 手册：照分步工作流生成 EEZ 工程（英 / 中，内容等价） |
| `firmware/eezml_rt/` | **可移植运行时内核**（纯 C + lvgl + bundled expat，零平台依赖；ESP-IDF 之外的平台直接把源文件加入构建） |
| `firmware/eezml_uart/` | ESP-IDF 接入层：热重载传输（UART / USB-Serial/JTAG）+ flash 固化；其他平台按同样的头文件契约重写一小块即可 |
| `tools/` | 推送工具（见下）、A-WASM 模拟器构建、金标准 CI、视觉回归 |
| `vscode/` | VS Code 扩展（双模式预览 / Import / Run） |
| `examples/` | glass / i18n / motor / richdata / phase2-demo 示例 |
| `golden/` | 金标准回归数据 |

## 快速上手

```bash
# 导出 eezml
python eezml_export.py your.eez-project -o out_dir

# 固件侧（ESP-IDF）
#   components/ 下放 eezml_rt + eezml_uart，main 里：
#   eezml_register_bitmap("bulb", &img_bulb);
#   eezml_create(lv_screen_active(), xml_text);   // XML 来自 EMBED 或固化文档
#   eezml_uart_start();                            // 热重载传输
```

二期行为引擎（变量/绑定/动作）样例见 `examples/phase2-demo.uixml`：
`<var>` 声明变量 → `bind="count"` 挂 lv_observer 绑定 → `on-clicked` 事件 →
声明式 `<anim>`/`<call native>` 步骤 → C 侧 `eezml_get/set_var_int` 驱动 UI 自动刷新。

## 热重载 + 固化

往**运行中**的设备推文档——无重编、无烧写，运行中的对象树约一秒内卸载重建：

```bash
# 先关 idf.py monitor（串口独占）
python tools/push.py --port COM5 doc.uixml             # 易失（仅本次运行）
python tools/push.py --port COM5 doc.uixml --persist   # 写入 flash，重启后仍在
python tools/push.py --port COM5 --wipe                # 清除固化，回到出厂界面
```

- 线协议：`EEZML BEGIN <name> <size> [persist]` + 原始 XML + `EEZML END <crc32>`。
  坏包整包拒绝；语法坏的文档先过干跑验证，旧 UI 纹丝不动。
- 变量跨热重载快照——计数器和绑定状态不丢。
- `--persist` 把文档写入 `storage` flash 分区（`esp_partition` 裸读写，不挂文件系统）；
  启动时固化的文档优先、编译期 EMBED 的兜底。`--wipe` 恢复出厂。
- 可移植性：只有 `firmware/eezml_uart/` 认识 ESP 硬件。内核（`eezml_rt`）、
  协议和记录格式全部平台无关；换 MCU 只需按目标平台的 flash API 重写
  `store/load/wipe` 三函数的小契约。

## 路线图

- **一期（已完成）**：静态 UI（部件+样式+assets）运行时加载，实机验收=双 GIF 画面与 C 导出等价
- **二期（已完成）**：声明式 action（12 种步骤）+ `bind` 数据绑定（lv_observer）+ 原生 C action 注册表 + 事件桥，实机交互验收通过
- **三期 3a（已完成）**：热重载——UART/USB 推 XML 到运行中设备，CRC + 干跑双层保护
- **三期 3b（已完成）**：固化——`--persist` 重启存活，EMBED 兜底，`--wipe` 出厂
- **下一步**：组件层运行时复用（widget-def）、assets 独立生成器、多文档固化槽、EEZ Studio"导出即推"按钮

## 许可

- 本仓工具链代码：随本仓声明
- `firmware/eezml_rt/expat/`：Expat MIT（原样保留版权头，源自 LVGL 9.4 bundled 副本）
