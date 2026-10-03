# figma2eez — Figma → EEZ Studio LVGL 转换器

把 Figma 设计稿（本地 `.fig` 文件）转换为可编辑、可构建的 EEZ Studio LVGL 工程——静态布局 + **Smart Animate 交互（弹簧动画/颜色渐变）** 自动生成。

## 用法

```bash
cd tools/figma2eez
npm install            # @open-pencil/fig（.fig 解析，fig-kiwi v106）

node figma2eez.cjs <输入.fig> [选项]

  -o <输出.eez-project>      输出路径（默认同名 .eez-project）
  --frame <名>               选择顶层 FRAME（默认第一个普通 frame）
  --variant <名>             组件集的默认变体（默认自动选择含交互起点的）
  --fonts-from <模板工程>    从已有工程复制字体对象（CJK/符号字体）
```

产物是标准 `.eez-project`：MCP 桥可 `open_project`/`check`/`build_project`，C 导出即固件可用；Studio 手工打开可继续编辑。

## 能力（v1）

| Figma | EEZ/LVGL |
|---|---|
| FRAME / 组件 / 实例（symbolData 解析，名字兜底） | panel 层级，auto-layout 拍平为绝对坐标 |
| 矩形/圆角矩形/椭圆 | panel：填充/圆角（椭圆=半径 50%），容器 pad=0、圆角容器 clip_corner |
| 文本 | label：字号/颜色/居中；拉丁 → Montserrat 内置档位；CJK → 需字体（见下） |
| lucide 命名矢量图标 | Material Symbols 符号字体 label（**颜色可渐变**；msr-codepoints.txt 码位表） |
| **prototypeInteractions: ON_CLICK → SWAP_STATE + SMART_ANIMATE** | **通用变体 diff 引擎**：同名节点 x 差 → `animSpring`（Figma 弹簧参数 `[mass,k,c,v0]` 原样传递）；填充色差 → `animTextColor SPRING`（逐通道+同弹簧曲线） |
| AFTER_TIMEOUT | SCREEN_LOADED 自动链（规划中，当前按点击处理） |
| 多变体互切 | 每个 x 动画节点配 `<name>_x` 变量 + Delay+SetVariable 跟踪——任意变体互切都从当前位置滑 |

交互源 → 命中区：key 直配 + 几何回退（源在任意变体内求绝对位置就近匹配部件），FRAME 型挂 CLICKED，文本/矢量由就近容器承载。

## 字体说明

- **拉丁文本**：自动映射 Montserrat 内置字号（MONTSERRAT_8..48），零配置。
- **中文/日韩**：lv_font_conv 需在 Studio 内跑（MCP 桥 `add_font`，`.ttc` 需先提为 `.ttf`），
  或用 `--fonts-from` 从已有工程复制字体对象（转换器自动把 `cjkNN` 引用重映射到模板里最接近字号的 CJK 字体）。
- **符号图标**：引用模板中的 `msym*` 字体（Material Symbols 静态实例经 `fontTools instancer` 生成——
  可变字体直接转换会渲染空白）。

## 验证过的完整链（金标准）

`Untitled.fig`（tabbar 弹簧滑块 demo，4 变体 15 交互）→ `AutoDemo.eez-project`：
check/build 零错误；Run 模式点击 tab → 滑块弹簧滑动 + 图标文字同步灰度渐变（992ms 同弹簧曲线）。

## 已知限制（v1）

- 复杂矢量/布尔运算 → 灰色占位 panel（后续可接 PNG 导出）
- 渐变填充/描边细节/混合模式 → 降级为纯色或不应用
- 多 frame NAVIGATE 转场 → 未映射（单 frame + 组件变体交互已覆盖）
- 图片填充（位图）→ 未导入（.fig blobs 提取待做）
