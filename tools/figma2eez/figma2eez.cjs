#!/usr/bin/env node
/**
 * figma2eez — 通用 Figma .fig → EEZ Studio LVGL 工程转换器
 *
 * 用法:
 *   node figma2eez.cjs <input.fig> [-o out.eez-project] [--page <名>]
 *                      [--fonts-from <模板工程>] [--variant <变体名>]
 *
 * 引擎选择（自动）:
 *   v1 组件集引擎 — 页面含"children 全为 SYMBOL/COMPONENT 的 frame"（变体集）
 *     → 单 screen + SWAP_STATE 式 Smart Animate diff（x 差→animSpring、
 *       填充差→animTextColor SPRING、专用命中区、变量跟踪）。
 *   v2 多帧引擎 — 页面含多个同尺寸普通 frame，由 SMART_ANIMATE / PUSH_* 交互
 *     连接（如 Figma 官方 Smart Animate Examples 的 08 - Watch）
 *     → 按 SMART_ANIMATE 边做并查集分组：一组 = 一个 screen 的多个状态
 *       （5 帧轮盘 = 1 screen × 5 状态）；PUSH_* 边 = screen 间 changeScreen
 *       （PUSH_FROM_RIGHT→MOVE_LEFT 等）。能力：
 *     - INSTANCE 展开：symbolData.symbolOverrides 按 guid 应用（文本/填充/
 *       圆角/透明度），实例缩放比缩放内容坐标
 *     - 状态 diff：同名顶层项 x/y/w/h 差 → animX/Y/Width/Height（EASE_IN_OUT,
 *       start=跟踪变量，快速连点不跳变；变量链尾 SetVariable 收敛）
 *     - z 序 diff：目标状态 z 序逐项 objMoveForeground（LVGL z=创建序静态，
 *       轮盘 front 卡换位必须重排）
 *     - 嵌套填充差（分页点/箭头变色）→ 每状态一块 ghost panel +
 *       objSetFlagHidden 切换（免 animBgColor）
 *     - 交互触发: DRAG → 部件 CLICKED+GESTURE（清 GESTURE_BUBBLE 防双触发）
 *       + Compare 状态守卫（背卡条被点不误触）；PUSH → changeScreen
 *     - VECTOR 启发式: 近方形 Oval/Dot/Circle → 圆 panel（含描边→边框），
 *       带清晰实心填充；渐变/图片填充跳过
 *     - SCREEN_LOADED 复位链：多状态 screen 每次载入回到状态 1（变量+几何+
 *       ghost 同步），与 Figma "PUSH 回首帧" 语义一致
 *
 * 产物: 标准 .eez-project（MCP 桥可 open/check/build，C 导出即固件可用）
 */
const fs = require("fs");
const path = require("path");
const crypto = require("crypto");

////////////////////////////////////////////////////////////////////////////////

function parseArgs(argv) {
    const args = { _: [] };
    for (let i = 2; i < argv.length; i++) {
        const a = argv[i];
        if (a === "-o") args.out = argv[++i];
        else if (a === "--frame") args.frame = argv[++i];
        else if (a === "--page") args.page = argv[++i];
        else if (a === "--variant") args.variant = argv[++i];
        else if (a === "--fonts-from") args.fontsFrom = argv[++i];
        else if (a === "--icons") args.icons = argv[++i];
        else if (a === "--no-flow") args.noFlow = true;
        else if (a === "--flow") args.noFlow = false;
        else args._.push(a);
    }
    if (!args._[0]) {
        console.error("用法: node figma2eez.cjs <input.fig> [-o out] [--page 名] [--frame 名] [--fonts-from 模板] [--variant 名]");
        process.exit(1);
    }
    return args;
}

const G = g => (g ? `${g.sessionID}:${g.localID}` : null);
const typeStr = n => (n && n.type ? (typeof n.type === "string" ? n.type : Object.keys(n.type)[0] || "?") : "?");

// ---------- .fig 解析 + CRDT 合并 ----------
function parseFig(figPath) {
    let openPencil;
    try {
        openPencil = require(path.join(__dirname, "node_modules", "@open-pencil", "fig"));
    } catch (e) {
        console.error("缺依赖: npm install @open-pencil/fig");
        process.exit(1);
    }
    const r = openPencil.parseFigBuffer(fs.readFileSync(figPath));
    const nodes = new Map();
    const order = [];
    for (const c of r.nodeChanges) {
        const k = G(c.guid);
        if (!nodes.has(k)) { nodes.set(k, {}); order.push(k); }
        Object.assign(nodes.get(k), c);
    }
    return { raw: r, nodes, order };
}

function buildTree(ctx) {
    const kids = new Map();
    const roots = [];
    for (const k of ctx.order) {
        const n = ctx.nodes.get(k);
        const p = n.parentIndex && n.parentIndex.guid;
        const pk = p ? G(p) : null;
        if (pk && ctx.nodes.has(pk)) {
            if (!kids.has(pk)) kids.set(pk, []);
            kids.get(pk).push(k);
        } else {
            roots.push(k);
        }
    }
    // z 序 = parentIndex.position 字节序升序（bottom→top）
    for (const [, arr] of kids) {
        arr.sort((a, b) => {
            const pa = (ctx.nodes.get(a).parentIndex || {}).position || "";
            const pb = (ctx.nodes.get(b).parentIndex || {}).position || "";
            return pa < pb ? -1 : pa > pb ? 1 : 0;
        });
    }
    return { kids, roots };
}

// ---------- 几何 / 样式 ----------
function solidFill(n) {
    for (const f of n.fillPaints || []) {
        if (f && f.type === "SOLID" && f.color && f.visible !== false) {
            const c = f.color;
            const hex = v => Math.round(Math.max(0, Math.min(1, v)) * 255).toString(16).padStart(2, "0").toUpperCase();
            return "#" + hex(c.r) + hex(c.g) + hex(c.b) + (c.a !== undefined && c.a < 1 ? hex(c.a) : "");
        }
    }
    return null;
}
function fillOpa(n) {
    for (const f of n.fillPaints || []) {
        if (f && f.type === "SOLID" && f.color && f.visible !== false) {
            return f.opacity !== undefined ? Math.round(f.opacity * 255) : 255;
        }
    }
    return 255;
}
function strokeInfo(n) {
    for (const f of n.strokePaints || []) {
        if (f && f.type === "SOLID" && f.color && f.visible !== false && n.strokeWeight) {
            const c = f.color;
            const hex = v => Math.round(Math.max(0, Math.min(1, v)) * 255).toString(16).padStart(2, "0").toUpperCase();
            return { color: "#" + hex(c.r) + hex(c.g) + hex(c.b), width: Math.round(n.strokeWeight) };
        }
    }
    // 渐变描边 → 取首停色近似（LVGL border 无渐变；描边环/弧先可见再谈保真）
    for (const f of n.strokePaints || []) {
        if (f && (f.type || "").startsWith("GRADIENT") && f.stops && f.stops.length && f.visible !== false && n.strokeWeight) {
            const c = f.stops[0].color;
            const hex = v => Math.round(Math.max(0, Math.min(1, v)) * 255).toString(16).padStart(2, "0").toUpperCase();
            return { color: "#" + hex(c.r) + hex(c.g) + hex(c.b), width: Math.round(n.strokeWeight), gradApprox: true };
        }
    }
    return null;
}

// ---------- 位图（.fig 内嵌 PNG 原样提取，零猜测）----------
// PNG IHDR 宽高（签名8+len4+"IHDR"4=16 偏移，big-endian；DataView 兼容任意 typed array）
function pngSize(buf) {
    if (!buf || buf.length < 24 || buf[0] !== 0x89 || buf[1] !== 0x50) return null;
    const dv = new DataView(buf.buffer, buf.byteOffset, buf.byteLength);
    return { w: dv.getUint32(16), h: dv.getUint32(20) };
}
function imagePaint(n) {
    for (const f of n.fillPaints || []) {
        if (f && f.type === "IMAGE" && f.visible !== false && f.image && f.image.hash) return f;
    }
    return null;
}
const hashHex = h => Buffer.from(h).toString("hex");
// PNG 预缩放（box 平均=缩小抗锯齿；LVGL img zoom 是近邻采样，缩小必马赛克，
// 所以位图在转换期就重采样到节点精确尺寸，zoom 恒 256 走原生像素）
let PngJS = null;
function resamplePng(pngBuf, tw, th) {
    try {
        if (!PngJS) PngJS = require(path.join(__dirname, "node_modules", "pngjs"));
        const img = PngJS.PNG.sync.read(Buffer.from(pngBuf));
        const sw = img.width, sh = img.height;
        if (sw === tw && sh === th) return Buffer.from(pngBuf);
        const out = new PngJS.PNG({ width: tw, height: th });
        // RGBA box 平均：sx/sy 累积权重精确处理非整数倍
        const wAcc = new Float64Array(tw * th * 4);
        const x0s = [], x1s = [], y0s = [], y1s = [];
        for (let x = 0; x < tw; x++) { x0s[x] = x * sw / tw; x1s[x] = (x + 1) * sw / tw; }
        for (let y = 0; y < th; y++) { y0s[y] = y * sh / th; y1s[y] = (y + 1) * sh / th; }
        for (let ty = 0; ty < th; ty++) {
            for (let tx = 0; tx < tw; tx++) {
                let r = 0, g = 0, b = 0, a = 0, wsum = 0;
                const sy0 = y0s[ty], sy1 = y1s[ty], sx0 = x0s[tx], sx1 = x1s[tx];
                for (let sy = Math.floor(sy0); sy < Math.min(Math.ceil(sy1), sh); sy++) {
                    const wy = Math.min(sy + 1, sy1) - Math.max(sy, sy0);
                    if (wy <= 0) continue;
                    for (let sx = Math.floor(sx0); sx < Math.min(Math.ceil(sx1), sw); sx++) {
                        const wx = Math.min(sx + 1, sx1) - Math.max(sx, sx0);
                        if (wx <= 0) continue;
                        const wgt = wx * wy;
                        const si = (sy * sw + sx) * 4;
                        // 预乘 alpha 平均（透明边缘不发黑）
                        const al = img.data[si + 3] / 255;
                        r += img.data[si] * al * wgt; g += img.data[si + 1] * al * wgt;
                        b += img.data[si + 2] * al * wgt; a += img.data[si + 3] * wgt;
                        wsum += wgt;
                    }
                }
                if (wsum > 0) { r /= wsum; g /= wsum; b /= wsum; a /= wsum; }
                const di = (ty * tw + tx) * 4;
                const aN = a / 255;
                out.data[di] = Math.round(aN > 0 ? r / aN : 0);
                out.data[di + 1] = Math.round(aN > 0 ? g / aN : 0);
                out.data[di + 2] = Math.round(aN > 0 ? b / aN : 0);
                out.data[di + 3] = Math.round(a);
            }
        }
        return PngJS.PNG.sync.write(out);
    } catch (e) {
        console.warn(`  [warn] PNG 预缩放失败（${e.message}），保留原尺寸`);
        return Buffer.from(pngBuf);
    }
}
// 等比 cover 居中裁剪（对齐 Figma FILL 语义）再缩放到节点精确尺寸
function coverCropPng(pngBuf, tw, th) {
    try {
        if (!PngJS) PngJS = require(path.join(__dirname, "node_modules", "pngjs"));
        const img = PngJS.PNG.sync.read(Buffer.from(pngBuf));
        const sw = img.width, sh = img.height;
        const scale = Math.max(tw / sw, th / sh);
        const cw = Math.min(sw, Math.round(tw / scale)), ch = Math.min(sh, Math.round(th / scale));
        const cx = Math.floor((sw - cw) / 2), cy = Math.floor((sh - ch) / 2);
        const cropped = new PngJS.PNG({ width: cw, height: ch });
        for (let y = 0; y < ch; y++) for (let x = 0; x < cw; x++) {
            const si = ((cy + y) * sw + (cx + x)) * 4, di = (y * cw + x) * 4;
            for (let k = 0; k < 4; k++) cropped.data[di + k] = img.data[si + k];
        }
        return resamplePng(PngJS.PNG.sync.write(cropped), tw, th);
    } catch (e) {
        return Buffer.from(pngBuf);
    }
}
// 圆角 alpha 遮罩（烘进 PNG——lv_image 不认样式 radius，任何渲染器都正确）
function applyRadiusMask(png, w, h, r) {
    try {
        if (!PngJS) PngJS = require(path.join(__dirname, "node_modules", "pngjs"));
        const img = PngJS.PNG.sync.read(png);
        const rad = Math.min(r, Math.min(w, h) / 2);
        if (rad <= 0) return png;
        for (let y = 0; y < h; y++) {
            for (let x = 0; x < w; x++) {
                // 到四角的圆角距离
                const cx = x < rad ? rad : (x > w - 1 - rad ? w - 1 - rad : x);
                const cy = y < rad ? rad : (y > h - 1 - rad ? h - 1 - rad : y);
                const dx = x - cx, dy = y - cy;
                const d = Math.sqrt(dx * dx + dy * dy);
                const i = (y * w + x) * 4;
                if (d > rad) img.data[i + 3] = 0;
                else if (d > rad - 1) img.data[i + 3] = Math.round(img.data[i + 3] * (rad - d)); // 1px 抗锯齿边
            }
        }
        return PngJS.PNG.sync.write(img);
    } catch (e) { return png; }
}
// 注册表：hash+WxH → { name, w, h }；同图多档位（卡角 17..24/右屏 36/46）各一条
function makeBitmapRegistry(ctx) {
    const byKey = new Map(); // "hash_WxH" → rec（同图多档位各一条，档位=节点精确尺寸）
    const images = new Map(((ctx.raw || {}).images || []).map(e => [e[0], e[1]]));
    const reg = {
        bitmaps: [],
        register(paint, nodeW, nodeH, radius) {
            const hash = hashHex(paint.image.hash);
            const tw = Math.max(2, Math.round(nodeW || 0)), th = Math.max(2, Math.round(nodeH || 0));
            const key = `${hash}_${tw}x${th}`;
            if (byKey.has(key)) return byKey.get(key);
            const png = images.get(hash);
            if (!png) {
                console.warn(`  [warn] IMAGE ${hash.slice(0, 8)}… 不在 archive images 表，跳过`);
                byKey.set(key, null);
                return null;
            }
            const sz = pngSize(png) || { w: 0, h: 0 };
            const name = "img_" + hash.slice(0, 8) + "_" + tw + "x" + th;
            // 预缩放/预裁剪到节点尺寸（等比则直接重采样；不等比按 FILL cover 裁剪）。
            // zoom 恒 256：LVGL img zoom 近邻采样，缩小必马赛克。
            let data = png;
            let mode = "原尺寸";
            if (sz.w && sz.h) {
                const nearUniform = Math.abs(tw / sz.w - th / sz.h) <= Math.max(tw / sz.w, th / sz.h) * 0.2;
                if (nearUniform && sz.w !== tw) { data = resamplePng(png, tw, th); mode = `${sz.w}x${sz.h}→${tw}x${th}`; }
                else if (!nearUniform) { data = coverCropPng(png, tw, th); mode = `cover ${sz.w}x${sz.h}→${tw}x${th}`; }
            }
            if (radius > 0) data = applyRadiusMask(data, tw, th, radius); // Figma 圆角烘进 alpha
            reg.bitmaps.push({
                objID: oid(), name,
                image: "data:image/png;base64," + Buffer.from(data).toString("base64"),
                bpp: 32, alwaysBuild: false, lvglBinaryOutputFormat: 3, lvglDither: false
            });
            const rec = { name, w: tw, h: th };
            byKey.set(key, rec);
            console.log(`  [bitmap] ${name} ${mode} (${(data.length / 1024).toFixed(1)}KB)`);
            return rec;
        }
    };
    return reg;
}

// ---------- 图标符号（Material Symbols Rounded，码位查 msr-codepoints.txt）----------
// 发射时用 iconsym 字体（--fonts-from 模板的 MSR 字体克隆一条小字号条目，
// lvglSymbols 只收实际用到的字符；MONTSERRAT 内置字体不含 FontAwesome 区，
// 小字号直接用内置字体=方块）
const VECTOR_SYMBOL_NAMES = {
    left: "chevron_left", right: "chevron_right", up: "expand_less", down: "expand_more",
    back: "arrow_back",
    close: "close", check: "check", plus: "add", minus: "remove",
    play: "play_arrow", pause: "pause", stop: "stop", prev: "skip_previous", next: "skip_next",
    refresh: "refresh", settings: "settings", search: "search", home: "home",
    heart: "favorite", heartrate: "monitor_heart", trash: "delete", edit: "edit",
    eye: "visibility", warning: "warning", question: "help", info: "info",
    bluetooth: "bluetooth", wifi: "wifi", volume: "volume_up",
    bell: "notifications", gps: "my_location"
};
// VECTOR 名 → 符号名（方向按宽高比：瘦高=left/right，扁宽=up/down）
function vectorSymbol(n, w, h) {
    const nm = (n.name || "").toLowerCase();
    if (/up|north/.test(nm) && /arrow|chevron/.test(nm)) return VECTOR_SYMBOL_NAMES.up;
    if (/down|south/.test(nm) && /arrow|chevron/.test(nm)) return VECTOR_SYMBOL_NAMES.down;
    if (/arrow|chevron|back|previous/.test(nm)) return w <= h ? VECTOR_SYMBOL_NAMES.left : VECTOR_SYMBOL_NAMES.right;
    if (/play/.test(nm)) return VECTOR_SYMBOL_NAMES.play;
    if (/pause/.test(nm)) return VECTOR_SYMBOL_NAMES.pause;
    if (/heart/.test(nm)) return VECTOR_SYMBOL_NAMES.heart;
    if (/settings|gear/.test(nm)) return VECTOR_SYMBOL_NAMES.settings;
    if (/search|magnif/.test(nm)) return VECTOR_SYMBOL_NAMES.search;
    if (/close|cancel/.test(nm)) return VECTOR_SYMBOL_NAMES.close;
    if (/check/.test(nm)) return VECTOR_SYMBOL_NAMES.check;
    if (/bell|notif/.test(nm)) return VECTOR_SYMBOL_NAMES.bell;
    if (/bluetooth/.test(nm)) return VECTOR_SYMBOL_NAMES.bluetooth;
    if (/wifi/.test(nm)) return VECTOR_SYMBOL_NAMES.wifi;
    return null;
}
// 兄弟数值文本旁的无名小矢量（Icon + "00:12"）→ 活动度量符号（心率等）。
// 图标常包在 Glyph/Icon 包装 FRAME 里，数值文本在祖父级——向上查两级。
function siblingMetricSymbol(n, kids, nodes) {
    const w = (n.size || {}).x || 0, h = (n.size || {}).y || 0;
    if (w > 20 || h > 20) return null;
    const nm = (n.name || "").toLowerCase();
    if (!/icon|duration|time|activity|pulse|rate/.test(nm)) return null;
    const parentKeyOf = node => {
        const g = node && node.parentIndex && node.parentIndex.guid;
        return g ? G(g) : null;
    };
    let scope = parentKeyOf(n);
    for (let depth = 0; depth < 2 && scope; depth++) {
        const hasNumSibling = (kids.get(scope) || []).some(ck => {
            const cn = nodes.get(ck);
            if (cn === n) return false;
            const chars = ((cn && cn.textData) || {}).characters || "";
            return typeStr(cn) === "TEXT" && /\d/.test(chars);
        });
        if (hasNumSibling) return VECTOR_SYMBOL_NAMES.heart; // 实心心形贴近设计原版（monitor_heart 是描边盒样式）
        scope = parentKeyOf(nodes.get(scope) || {});
    }
    return null;
}
const hexToInt = c => c ? parseInt(c.slice(1, 7), 16) : 0x191919;
// GRADIENT_LINEAR → LVGL 双色渐变样式（bg_color→bg_grad_color，方向按 handle 主轴）
function gradientFill(n) {
    for (const f of n.fillPaints || []) {
        if (f && f.type === "GRADIENT_LINEAR" && f.stops && f.stops.length >= 2 && f.visible !== false) {
            const hex = v => Math.round(Math.max(0, Math.min(1, v)) * 255).toString(16).padStart(2, "0").toUpperCase();
            const st = f.stops.map(st => "#" + hex(st.color.r) + hex(st.color.g) + hex(st.color.b));
            let dir = "VER";
            const h = f.gradientHandlePositions;
            if (h && h.length >= 2) {
                const dx = Math.abs((h[0].x || 0) - (h[1].x || 0)), dy = Math.abs((h[0].y || 0) - (h[1].y || 0));
                dir = dx > dy ? "HOR" : "VER";
            }
            return { bg_color: st[0], bg_grad_color: st[st.length - 1], bg_grad_dir: dir };
        }
    }
    return null;
}
const sanitize = (name, prefix) => {
    let s = (name || prefix || "item").replace(/[^\w]/g, "_").toLowerCase().replace(/_+/g, "_").replace(/^_|_$/g, "");
    if (!s) s = prefix || "item";
    if (/^\d/.test(s)) s = "Screen" + s;
    return s;
};
// Figma 文本水平对齐（kiwi proto 默认值省略：无字段=LEFT）
function figTextAlign(n) {
    const td = n.textData || {};
    const src = (td.styleOverrideTable && td.styleOverrideTable.length && td.styleOverrideTable[0].textAlignHorizontal) ||
        td.textAlignHorizontal || (td.style && td.style.textAlignHorizontal);
    return { L: "LEFT", C: "CENTER", R: "RIGHT", J: "LEFT", LEFT: "LEFT", CENTER: "CENTER", RIGHT: "RIGHT", JUSTIFIED: "LEFT" }[src] || "LEFT";
}
// 按钮文本：同父 ROUNDED_RECTANGLE 与文本框重叠>70%（Figma 视觉居中的按钮文字，
// kiwi 无 align 字段可读，按结构识别——Start Run 106x32 叠 Rectangle 112x28）
function isButtonText(n, kids, nodes) {
    const g = n.parentIndex && n.parentIndex.guid;
    const pk = g ? G(g) : null;
    if (!pk) return false;
    const t = n.transform || {};
    const x = t.m02 || 0, y = t.m12 || 0, w = (n.size || {}).x || 0, h = (n.size || {}).y || 0;
    for (const ck of kids.get(pk) || []) {
        const cn = nodes.get(ck);
        if (cn === n || typeStr(cn) !== "ROUNDED_RECTANGLE") continue;
        const ct = cn.transform || {};
        const cx = ct.m02 || 0, cy = ct.m12 || 0, cw = (cn.size || {}).x || 0, ch = (cn.size || {}).y || 0;
        const ox = Math.max(0, Math.min(x + w, cx + cw) - Math.max(x, cx));
        const oy = Math.max(0, Math.min(y + h, cy + ch) - Math.max(y, cy));
        if (ox * oy > 0.7 * w * h) return true;
    }
    return false;
}

// ---------- EEZ 部件工厂（全默认字段，血泪教训全在内） ----------
let oidCounter = 0;
const oid = () => `${crypto.randomUUID().slice(0, 8)}-${(++oidCounter).toString(16).padStart(4, "0")}`;

function makeWidget(type, over) {
    return Object.assign({
        objID: oid(), type,
        left: 0, top: 0, width: 100, height: 100,
        leftUnit: "px", topUnit: "px", widthUnit: "px", heightUnit: "px",
        customInputs: [], customOutputs: [],
        style: { objID: oid(), useStyle: "default", conditionalStyles: [], childStyles: [] },
        timeline: [], eventHandlers: [], children: [],
        identifier: "", widgetFlags: "", states: "",
        localStyles: { objID: oid(), definition: {} }
    }, over);
}

// 装饰面板：清 CLICKABLE/SCROLLABLE（点击穿透到 screen；panel 默认全带）
const DECOR_FLAGS = "GESTURE_BUBBLE";
// 交互面板：CLICKABLE + 清 GESTURE_BUBBLE/SCROLLABLE（手势不冒泡到 screen）
const HIT_FLAGS = "CLICKABLE|CLICK_FOCUSABLE|PRESS_LOCK|SNAPPABLE";

function panelWidget(id, x, y, w, h, styleProps, flags) {
    const d = Object.assign({ pad_left: 0, pad_right: 0, pad_top: 0, pad_bottom: 0, border_width: 0 }, styleProps);
    if (d.radius > 0 && !d.clip_corner_keep) delete d.clip_corner_keep;
    if (d.radius > 0 && d.clip !== false) d.clip_corner = true;
    delete d.clip;
    return makeWidget("LVGLPanelWidget", {
        identifier: id, left: Math.round(x), top: Math.round(y),
        width: Math.round(w), height: Math.round(h),
        widgetFlags: flags !== undefined ? flags : DECOR_FLAGS,
        localStyles: { objID: oid(), definition: { MAIN: { DEFAULT: d } } }
    });
}

function labelWidget(id, x, y, w, h, text, styleProps, opts) {
    const fsz = (opts && opts.fontSize) || 16;
    const singleLine = !(text || "").includes("\n") && h <= fsz * 1.7;
    // 单行短文本 → 宽度自适应内容（渲染字体比 Inter 宽，定宽+WRAP 会截断换行）。
    // 不能用 longMode DOT/CLIP：LVGL 9.4 枚举改名 LV_LABEL_LONG_MODE_*，
    // Studio 生成的 LV_LABEL_LONG_<value> 在 9.4 编不过——content 宽不换行，无此依赖
    const units = singleLine ? { widthUnit: "content", heightUnit: "content" } : {};
    return makeWidget("LVGLLabelWidget", {
        identifier: id, left: Math.round(x), top: Math.round(y),
        width: Math.round(w), height: Math.round(h),
        text, textType: "literal", longMode: "WRAP", // 缺省即炸：C 导出会拼 LV_LABEL_LONG_<undefined>
        widgetFlags: DECOR_FLAGS,
        ...units,
        localStyles: { objID: oid(), definition: { MAIN: { DEFAULT: Object.assign({ text_align: "CENTER" }, styleProps || {}) } } }
    });
}

function imageWidget(id, x, y, w, h, bitmapName, zoom, radius) {
    // 圆角已烘进 PNG alpha（radius 参数仅用于注册位图时的遮罩），样式层不再设
    return makeWidget("LVGLImageWidget", {
        identifier: id, left: Math.round(x), top: Math.round(y),
        width: Math.round(Math.max(w, 4)), height: Math.round(Math.max(h, 4)),
        image: bitmapName, setPivot: false, pivotX: 0, pivotY: 0,
        zoom: Math.round(zoom || 256), angle: 0, innerAlign: "CENTER",
        widgetFlags: DECOR_FLAGS,
        localStyles: { objID: oid(), definition: {} }
    });
}
// Figma cornerRadius → 像素圆角（1000 等哨兵值=全圆；node 无值时矩形）
function imgRadius(n, w, h) {
    const cr = n.cornerRadius;
    if (!cr || cr <= 0) return 0;
    return Math.min(cr, Math.min(w, h) / 2);
}

////////////////////////////////////////////////////////////////////////////////

// lucide 名 → Material Symbols 名/码位
const LUCIDE_TO_MS = {
    "arrow-down-circle": "download", "bluetooth": "bluetooth", "cast": "cast",
    "copy": "content_copy", "download": "download", "wifi": "wifi",
    "settings": "settings", "home": "home", "search": "search", "user": "person",
    "heart": "favorite", "star": "star", "check": "check", "close": "close",
    "x": "close", "menu": "menu", "plus": "add", "minus": "remove",
    "chevron-right": "chevron_right", "chevron-left": "chevron_left",
    "chevron-up": "expand_less", "chevron-down": "expand_more",
    "play": "play_arrow", "pause": "pause", "trash": "delete",
    "bell": "notifications", "lock": "lock", "eye": "visibility",
    "sun": "light_mode", "moon": "dark_mode", "volume-2": "volume_up"
};

function loadCodepoints() {
    const p = path.join(__dirname, "msr-codepoints.txt");
    if (!fs.existsSync(p)) return {};
    const map = {};
    for (const line of fs.readFileSync(p, "utf8").split("\n")) {
        const [name, cp] = line.trim().split(/\s+/);
        if (name && cp) map[name] = parseInt(cp, 16);
    }
    return map;
}

// ---------- 字体选择 ----------
function makeTextFont(fontUse) {
    return function textFont(size, chars) {
        const hasCJK = /[\u4e00-\u9fff]/.test(chars || "");
        const sz = Math.max(8, Math.min(48, Math.round(size || 15)));
        if (hasCJK) {
            const key = "cjk" + sz;
            if (!fontUse.has(key)) fontUse.set(key, new Set());
            for (const ch of chars) fontUse.get(key).add(ch);
            return { font: key, cjk: true, size: sz };
        }
        const steps = [8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48];
        const ms = steps.reduce((a, b) => Math.abs(b - sz) < Math.abs(a - sz) ? b : a);
        return { font: "MONTSERRAT_" + ms, cjk: false, size: ms };
    };
}

function applyFontsFrom(proj, fontUse, args) {
    const cjkNeeds = [...fontUse.entries()].filter(([k]) => k.startsWith("cjk"));
    if (!args.fontsFrom) {
        if (cjkNeeds.length) {
            console.warn(`  [warn] 含中文文本 ${[...new Set([].concat(...cjkNeeds.map(([, s]) => [...s])))].join("")} —— 需要 CJK 字体：`);
            console.warn("        经 MCP 桥 add_font 生成（ttc 需先提 ttf），或 --fonts-from 模板工程复制，否则中文不显示");
        }
        const symChars = fontUse.get("iconsym");
        if (symChars && symChars.size) console.warn(`  [warn] 图标符号 ${[...symChars].join("")} 需要 --fonts-from 模板（MaterialSymbols 字体）才能渲染，否则符号为空`);
        for (const [k] of fontUse) if (k.startsWith("iconsym_")) console.warn(`  [warn] 图标符号档位 ${k} 需要 --fonts-from 模板（MaterialSymbols 字体），否则符号不显示`);
        return;
    }
    try {
        const tpl = JSON.parse(fs.readFileSync(args.fontsFrom, "utf8"));
        for (const f of tpl.fonts || []) proj.fonts.push(f);
        const cjkFonts = proj.fonts.filter(f => /msyh|cjk|han|noto/i.test(f.name));
        const remap = {};
        for (const [k] of fontUse) {
            if (k.startsWith("cjk")) {
                const size = parseInt(k.slice(3)) || 15;
                if (cjkFonts.length) {
                    const best = cjkFonts.reduce((a, b) => Math.abs((b.source?.size || 15) - size) < Math.abs((a.source?.size || 15) - size) ? b : a);
                    remap[k] = best.name;
                }
            }
        }
        // iconsym_<size>：按图标几何分档克隆 MSR 字体（每档一条，字号=档位，
        // lvglSymbols=该档用到的字符）。embeddedFontFile（内嵌 TTF base64）
        // 原样保留，构建时 lv_font_conv 重出字形。
        const symTiers = [...fontUse.entries()].filter(([k]) => k.startsWith("iconsym_"));
        if (symTiers.length) {
            const msr = (tpl.fonts || []).find(f => /msr|materialsymbols|material[-_]?symbols/i.test(f.name) ||
                (f.source && /MaterialSymbols/i.test(f.source.filePath || "")));
            if (msr && msr.embeddedFontFile) {
                for (const [tier, chars] of symTiers) {
                    const size = parseInt(tier.split("_")[1]) || 16;
                    const symbols = [...chars].join("");
                    const clone = JSON.parse(JSON.stringify(Object.assign({}, msr, {
                        name: tier,
                        source: Object.assign({}, msr.source, { size }),
                        size,
                        lvglRanges: "",
                        lvglSymbols: symbols,
                        lvglGlyphs: { encodings: [], symbols }
                    })));
                    clone.objID = oid();
                    if (clone.source) clone.source.objID = oid();
                    // 预览元数据按新字号重算（height=em 高度近似 size+4）
                    clone.height = size + 4;
                    if (msr.height > 0) {
                        const k = clone.height / msr.height;
                        clone.ascent = Math.round((msr.ascent || Math.round(msr.height * 0.78)) * k);
                        clone.descent = Math.max(0, clone.height - clone.ascent);
                    }
                    proj.fonts.push(clone);
                    console.log(`字体: ${tier} = MSR ${size}px 符号 ${symbols.length} 字符（${[...chars].map(c => "U+" + c.codePointAt(0).toString(16)).join(" ")}）`);
                }
            } else {
                console.warn("  [warn] 模板无 MaterialSymbols 字体（需 embeddedFontFile），图标符号将不渲染");
            }
        }
        const remapFont = w => {
            if (!w.localStyles) return;
            const d = w.localStyles.definition;
            if (!d.MAIN || !d.MAIN.DEFAULT) return;
            const t = d.MAIN.DEFAULT.text_font;
            if (t && remap[t]) d.MAIN.DEFAULT.text_font = remap[t];
        };
        for (const page of proj.userPages) {
            const walkRemap = w => { remapFont(w); (w.children || []).forEach(walkRemap); };
            (w => w && walkRemap(w))(page.components.find(c => c.type === "LVGLScreenWidget"));
        }
        console.log(`字体: 模板 ${proj.fonts.length} 个，映射 ${JSON.stringify(remap)}`);
    } catch (e) { console.warn("  [warn] --fonts-from 读取失败:", e.message); }
}

////////////////////////////////////////////////////////////////////////////////

// ================================================================
// v1 引擎：组件集变体（Untitled.fig tabbar demo 路径，保持原样）
// ================================================================
function runV1(args, pageKey, page, ctx, codepoints) {
    const { nodes, order, kids } = ctx;
    let frames = (kids.get(pageKey) || []).map(k => ({ key: k, node: nodes.get(k) })).filter(f => typeStr(f.node) === "FRAME");
    if (args.frame) frames = frames.filter(f => f.node.name === args.frame);
    if (!frames.length) { console.error("未找到 FRAME（--frame 指定名字？）"); process.exit(1); }

    const componentSets = [];
    const consumed = new Set();
    for (const f of frames) {
        const ch = kids.get(f.key) || [];
        if (ch.length >= 2 && ch.every(c => ["SYMBOL", "COMPONENT"].includes(typeStr(nodes.get(c))))) {
            componentSets.push({ key: f.key, node: f.node, variants: ch });
            consumed.add(f.key);
        }
    }
    const plainFrames = frames.filter(f => !consumed.has(f.key));
    const rootFrame = plainFrames[0] || { k: componentSets[0].key, node: componentSets[0].node };
    console.log(`目标 frame: "${rootFrame.node.name}" (${plainFrames.length} 普通 + ${componentSets.length} 组件集)`);

    const interactions = [];
    for (const k of order) {
        const n = nodes.get(k);
        for (const it of n.prototypeInteractions || []) {
            const ev = it.event || {};
            for (const ca of it.actions || []) {
                for (const a of (ca.actions && ca.actions.length ? ca.actions : [ca])) {
                    const targetKey = a.transitionNodeID ? G(a.transitionNodeID) : null;
                    const ef = a.easingFunction || [];
                    interactions.push({
                        srcKey: k, srcNode: n,
                        trigger: ev.interactionType,
                        timeout: ev.interactionDuration,
                        targetKey, targetNode: targetKey ? nodes.get(targetKey) : null,
                        type: a.transitionType, navigation: a.navigationType,
                        smart: !!a.transitionShouldSmartAnimate || a.transitionType === "SMART_ANIMATE",
                        duration: Math.round((a.transitionDuration || 0) * 1000),
                        k: ef[1] || 170, c: ef[2] || 15, v0: ef[3] || 0
                    });
                }
            }
        }
    }
    console.log(`交互: ${interactions.length} 条（SWAP_STATE+SMART_ANIMATE=${interactions.filter(i => i.navigation === "SWAP_STATE" && i.smart).length}）`);

    function variantContent(vKey) {
        const m = new Map();
        const walk = (k, ox, oy) => {
            for (const c of kids.get(k) || []) {
                const n = nodes.get(c);
                const t = n.transform || {};
                const box = { x: (t.m02 || 0) + ox, y: (t.m12 || 0) + oy, w: (n.size || {}).x || 0, h: (n.size || {}).y || 0 };
                const ty = typeStr(n);
                if (["TEXT", "RECTANGLE", "ROUNDED_RECTANGLE", "ELLIPSE", "VECTOR", "INSTANCE", "FRAME"].includes(ty)) {
                    m.set(n.name || ty + ":" + c, { node: n, key: c, type: ty, ...box, fill: solidFill(n), text: (n.textData || {}).characters });
                }
                walk(c, box.x, box.y);
            }
        };
        walk(vKey, 0, 0);
        return m;
    }
    const variants = [];
    for (const cs of componentSets) {
        for (const v of cs.variants) {
            variants.push({ key: v, node: nodes.get(v), content: variantContent(v), set: cs });
        }
    }

    let defaultVariant = null;
    if (args.variant) defaultVariant = variants.find(v => v.node.name === args.variant);
    if (!defaultVariant && variants.length) {
        defaultVariant = variants.find(v => [...v.content.values()].some(c => interactions.some(i => i.srcKey === c.key))) || variants[0];
    }

    const fillDiffNames = new Set();
    if (variants.length >= 2) {
        const names0 = new Set();
        for (const v of variants) for (const nm of v.content.keys()) names0.add(nm);
        for (const nm of names0) {
            const fills = variants.map(v => v.content.get(nm)).filter(Boolean).map(c => c.fill).filter(Boolean);
            if (new Set(fills).size > 1) fillDiffNames.add(nm);
        }
    }

    const fontUse = new Map();
    const textFont = makeTextFont(fontUse);
    const idUsed = new Set();
    const uniqId = base => { let i = 1, id = base; while (idUsed.has(id)) id = base + i++; idUsed.add(id); return id; };

    function emitNode(k, ox, oy, out, hitCollector) {
        const n = nodes.get(k);
        const ty = typeStr(n);
        const t = n.transform || {};
        const x = (t.m02 || 0) + ox, y = (t.m12 || 0) + oy;
        const w = (n.size || {}).x || 0, h = (n.size || {}).y || 0;
        if (ty === "FRAME" || ty === "COMPONENT" || ty === "SYMBOL" || ty === "INSTANCE") {
            const id = uniqId((n.name || "panel").replace(/[^\w]/g, "_").toLowerCase() || "panel");
            const fill = solidFill(n);
            const radius = Math.round(n.cornerRadius || 0);
            const p = panelWidget(id, x, y, w, h, Object.assign({}, fill ? { bg_color: fill } : { bg_opa: 0 }, radius ? { radius } : {}));
            out.push(p);
            emitChildren(k, x, y, p.children, hitCollector);
            hitCollector.push({ key: k, node: n, type: ty, widget: p, x, y, w, h });
            return p;
        }
        if (ty === "TEXT") {
            const chars = ((n.textData || {}).characters) || "";
            const fill = solidFill(n) || "#191919";
            const { font } = textFont(n.fontSize * scl, chars);
            const id = uniqId("label");
            const lw = Math.max(w, 12), lh = Math.max(h, 14);
            const lbl = labelWidget(id, x, y, lw, lh, chars, { text_color: fill, text_font: font });
            out.push(lbl);
            hitCollector.push({ key: k, node: n, type: ty, widget: lbl, x, y, w, h });
            return lbl;
        }
        if (ty === "VECTOR" || ty === "LINE") {
            const nm = ((n.name || "").split("/").pop() || "").toLowerCase().replace(/\s+/g, "-");
            const lucide = Object.keys(LUCIDE_TO_MS).find(l => nm === l || nm.includes(l));
            if (lucide && codepoints[LUCIDE_TO_MS[lucide]]) {
                const cp = codepoints[LUCIDE_TO_MS[lucide]];
                const { font } = textFont(Math.max(w, h), String.fromCodePoint(cp));
                const id = uniqId("icon_" + LUCIDE_TO_MS[lucide]);
                const lbl = labelWidget(id, x, y, Math.max(w, 12), Math.max(h, 12), String.fromCodePoint(cp),
                    { text_color: "#191919", text_font: font });
                out.push(lbl);
                hitCollector.push({ key: k, node: n, type: ty, widget: lbl, x, y, w, h });
                return lbl;
            }
            const id = uniqId((n.name || "vector").replace(/[^\w]/g, "_").toLowerCase() || "vector");
            const p = panelWidget(id, x, y, Math.max(w, 4), Math.max(h, 4), { bg_color: solidFill(n) || "#999999" });
            out.push(p);
            return p;
        }
        if (["RECTANGLE", "ROUNDED_RECTANGLE", "ELLIPSE", "STAR", "POLYGON", "BOOLEAN_OPERATION"].includes(ty)) {
            const id = uniqId((n.name || "shape").replace(/[^\w]/g, "_").toLowerCase() || "shape");
            const fill = solidFill(n);
            let radius = Math.round(n.cornerRadius || 0);
            if (ty === "ELLIPSE") radius = Math.round(Math.min(w, h) / 2);
            const p = panelWidget(id, x, y, w, h, Object.assign({}, fill ? { bg_color: fill } : { bg_opa: 0 }, radius ? { radius } : {}));
            out.push(p);
            hitCollector.push({ key: k, node: n, type: ty, widget: p, x, y, w, h });
            return p;
        }
        return null;
    }

    function emitChildren(k, ox, oy, out, hitCollector) {
        for (const c of kids.get(k) || []) emitNode(c, ox, oy, out, hitCollector);
    }

    const hitAreas = [];
    const screenChildren = [];
    const widgetByItemName = new Map();
    const iconFollowText = new Map();
    function emitFrameContent(frameKey, out) {
        for (const c of kids.get(frameKey) || []) {
            const n = nodes.get(c);
            const ty = typeStr(n);
            const t = n.transform || {};
            let symKey = null;
            if (n.symbolData && n.symbolData.symbolID) symKey = G(n.symbolData.symbolID);
            const cs = componentSets.find(s => s.key === symKey || s.key === c) ||
                componentSets.find(s => s.node.name === n.name);
            if (cs && defaultVariant) {
                const instFill = solidFill(n);
                const holder = panelWidget(uniqId("inst"), t.m02 || 0, t.m12 || 0,
                    (n.size || {}).x || 0, (n.size || {}).y || 0,
                    Object.assign({}, instFill ? { bg_color: instFill } : { bg_opa: 0 },
                        n.cornerRadius ? { radius: Math.round(n.cornerRadius) } : {}));
                for (const [name, cc] of defaultVariant.content) {
                    emitVariantItem(cc, holder, name, hitAreas);
                }
                out.push(holder);
            } else {
                emitNode(c, 0, 0, out, hitAreas);
            }
        }
    }

    function emitVariantItem(cc, holder, name, hitCollector) {
        const item = cc;
        const fill = item.fill;
        if (item.type === "TEXT") {
            const chars = item.text || "";
            const { font } = textFont(item.node.fontSize, chars);
            const lbl = labelWidget(uniqId("label"),
                item.x, item.y, Math.max(item.w, 12), Math.max(item.h, 14), chars,
                { text_color: fill || "#191919", text_font: font });
            holder.children.push(lbl);
            if (!widgetByItemName.has(name)) widgetByItemName.set(name, lbl);
            hitCollector.push({ key: item.key, node: item.node, type: "TEXT", widget: lbl, x: item.x, y: item.y, w: item.w, h: item.h });
        } else if (item.type === "VECTOR" || item.type === "INSTANCE") {
            const nm = ((item.node.name || "").split("/").pop() || "").toLowerCase().replace(/\s+/g, "-");
            const lucide = Object.keys(LUCIDE_TO_MS).find(l => nm === l || nm.includes(l));
            if (lucide && codepoints[LUCIDE_TO_MS[lucide]]) {
                const cp = codepoints[LUCIDE_TO_MS[lucide]];
                const { font } = textFont(Math.max(item.w, item.h), String.fromCodePoint(cp));
                const lbl = labelWidget(uniqId("icon_" + LUCIDE_TO_MS[lucide]), item.x, item.y,
                    Math.max(item.w, 12), Math.max(item.h, 12), String.fromCodePoint(cp),
                    { text_color: "#191919", text_font: font });
                holder.children.push(lbl);
                if (!widgetByItemName.has(name)) widgetByItemName.set(name, lbl);
                const sibText = [...defaultVariant.content.values()]
                    .filter(c => c.type === "TEXT" && Math.abs(c.x - item.x) < 40 && fillDiffNames.has(c.node.name))
                    .sort((a, b) => Math.abs(a.x - item.x) - Math.abs(b.x - item.x))[0];
                if (sibText) iconFollowText.set(lbl, sibText.node.name);
                hitCollector.push({ key: item.key, node: item.node, type: "VECTOR", widget: lbl, x: item.x, y: item.y, w: item.w, h: item.h });
            }
        } else {
            const p = panelWidget(uniqId((name || "shape").replace(/[^\w]/g, "_").toLowerCase() || "shape"),
                item.x, item.y, item.w, item.h,
                Object.assign({}, fill ? { bg_color: fill } : { bg_opa: 0 },
                    item.node.cornerRadius ? { radius: Math.round(item.node.cornerRadius) } : {}));
            holder.children.push(p);
            if (!widgetByItemName.has(name)) widgetByItemName.set(name, p);
            hitCollector.push({ key: item.key, node: item.node, type: item.type, widget: p, x: item.x, y: item.y, w: item.w, h: item.h });
        }
    }

    const rootBox = rootFrame.node.size || { x: 402, y: 874 };
    emitFrameContent(rootFrame.key, screenChildren);

    const components = [];
    const connectionLines = [];
    const variables = [];

    const xVarByNodeName = new Map();
    const colorAnimNodes = new Set();
    if (variants.length >= 2) {
        const names = new Set();
        for (const v of variants) for (const nm of v.content.keys()) names.add(nm);
        for (const nm of names) {
            const xs = variants.map(v => v.content.get(nm)).filter(Boolean).map(c => Math.round(c.x));
            const uniqX = [...new Set(xs)];
            if (uniqX.length > 1 && xs.length >= 2) xVarByNodeName.set(nm, uniqX);
            const fills = variants.map(v => v.content.get(nm)).filter(Boolean).map(c => c.fill).filter(Boolean);
            if (new Set(fills).size > 1) colorAnimNodes.add(nm);
        }
    }

    const widgetByName = new Map();
    const indexWidgets = w => { widgetByName.set(w.identifier, w); (w.children || []).forEach(indexWidgets); };
    screenChildren.forEach(indexWidgets);

    function matchWidget(v, itemName) {
        if (widgetByItemName.has(itemName)) return widgetByItemName.get(itemName);
        const item = v.content.get(itemName);
        if (!item) return null;
        let best = null, bestD = 1e9;
        const walk = w => {
            if (!(w.children || []).length && w.identifier) {
                const d = Math.abs(w.left - item.x) + Math.abs(w.top - item.y) + Math.abs(w.width - item.w) / 2;
                if (d < bestD) { bestD = d; best = w; }
            }
            (w.children || []).forEach(walk);
        };
        screenChildren.forEach(walk);
        return bestD < 40 ? best : null;
    }

    const chainByVariant = new Map();
    const linkVar = {};
    function goChain(v) {
        if (chainByVariant.has(v.key)) return chainByVariant.get(v.key);
        const comp = {
            objID: oid(), type: "LVGLActionComponent",
            left: 460 + chainByVariant.size * 200, top: 60 + chainByVariant.size * 90,
            width: 190, height: 70, customInputs: [], customOutputs: [], actions: []
        };
        const setv = {
            objID: oid(), type: "SetVariableActionComponent",
            left: comp.left, top: comp.top + 90, width: 190, height: 54,
            customInputs: [], customOutputs: [], entries: []
        };
        for (const [nm, xs] of xVarByNodeName) {
            const item = v.content.get(nm);
            if (!item) continue;
            const w = matchWidget(v, nm);
            if (!w) continue;
            const varName = (nm.replace(/[^\w]/g, "_").toLowerCase() || "anim") + "_x";
            if (!linkVar[varName]) {
                linkVar[varName] = true;
                variables.push({ objID: oid(), name: varName, type: "integer", defaultValue: String(xs[0]), persistent: false });
            }
            const inter = interactions.find(i => i.smart && i.targetKey === v.key) ||
                interactions.find(i => i.targetKey && variants.some(vv => vv.key === i.targetKey)) ||
                { duration: 500, k: 170, c: 15, v0: 0 };
            comp.actions.push({
                objID: oid(), action: "animSpring",
                object: w.identifier, objectType: "literal",
                start: varName, startType: "expression",
                end: Math.round(item.x), endType: "literal",
                delay: 0, delayType: "literal", time: inter.duration, timeType: "literal",
                instant: false, instantType: "literal",
                repeatCount: 0, repeatCountType: "literal", playback: false, playbackType: "literal",
                stiffness: inter.k, stiffnessType: "literal",
                damping: inter.c, dampingType: "literal",
                velocity: inter.v0, velocityType: "literal"
            });
            setv.entries.push({ objID: oid(), variable: varName, value: String(Math.round(item.x)) });
        }
        for (const nm of colorAnimNodes) {
            const item = v.content.get(nm);
            if (!item || !item.fill) continue;
            const w = matchWidget(v, nm);
            if (!w || w.type !== "LVGLLabelWidget") continue;
            const inter = interactions.find(i => i.smart && i.targetKey === v.key) ||
                interactions.find(i => i.targetKey && variants.some(vv => vv.key === i.targetKey)) ||
                { duration: 500, k: 170, c: 15 };
            const followers = [...iconFollowText.entries()].filter(([, tn]) => tn === nm).map(([wgt]) => wgt);
            for (const target of [w, ...followers]) {
                comp.actions.push({
                    objID: oid(), action: "animTextColor",
                    object: target.identifier, objectType: "literal",
                    start: 0, startType: "literal",
                    end: hexToInt(item.fill), endType: "literal",
                    delay: 0, delayType: "literal", time: inter.duration, timeType: "literal",
                    instant: false, instantType: "literal",
                    relative: true, relativeType: "literal",
                    path: "SPRING", pathType: "literal",
                    repeatCount: 0, repeatCountType: "literal", playback: false, playbackType: "literal",
                    stiffness: inter.k, stiffnessType: "literal",
                    damping: inter.c, dampingType: "literal"
                });
            }
        }
        components.push(comp, setv);
        connectionLines.push({ objID: oid(), source: comp.objID, output: "@seqout", target: setv.objID, input: "@seqin" });
        chainByVariant.set(v.key, comp);
        return comp;
    }

    let screenHandlers = [];
    const cellsByChain = new Map();
    const srcAbsInVariant = startKey => {
        let x = 0, y = 0, w = 0, h = 0, found = false, first = true;
        for (let k = startKey; k; ) {
            const n = nodes.get(k);
            if (!n) break;
            if (variants.some(v => v.key === k)) { found = true; break; }
            const t = n.transform || {};
            x += t.m02 || 0; y += t.m12 || 0;
            if (first) { w = (n.size || {}).x || 0; h = (n.size || {}).y || 0; first = false; }
            const p = n.parentIndex && n.parentIndex.guid;
            k = p ? G(p) : null;
        }
        return found ? { x, y, w, h } : null;
    };
    for (const it of interactions) {
        if (!it.targetKey || !it.smart) continue;
        const targetVariant = variants.find(v => v.key === it.targetKey);
        if (!targetVariant) continue;
        if (it.trigger === "AFTER_TIMEOUT" || it.trigger === "ON_LOAD") {
            screenHandlers.push({ objID: oid(), eventName: "SCREEN_LOADED", handlerType: "flow", userData: 0 });
            continue;
        }
        const chain = goChain(targetVariant);
        let cell = null;
        for (let k = it.srcKey; k; ) {
            const n = nodes.get(k);
            if (!n) break;
            if (variants.some(v => v.key === k)) break;
            if (typeStr(n) === "FRAME") {
                const box = srcAbsInVariant(k);
                if (box && box.w > 0) { cell = box; break; }
            }
            const p = n.parentIndex && n.parentIndex.guid;
            k = p ? G(p) : null;
        }
        if (!cell) {
            const box = srcAbsInVariant(it.srcKey);
            cell = box;
        }
        if (!cell) { console.warn(`  [warn] 交互源无几何: ${it.srcNode.name}`); continue; }
        if (!cellsByChain.has(chain.objID)) cellsByChain.set(chain.objID, []);
        const cells = cellsByChain.get(chain.objID);
        if (!cells.some(c => Math.abs(c.x - cell.x) < 5 && Math.abs(c.y - cell.y) < 5)) cells.push(cell);
    }
    const allCells = [...cellsByChain.values()].flat().slice().sort((a, b) => a.x - b.x);
    const boundOf = cell => {
        const i = allCells.indexOf(cell);
        const left = i > 0 ? Math.round((allCells[i - 1].x + allCells[i - 1].w + cell.x) / 2) : cell.x;
        const right = i < allCells.length - 1 ? Math.round((cell.x + cell.w + allCells[i + 1].x) / 2) : cell.x + cell.w;
        return { x0: left, x1: right };
    };
    for (const [chainId, cells] of cellsByChain) {
        const chain = components.find(c => c.objID === chainId);
        for (const cell of cells) {
            const { x0, x1 } = boundOf(cell);
            const hit = panelWidget(uniqId("hit"), x0, cell.y, Math.max(x1 - x0, 8), Math.max(cell.h, 12), { bg_opa: 0 });
            hit.widgetFlags = "CLICKABLE";
            hit.eventHandlers.push({ objID: oid(), eventName: "CLICKED", handlerType: "flow", userData: 0 });
            const holder = screenChildren.find(w => w.identifier === "inst") || screenChildren[screenChildren.length - 1];
            if (holder && holder.children) holder.children.push(hit);
            else screenChildren.push(hit);
            connectionLines.push({ objID: oid(), source: hit.objID, output: "CLICKED", target: chainId, input: "@seqin" });
        }
    }
    if (screenHandlers.length) screenHandlers = [screenHandlers[0]];

    const proj = newProject(args, rootBox, variables);
    proj.userPages.push({
        objID: oid(), name: "Main",
        left: 0, top: 0, width: Math.round(rootBox.x), height: Math.round(rootBox.y),
        createAtStart: true, deleteOnScreenUnload: false, isUsedAsUserWidget: false,
        localVariables: [], componentGroups: [], userProperties: [],
        connectionLines,
        components: []
    });
    const screen = panelWidget("screen", 0, 0, rootBox.x, rootBox.y, { bg_color: solidFill(rootFrame.node) || "#FFFFFF" });
    screen.type = "LVGLScreenWidget";
    screen.children = screenChildren;
    screen.eventHandlers = screenHandlers;
    proj.userPages[0].components.push(screen, ...components);

    applyFontsFrom(proj, fontUse, args);

    const out = args.out || args._[0].replace(/\.fig$/i, "") + ".eez-project";
    fs.mkdirSync(path.dirname(path.resolve(out)), { recursive: true });
    fs.writeFileSync(out, JSON.stringify(proj, null, 2), "utf8");
    console.log(`\n产出: ${out}`);
    console.log(`部件 ${countWidgets(screenChildren)} | 组件 ${components.length} | 连线 ${connectionLines.length} | 变量 ${variables.length} | 交互链 ${chainByVariant.size}`);
}

// EEZ LVGL C 导出的标准文件模板（screens.c/ui.c 等 12 件；无它 build 不产源码）
const LVGL_BUILD_FILE_TEMPLATES = [
    {
        "objID": "79e3eb40-09fb-4446-b04c-1ee2b7d09550",
        "fileName": "screens.h",
        "template": "#ifndef EEZ_LVGL_UI_SCREENS_H\r\n#define EEZ_LVGL_UI_SCREENS_H\r\n\r\n//${eez-studio LVGL_INCLUDE}\r\n\r\n#ifdef __cplusplus\r\nextern \"C\" {\r\n#endif\r\n\r\n//${eez-studio LVGL_SCREENS_DECL}\r\n//${eez-studio LVGL_SCREENS_DECL_EXT}\r\n\r\n#ifdef __cplusplus\r\n}\r\n#endif\r\n\r\n#endif /*EEZ_LVGL_UI_SCREENS_H*/"
    },
    {
        "objID": "6f617675-0c02-4cc3-e27d-80b2e83efd39",
        "fileName": "screens.c",
        "template": "#include <string.h>\n\n#include \"screens.h\"\n#include \"images.h\"\n#include \"fonts.h\"\n#include \"actions.h\"\n#include \"vars.h\"\n#include \"styles.h\"\n#include \"ui.h\"\n\n//${eez-studio LVGL_SCREENS_DEF}\n//${eez-studio LVGL_SCREENS_DEF_EXT}"
    },
    {
        "objID": "63fd23b2-9f4a-4e98-e3ca-990d35cae87a",
        "fileName": "actions.h",
        "template": "#ifndef EEZ_LVGL_UI_EVENTS_H\r\n#define EEZ_LVGL_UI_EVENTS_H\r\n\r\n//${eez-studio LVGL_INCLUDE}\r\n\r\n#ifdef __cplusplus\r\nextern \"C\" {\r\n#endif\r\n\r\n//${eez-studio LVGL_ACTIONS_DECL}\r\n\r\n#ifdef __cplusplus\r\n}\r\n#endif\r\n\r\n#endif /*EEZ_LVGL_UI_EVENTS_H*/"
    },
    {
        "objID": "7aea62c7-6057-441b-9c10-2433c8f7a94a",
        "fileName": "vars.h",
        "template": "#ifndef EEZ_LVGL_UI_VARS_H\r\n#define EEZ_LVGL_UI_VARS_H\r\n\r\n#include <stdint.h>\r\n#include <stdbool.h>\r\n\r\n#ifdef __cplusplus\r\nextern \"C\" {\r\n#endif\r\n\r\n// enum declarations\r\n\r\n//${eez-studio FLOW_ENUMS}\r\n\r\n// Flow global variables\r\n\r\n//${eez-studio FLOW_GLOBAL_VARIABLES_ENUM}\r\n\r\n// Native global variables\r\n\r\n//${eez-studio LVGL_VARS_DECL}\r\n\r\n#ifdef __cplusplus\r\n}\r\n#endif\r\n\r\n#endif /*EEZ_LVGL_UI_VARS_H*/"
    },
    {
        "objID": "bb7654c8-a526-401d-fac5-a872b17156c1",
        "fileName": "structs.h",
        "template": "#ifndef EEZ_LVGL_UI_STRUCTS_H\n#define EEZ_LVGL_UI_STRUCTS_H\n\n//${eez-studio EEZ_FOR_LVGL_CHECK}\n\n#if defined(EEZ_FOR_LVGL)\n\n#include <eez/flow/flow.h>\n#include <stdint.h>\n#include <stdbool.h>\n\n#include \"vars.h\"\n\nusing namespace eez;\n\n//${eez-studio FLOW_STRUCTS}\n\n//${eez-studio FLOW_STRUCT_VALUES}\n\n#endif\n\n#endif /*EEZ_LVGL_UI_STRUCTS_H*/\n"
    },
    {
        "objID": "4bb862c9-69dd-41a5-e483-df644cc10cd8",
        "fileName": "images.h",
        "template": "#ifndef EEZ_LVGL_UI_IMAGES_H\r\n#define EEZ_LVGL_UI_IMAGES_H\r\n\r\n//${eez-studio LVGL_INCLUDE}\r\n\r\n#ifdef __cplusplus\r\nextern \"C\" {\r\n#endif\r\n\r\n//${eez-studio LVGL_IMAGES_DECL}\r\n\r\n#ifdef __cplusplus\r\n}\r\n#endif\r\n\r\n#endif /*EEZ_LVGL_UI_IMAGES_H*/"
    },
    {
        "objID": "4fd43a87-33a4-4038-b15b-117eeed100d0",
        "fileName": "images.c",
        "template": "#include \"images.h\"\n\n//${eez-studio LVGL_IMAGES_DEF}"
    },
    {
        "objID": "102bd5c3-aa27-47b8-fa12-4bf6958eeda8",
        "fileName": "fonts.h",
        "template": "#ifndef EEZ_LVGL_UI_FONTS_H\r\n#define EEZ_LVGL_UI_FONTS_H\r\n\r\n//${eez-studio LVGL_INCLUDE}\r\n\r\n#ifdef __cplusplus\r\nextern \"C\" {\r\n#endif\r\n\r\n//${eez-studio LVGL_FONTS_DECL}\r\n\r\n#ifdef __cplusplus\r\n}\r\n#endif\r\n\r\n#endif /*EEZ_LVGL_UI_FONTS_H*/"
    },
    {
        "objID": "be0f00df-812e-4ed4-948f-dcb49e636a46",
        "fileName": "styles.h",
        "template": "#ifndef EEZ_LVGL_UI_STYLES_H\r\n#define EEZ_LVGL_UI_STYLES_H\r\n\r\n//${eez-studio LVGL_INCLUDE}\r\n\r\n#ifdef __cplusplus\r\nextern \"C\" {\r\n#endif\r\n\r\n//${eez-studio LVGL_STYLES_DECL}\r\n\r\n#ifdef __cplusplus\r\n}\r\n#endif\r\n\r\n#endif /*EEZ_LVGL_UI_STYLES_H*/"
    },
    {
        "objID": "6409b27d-bce0-4de0-dc39-00614847d276",
        "fileName": "styles.c",
        "template": "#include \"styles.h\"\n#include \"images.h\"\n#include \"fonts.h\"\n\n//${eez-studio LVGL_STYLES_DEF}\n"
    },
    {
        "objID": "d05c0814-03ab-4c21-cef9-684513ff2adc",
        "fileName": "ui.h",
        "template": "#ifndef EEZ_LVGL_UI_GUI_H\n#define EEZ_LVGL_UI_GUI_H\n\n//${eez-studio LVGL_INCLUDE}\n\n//${eez-studio EEZ_FOR_LVGL_CHECK}\n\n#if defined(EEZ_FOR_LVGL)\n#include <eez/flow/lvgl_api.h>\n#endif\n\n#if !defined(EEZ_FOR_LVGL)\n#include \"screens.h\"\n#endif\n\n#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n//${eez-studio GUI_ASSETS_DECL}\n\nvoid ui_init();\nvoid ui_tick();\n\n#if !defined(EEZ_FOR_LVGL)\nvoid loadScreen(enum ScreensEnum screenId);\n#endif\n\n#ifdef __cplusplus\n}\n#endif\n\n#endif // EEZ_LVGL_UI_GUI_H"
    },
    {
        "objID": "084926f1-634e-4671-9fc3-cc34c950eba7",
        "fileName": "ui.c",
        "template": "#if defined(EEZ_FOR_LVGL)\n#include <eez/core/vars.h>\n#endif\n\n#include \"ui.h\"\n#include \"screens.h\"\n#include \"images.h\"\n#include \"actions.h\"\n#include \"vars.h\"\n\n//${eez-studio GUI_ASSETS_DEF}\n\n//${eez-studio LVGL_NATIVE_VARS_TABLE_DEF}\n\n//${eez-studio LVGL_ACTIONS_ARRAY_DEF}\n\n#if defined(EEZ_FOR_LVGL)\n\nvoid ui_init() {\n    eez_flow_init(assets, sizeof(assets), (lv_obj_t **)&objects, sizeof(objects), images, sizeof(images), actions);\n}\n\nvoid ui_tick() {\n    eez_flow_tick();\n    tick_screen(g_currentScreen);\n}\n\n#else\n\nstatic int16_t currentScreen = -1;\n\nstatic lv_obj_t *getLvglObjectFromIndex(int32_t index) {\n    if (index == -1) {\n        return 0;\n    }\n    return ((lv_obj_t **)&objects)[index];\n}\n\nvoid loadScreen(enum ScreensEnum screenId) {\n    currentScreen = screenId - 1;\n    lv_obj_t *screen = getLvglObjectFromIndex(currentScreen);\n    lv_scr_load_anim(screen, LV_SCR_LOAD_ANIM_FADE_IN, 200, 0, false);\n}\n\nvoid ui_init() {\n    create_screens();\n//${eez-studio LVGL_LOAD_FIRST_SCREEN}\n}\n\nvoid ui_tick() {\n    tick_screen(currentScreen);\n}\n\n#endif\n"
    }
];

function newProject(args, box, variables) {
    return {
        themesVersion: 1,
        objID: oid(),
        settings: {
            objID: oid(),
            general: {
                objID: oid(), projectType: "lvgl", projectVersion: "v3",
                lvglVersion: "9.4.0", extensions: [], imports: [], flowSupport: true,
                displayWidth: Math.round(box.x), displayHeight: Math.round(box.y),
                description: `figma2eez import: ${path.basename(args._[0])}`
            },
            build: {
                objID: oid(),
                configurations: [{ objID: oid(), name: "Default" }],
                files: LVGL_BUILD_FILE_TEMPLATES,
                destinationFolder: "src",
                imageExportMode: "source",
                // F7 模拟器链（emcc）依赖 eez-framework amalgamation 产物（eez-flow.cpp
                // + eez/ 头），不开则 src/ 缺框架、模拟器编译炸 eez/flow/lvgl_api.h
                generateSourceCodeForEezFramework: true
            }
        },
        themes: [{ objID: oid(), name: "Default", colors: [] }],
        lvglStyles: { objID: oid(), styles: [] },
        fonts: [], bitmaps: [],
        variables: { objID: oid(), globalVariables: variables, structures: [], enums: [] },
        actions: [],
        userPages: []
    };
}

function countWidgets(ws) { let n = 0; for (const w of ws) { n++; n += countWidgets(w.children || []); } return n; }

// ================================================================
// v2 引擎：多帧状态组 + PUSH 转场（08 - Watch 路径）
// ================================================================
const PUSH_TO_FADE = {
    PUSH_FROM_RIGHT: "MOVE_LEFT",   // 新屏从右进 → 旧屏被向左推
    PUSH_FROM_LEFT: "MOVE_RIGHT",
    PUSH_FROM_TOP: "MOVE_BOTTOM",
    PUSH_FROM_BOTTOM: "MOVE_TOP"
};

function runV2(args, pageKey, page, ctx, codepoints) {
    const { nodes, order, kids } = ctx;
    const bitmapReg = makeBitmapRegistry(ctx);
    // 同位 IMAGE 组共享最大圆角（右屏图标=圆底+方 Overlay 组，视觉整组圆）
    const radiusByGeo = new Map();

    // ---- 帧与交互收集 ----
    const frameKeys = (kids.get(pageKey) || []).filter(k => typeStr(nodes.get(k)) === "FRAME");
    const frameOrder = new Map(frameKeys.map((k, i) => [k, i]));
    const topFrameOf = new Map(); // 页内任意节点 → 顶层 frame key
    for (const fk of frameKeys) {
        const stack = [fk];
        while (stack.length) {
            const k = stack.pop();
            topFrameOf.set(k, fk);
            for (const c of kids.get(k) || []) stack.push(c);
        }
    }

    const interactions = [];
    for (const k of order) {
        if (!topFrameOf.has(k)) continue;
        const n = nodes.get(k);
        for (const it of n.prototypeInteractions || []) {
            const ev = it.event || {};
            for (const ca of it.actions || []) {
                for (const a of (ca.actions && ca.actions.length ? ca.actions : [ca])) {
                    const targetKey = a.transitionNodeID ? G(a.transitionNodeID) : null;
                    if (!targetKey || !topFrameOf.has(targetKey)) continue;
                    const nav = a.transitionType || "";
                    const push = /^PUSH_/.test(nav);
                    interactions.push({
                        srcKey: k, srcFrame: topFrameOf.get(k),
                        trigger: ev.interactionType,
                        targetKey, targetFrame: topFrameOf.get(targetKey),
                        nav, push, smart: !!a.transitionShouldSmartAnimate,
                        duration: Math.round((a.transitionDuration || 0) * 1000) || 230
                    });
                }
            }
        }
    }
    const stateEdges = interactions.filter(i => i.smart && !i.push && i.targetFrame !== i.srcFrame && ["DRAG", "ON_CLICK", "ON_TAP", "CLICK"].includes(i.trigger));
    const pushEdges = interactions.filter(i => i.push && ["DRAG", "ON_CLICK", "ON_TAP", "CLICK"].includes(i.trigger));
    console.log(`帧 ${frameKeys.length} 个 | 状态边 ${stateEdges.length} | PUSH 边 ${pushEdges.length}`);

    // ---- 并查集分组（SMART 边相连的帧 = 同一 screen 的状态）----
    // 启发式：源帧只挂着一条 SMART 边、且该帧另有 PUSH 连接时，这条边视为
    // 惰性跨屏边（如详情屏里停在屏外的卡片）丢弃——否则会把 PUSH 对端
    // 屏并进轮盘状态组
    const smartDegree = new Map(frameKeys.map(k => [k, 0]));
    for (const e of stateEdges) {
        smartDegree.set(e.srcFrame, (smartDegree.get(e.srcFrame) || 0) + 1);
        smartDegree.set(e.targetFrame, (smartDegree.get(e.targetFrame) || 0) + 1);
    }
    const pushConnected = new Set();
    for (const e of pushEdges) { pushConnected.add(e.srcFrame); pushConnected.add(e.targetFrame); }
    const keptStateEdges = stateEdges.filter(e => {
        const lonely = smartDegree.get(e.srcFrame) === 1 || smartDegree.get(e.targetFrame) === 1;
        if (lonely && (pushConnected.has(e.srcFrame) || pushConnected.has(e.targetFrame))) {
            console.log(`  [drop] 惰性跨屏边: "${nodes.get(e.srcFrame).name}" → "${nodes.get(e.targetFrame).name}"`);
            return false;
        }
        return true;
    });
    const parent = new Map(frameKeys.map(k => [k, k]));
    const find = k => (parent.get(k) === k ? k : (parent.set(k, find(parent.get(k))), parent.get(k)));
    for (const e of keptStateEdges) parent.set(find(e.srcFrame), find(e.targetFrame));
    const groups = new Map();
    for (const fk of frameKeys) {
        const r = find(fk);
        if (!groups.has(r)) groups.set(r, []);
        groups.get(r).push(fk);
    }
    // 状态顺序 = 组内画布 x（Figma 状态序列帧按从左到右铺设）；屏顺序 = 组内最小 x
    const frameX = new Map(frameKeys.map(k => [k, ((nodes.get(k).transform || {}).m02) || 0]));
    const screens = [...groups.values()]
        .map(g => g.slice().sort((a, b) => frameX.get(a) - frameX.get(b)))
        .sort((a, b) => frameX.get(a[0]) - frameX.get(b[0]));
    const screenIndexOfFrame = new Map();
    screens.forEach((g, gi) => g.forEach(fk => screenIndexOfFrame.set(fk, gi)));

    // ---- 公共发射设施 ----
    const fontUse = new Map();
    const textFont = makeTextFont(fontUse);
    const idUsed = new Set();
    const uniqId = base => { let i = 1, id = base; while (idUsed.has(id)) id = base + i++; idUsed.add(id); return id; };
    const globalVars = [];
    const usedVarNames = new Set();
    const addVar = (name, def) => {
        let n = name, i = 1;
        while (usedVarNames.has(n)) n = name + i++;
        usedVarNames.add(n);
        globalVars.push({ objID: oid(), name: n, type: "integer", defaultValue: String(def), persistent: false });
        return n;
    };

    // INSTANCE 展开（覆盖按 guid 应用；卡面=最大实心叶；阴影=derivedSymbolData/自身 effects）
    function expandInstance(instNode) {
        const masterKey = instNode.symbolData && instNode.symbolData.symbolID ? G(instNode.symbolData.symbolID) : null;
        const master = masterKey ? nodes.get(masterKey) : null;
        if (!master) return null;
        const mw = (master.size || {}).x || 1, mh = (master.size || {}).y || 1;
        const iw = (instNode.size || {}).x || mw, ih = (instNode.size || {}).y || mh;
        const sx = iw / mw, sy = ih / mh;
        const leaves = [];
        (function walk(k, ox, oy) {
            for (const c of kids.get(k) || []) {
                const n = nodes.get(c);
                if (n.visible === false) continue;
                const t = n.transform || {};
                const x = (t.m02 || 0) + ox, y = (t.m12 || 0) + oy;
                const ty = typeStr(n);
                if (["FRAME", "SYMBOL", "COMPONENT", "INSTANCE", "GROUP"].includes(ty)) walk(c, x, y);
                else leaves.push({ key: c, node: n, type: ty, x, y, w: (n.size || {}).x || 0, h: (n.size || {}).y || 0 });
            }
        })(masterKey, 0, 0);
        const ovs = new Map();
        for (const o of (instNode.symbolData && instNode.symbolData.symbolOverrides) || []) {
            const gs = (o.guidPath && o.guidPath.guids) || [];
            if (gs.length) {
                const leaf = G(gs[gs.length - 1]);
                ovs.set(leaf, Object.assign(ovs.get(leaf) || {}, o));
            }
        }
        // face = DFS 序最底的大面积叶（Figma 卡片背景=子树最底层；
        // 上层同尺寸矢量多为设计废层，如本例的紫色 Card 4——原型中不可见）
        let face = null;
        for (const lf of leaves) {
            if (["VECTOR", "ROUNDED_RECTANGLE", "RECTANGLE"].includes(lf.type)) {
                const ovd = ovs.get(lf.key);
                const fill = (ovd && ovd.fillPaints && solidFill({ fillPaints: ovd.fillPaints })) || solidFill(lf.node) || gradientFill(lf.node);
                if (fill && lf.w >= mw * 0.9 && lf.h >= mh * 0.9) { face = lf; break; }
            }
        }
        const faceOv = face ? ovs.get(face.key) : null;
        const faceFill = face ? ((faceOv && faceOv.fillPaints && (solidFill({ fillPaints: faceOv.fillPaints }) || gradientFill({ fillPaints: faceOv.fillPaints }))) || solidFill(face.node) || gradientFill(face.node)) : solidFill(instNode);
        const radius = Math.round((face && face.node.cornerRadius) || instNode.cornerRadius || 0);
        const dsd = (instNode.symbolData && instNode.symbolData.derivedSymbolData) || [];
        const fxSrc = dsd.find(d => d.effects && d.effects.length) || instNode;
        const fx = (fxSrc.effects || [])
            .filter(e => e.type === "DROP_SHADOW" && e.visible !== false)
            .sort((a, b) => (b.radius || 0) - (a.radius || 0))[0];
        const panelStyle = Object.assign({}, radius ? { radius } : {});
        if (typeof faceFill === "string") panelStyle.bg_color = faceFill;
        else if (faceFill && typeof faceFill === "object") Object.assign(panelStyle, faceFill); // 渐变三件套
        else panelStyle.bg_opa = 0;
        if (fx) {
            const hex = v => Math.round(Math.max(0, Math.min(1, v)) * 255).toString(16).padStart(2, "0");
            panelStyle.shadow_color = "#" + hex(fx.color.r) + hex(fx.color.g) + hex(fx.color.b) + (fx.color.a < 1 ? hex(fx.color.a) : "");
            panelStyle.shadow_width = Math.round(fx.radius || 0);
            panelStyle.shadow_ofs_x = Math.round((fx.offset || {}).x || 0);
            panelStyle.shadow_ofs_y = Math.round((fx.offset || {}).y || 0);
            panelStyle.shadow_spread = Math.round(fx.spread || 0);
        }
        return { leaves, ovs, sx, sy, face, panelStyle, radius };
    }

    function emitInstanceChildren(instNode, holder, baseId) {
        const ex = expandInstance(instNode);
        if (!ex) return;
        if (process.env.F2E_DEBUG) {
            const hit = ex.leaves.filter(lf => ex.ovs.has(lf.key)).length;
            const texts = ex.leaves.filter(lf => lf.type === "TEXT").map(lf => {
                const ov = ex.ovs.get(lf.key) || {};
                return ((ov.textData && ov.textData.characters) || "(" + ((lf.node.textData || {}).characters || "") + ")");
            });
            console.log(`  [inst ${instNode.name}] leaves=${ex.leaves.length} ovHits=${hit} ovsSize=${ex.ovs.size} texts=${texts.join("|")}`);
        }
        let li = 0, ii = 0;
        // 同位置 IMAGE 叶的选择：实例覆盖层 > 母版层（覆盖语义=换图）。Smart Animate
        // 官方渲染是动画终态（母版层淡出），静态 .fig 查不到 opacity 动画，按覆盖
        // 优先近似；同档内取 z 最高。跨档/跨实例各自成图，hash 不同即不同 bitmap
        const topImgLeaf = new Map(); // "x,y,w,h" → { leaf, covered }
        for (const lf of ex.leaves) {
            if (lf === ex.face) continue;
            if (!["RECTANGLE", "ROUNDED_RECTANGLE", "ELLIPSE"].includes(lf.type)) continue;
            const ov = ex.ovs.get(lf.key) || {};
            const paints = (ov.fillPaints && ov.fillPaints.length ? ov.fillPaints : lf.node.fillPaints) || [];
            const imgP = imagePaint({ fillPaints: paints });
            if (!imgP) continue;
            const sig = `${Math.round(lf.x)},${Math.round(lf.y)},${Math.round(lf.w)},${Math.round(lf.h)}`;
            const covered = !!(ov.fillPaints && ov.fillPaints.length && imagePaint({ fillPaints: ov.fillPaints }));
            const prev = topImgLeaf.get(sig);
            // covered 优先；同档取后出现的（z 高）
            if (!prev || (covered && !prev.covered) || (covered === prev.covered)) topImgLeaf.set(sig, { leaf: lf, covered });
        }
        for (const lf of ex.leaves) {
            if (lf === ex.face) continue;
            if (lf.type === "TEXT") {
                const ov = ex.ovs.get(lf.key) || {};
                const chars = (ov.textData && ov.textData.characters) || ((lf.node.textData || {}).characters) || "";
                if (!chars) continue;
                const x = lf.x * ex.sx, y = lf.y * ex.sy;
                const w = lf.w * ex.sx, h = lf.h * ex.sy;
                const fill = solidFill({ fillPaints: (ov.fillPaints || lf.node.fillPaints || []) }) || "#FFFFFF";
                // 字号随实例缩放（Figma 后卡=母版 80%/73% 实例，文字同步缩小；
                // 原字号在缩小的卡上会过大溢出卡边——"文字偏移出卡"的真凶）
                const { font } = textFont(lf.node.fontSize * Math.sqrt(ex.sx * ex.sy), chars);
                const opa = ov.opacity !== undefined ? ov.opacity : (lf.node.opacity !== undefined ? lf.node.opacity : 1);
                const style = { text_color: fill, text_font: font, text_align: isButtonText(lf.node, kids, nodes) ? "CENTER" : figTextAlign(lf.node) };
                if (opa < 1) style.text_opa = Math.round(opa * 255);
                // 垂直居中补偿（Figma 行高框 vs LVGL 顶对齐）
                const lineH = (lf.node.fontSize || 12) * 1.25;
                const dy = h > lineH * 1.45 ? Math.round((h - lineH) / 2) : 0;
                holder.children.push(labelWidget(uniqId(`${baseId}_t${li++}`), x, y + dy, Math.max(w, 12), Math.max(h, 14), chars, style, { fontSize: lf.node.fontSize || 12 }));
            } else if (lf.type === "VECTOR") {
                // 小图标矢量 → MSR 符号（名匹配或数值伴生推断）；字号=图标几何分档
                const sym = vectorSymbol(lf.node, lf.w, lf.h) || siblingMetricSymbol(lf.node, kids, nodes);
                if (!sym || !codepoints[sym]) continue;
                const symCh = String.fromCodePoint(codepoints[sym]);
                const x = lf.x * ex.sx, y = lf.y * ex.sy;
                const w = lf.w * ex.sx, h = lf.h * ex.sy;
                const fsz = Math.max(8, Math.min(16, Math.round(Math.max(w, h))));
                const fname = "iconsym_" + fsz;
                if (!fontUse.has(fname)) fontUse.set(fname, new Set());
                fontUse.get(fname).add(symCh);
                const iw2 = Math.max(fsz + 2, 12), ih2 = Math.max(fsz + 2, 12);
                const fill = solidFill({ fillPaints: ((ex.ovs.get(lf.key) || {}).fillPaints || lf.node.fillPaints || []) }) || "#FFFFFF";
                holder.children.push(labelWidget(uniqId(`${baseId}_s${ii++}`), x - (iw2 - w) / 2, y - (ih2 - h) / 2, iw2, ih2, symCh,
                    { text_color: fill, text_font: fname }));
            } else if (["RECTANGLE", "ROUNDED_RECTANGLE", "ELLIPSE"].includes(lf.type)) {
                // 同位多层：只发选中的那层（覆盖优先，见 topImgLeaf 规则）
                const sig = `${Math.round(lf.x)},${Math.round(lf.y)},${Math.round(lf.w)},${Math.round(lf.h)}`;
                const chosen = topImgLeaf.get(sig);
                if (!chosen || chosen.leaf !== lf) continue;
                const ov = ex.ovs.get(lf.key) || {};
                const paints = (ov.fillPaints && ov.fillPaints.length ? ov.fillPaints : lf.node.fillPaints) || [];
                const imgP = imagePaint({ fillPaints: paints });
                if (!imgP) continue;
                const x = lf.x * ex.sx, y = lf.y * ex.sy;
                const w = lf.w * ex.sx, h = lf.h * ex.sy;
                // 圆角读覆盖值优先（Overlay 层的 cornerRadius 覆盖=圆形哨兵 1000）
                const crNode = ov.cornerRadius !== undefined ? { cornerRadius: ov.cornerRadius } : lf.node;
                const bmp = bitmapReg.register(imgP, w, h, imgRadius(crNode, w, h));
                if (!bmp) continue;
                holder.children.push(imageWidget(uniqId(`${baseId}_i${ii++}`), x, y, w, h, bmp.name,
                    256, imgRadius(crNode, w, h)));
            }
        }
    }

    // 普通节点 → 部件树（装饰件不拦截点击；skipKeys=被 ghost 替代的叶）
    // 3x3 仿射矩阵工具（行主序 [a,b,tx,c,d,ty]）。kiwi transform 完整语义：
    // 父坐标 = 累计矩阵 × 节点矩阵 × 自身坐标（原点=左上）。历史实现只读平移
    // 分量——Timer 组 180° 翻转+Oval 0.73 缩放全被忽略（环错位 112px/尺寸偏大）
    const M3 = {
        ident: () => [1, 0, 0, 0, 1, 0],
        ofNode: n => { const t = n.transform || {}; return [t.m00 !== undefined ? t.m00 : 1, t.m01 || 0, t.m02 || 0, t.m10 || 0, t.m11 !== undefined ? t.m11 : 1, t.m12 || 0]; },
        mul: (A, B) => [
            A[0] * B[0] + A[1] * B[3], A[0] * B[1] + A[1] * B[4], A[0] * B[2] + A[1] * B[5] + A[2],
            A[3] * B[0] + A[4] * B[3], A[3] * B[1] + A[4] * B[4], A[3] * B[2] + A[4] * B[5] + A[5],
        ],
        apply: (M, x, y) => [M[0] * x + M[1] * y + M[2], M[3] * x + M[4] * y + M[5]],
    };
    // 节点在累计矩阵下的渲染框（轴对齐 bbox）+ 均匀缩放率
    function nodeBox(n, M) {
        // 中心锚定：中心=累计矩阵作用于自身中心；尺寸=原始×均匀缩放。
        // 旋转节点（Oval 43°、Timer 180°）bbox 会放大框且中心漂移，LVGL 无旋转，
        // 中心锚定是无旋转渲染的最佳近似；无旋转时与平移法逐像素等价
        const Mb = M3.mul(M, M3.ofNode(n));
        const w0 = (n.size || {}).x || 0, h0 = (n.size || {}).y || 0;
        const scale = Math.sqrt(Math.abs(Mb[0] * Mb[4] - Mb[1] * Mb[3])) || 1;
        const c = M3.apply(Mb, w0 / 2, h0 / 2);
        return {
            x: c[0] - w0 * scale / 2, y: c[1] - h0 * scale / 2,
            w: w0 * scale, h: h0 * scale,
            M: Mb, scale
        };
    }

    function emitStaticNode(k, M, out, skipKeys) {
        const n = nodes.get(k);
        const ty = typeStr(n);
        if (n.visible === false) return null;
        if (skipKeys && skipKeys.has(k)) return null;
        const box = nodeBox(n, M);
        const { x, y } = box;
        const w = box.w, h = box.h;
        const scl = box.scale; // 均匀缩放（字号/描边/圆角随缩）
        const id = uniqId(sanitize(n.name, "n"));
        if (["FRAME", "SYMBOL", "COMPONENT", "GROUP", "INSTANCE"].includes(ty)) {
            if (ty === "INSTANCE") {
                const ex = expandInstance(n);
                const fill = solidFill(n);
                const radius = Math.round(n.cornerRadius || 0);
                const p = panelWidget(id, x, y, w, h, Object.assign({}, ex ? ex.panelStyle : (fill ? { bg_color: fill } : { bg_opa: 0 }), radius && !(ex && ex.radius) ? { radius } : {}));
                emitInstanceChildren(n, p, id);
                out.push(p);
                return p;
            }
            const fill = solidFill(n);
            const opa = fillOpa(n);
            const radius = Math.round(n.cornerRadius || 0);
            const style = Object.assign({}, radius ? { radius } : {});
            if (fill) { style.bg_color = fill; if (opa < 255) style.bg_opa = opa; }
            else style.bg_opa = 0;
            const p = panelWidget(id, x, y, w, h, style);
            out.push(p);
            // 同位同图位图叶只留 parentIndex 最上一层（Figma 底图常复制两层做效果，
            // 叠两遍除浪费外还可能与 zoom 重采样产生重影）
            const topImg = new Map(); // 签名 → 最上层 key（kids 序=层序，同签名后者覆盖）
            for (const c of kids.get(k) || []) {
                const cn = nodes.get(c);
                if (!cn || cn.visible === false) continue;
                const ip = imagePaint(cn);
                if (!ip) continue;
                const ct = cn.transform || {};
                topImg.set(`${hashHex(ip.image.hash)}@${Math.round(ct.m02 || 0)},${Math.round(ct.m12 || 0)},${Math.round((cn.size || {}).x || 0)},${Math.round((cn.size || {}).y || 0)}`, c);
            }
            const dupImgKeys = new Set(); if (process.env.F2E_DEBUG) console.log(`    [dedup-scan] FRAME `+`"${n.name}" children=${(kids.get(k)||[]).length} topImgSigs=${topImg.size}`);
            for (const [sig, topKey] of topImg) {
                const sigHash = sig.split("@")[0];
                const [, geo] = sig.split("@");
                const [gx, gy, gw, gh] = geo.split(",").map(Number);
                for (const sib of kids.get(k) || []) {
                    if (sib === topKey) break;
                    const sn = nodes.get(sib);
                    if (!sn || sn.visible === false) continue;
                    const sp = imagePaint(sn);
                    if (!sp || hashHex(sp.image.hash) !== sigHash) continue;
                    const st = sn.transform || {};
                    if (Math.round(st.m02 || 0) === gx && Math.round(st.m12 || 0) === gy &&
                        Math.round((sn.size || {}).x || 0) === gw && Math.round((sn.size || {}).y || 0) === gh) dupImgKeys.add(sib);
                }
            }
            if (process.env.F2E_DEBUG && dupImgKeys.size) console.log(`    [dedup] "${n.name}" 去重 ${dupImgKeys.size} 层`);
            // 同位（同几何不同图）IMAGE 组的 radius 对齐取组内最大——Figma 右屏图标
            // 组=圆底(cornerRadius 哨兵)+方形 Overlay 叠放，视觉整组是圆；单层各自
            // 烘焙会让方形 Overlay 露角
            for (const c of kids.get(k) || []) {
                const cn = nodes.get(c);
                if (!cn || cn.visible === false) continue;
                const ip = imagePaint(cn);
                if (!ip) continue;
                const ct = cn.transform || {};
                const sig = `${Math.round(ct.m02 || 0)},${Math.round(ct.m12 || 0)},${Math.round((cn.size || {}).x || 0)},${Math.round((cn.size || {}).y || 0)}`;
                const r = cn.cornerRadius || 0;
                if (r > (radiusByGeo.get(sig) || 0)) radiusByGeo.set(sig, r);
            }
            for (const c of kids.get(k) || []) {
                // children 坐标相对父部件（EEZ LVGL 语义）——fig 帧内子节点
                // 的 transform 本就是相对父的，直接传 0,0；传 x,y 会双重偏移
                // （内容被父 panel clip_corner 裁掉=整屏黑）
                emitStaticNode(c, box.M, p.children, skipKeys ? new Set([...skipKeys, ...dupImgKeys]) : (dupImgKeys.size ? new Set(dupImgKeys) : null));
            }
            return p;
        }
        if (ty === "TEXT") {
            const chars = ((n.textData || {}).characters) || "";
            if (!chars) return null;
            const fill = solidFill(n) || "#191919";
            const { font } = textFont(n.fontSize, chars);
            const opa = n.opacity !== undefined ? n.opacity : 1;
            const style = { text_color: fill, text_font: font, text_align: isButtonText(n, kids, nodes) ? "CENTER" : figTextAlign(n) };
            if (opa < 1) style.text_opa = Math.round(opa * 255);
            // Figma 文本垂直居中于行高框，LVGL label 顶对齐——单行框高补偿下移
            const lineH = (n.fontSize || 12) * 1.25;
            const dy = h > lineH * 1.45 ? Math.round((h - lineH) / 2) : 0;
            const lbl = labelWidget(id, x, y + dy, Math.max(w, 12), Math.max(h, 14) - dy * 0, chars, style, { fontSize: n.fontSize || 12 });
            out.push(lbl);
            return lbl;
        }
        // 同位 IMAGE 组共享圆角（组表由 FRAME 分支填充；无组值时回退节点自身）
        const groupRadius = () => {
            const gs = `${Math.round(x)},${Math.round(y)},${Math.round(w)},${Math.round(h)}`;
            return radiusByGeo.has(gs) ? { cornerRadius: radiusByGeo.get(gs) } : n;
        };
        if (ty === "VECTOR" || ty === "LINE") {
            const nm = (n.name || "").toLowerCase();
            const imgP = imagePaint(n);
            if (imgP) {
                const bmp = bitmapReg.register(imgP, w, h, imgRadius(groupRadius(), w, h));
                if (bmp) {
                    const iw = imageWidget(id, x, y, w, h, bmp.name, 256, imgRadius(n, w, h));
                    out.push(iw);
                    return iw;
                }
                return null;
            }
            const sym = vectorSymbol(n, w, h) || siblingMetricSymbol(n, kids, nodes);
            const symCh = sym && codepoints[sym] ? String.fromCodePoint(codepoints[sym]) : null;
            if (symCh) {
                const fsz = Math.max(8, Math.min(16, Math.round(Math.max(w, h))));
                const fname = "iconsym_" + fsz;
                if (!fontUse.has(fname)) fontUse.set(fname, new Set());
                fontUse.get(fname).add(symCh);
                const fill = solidFill(n) || "#191919";
                const iw2 = Math.max(fsz + 2, 12), ih2 = Math.max(fsz + 2, 12);
                // 居中补偿：label 框放大后几何中心对齐节点中心
                const lbl = labelWidget(id, x - (iw2 - w) / 2, y - (ih2 - h) / 2, iw2, ih2, symCh,
                    { text_color: fill, text_font: fname });
                out.push(lbl);
                return lbl;
            }
            const fill = solidFill(n);
            const stroke = strokeInfo(n);
            if (!fill && !stroke) return null;
            // 小实心矢量：仅圆点/圆类按圆 panel 渲染，其余（闪电等异形）不画假方块
            if (!stroke && w <= 16 && h <= 16 && !/dot|circle|oval|ellipse/.test(nm)) return null;
            const nearSquare = Math.abs(w - h) <= Math.max(w, h) * 0.15;
            const radius = nearSquare || /oval|circle|ellipse|dot/.test(nm) ? Math.round(Math.min(w, h) / 2) : Math.round(n.cornerRadius || 0);
            const style = Object.assign({},
                fill ? { bg_color: fill } : { bg_opa: 0 },
                fill && fillOpa(n) < 255 ? { bg_opa: fillOpa(n) } : {},
                radius ? { radius } : {},
                stroke && stroke.width > 0 ? { border_color: stroke.color, border_width: Math.max(1, Math.round(stroke.width * scl)) } : {});
            const p = panelWidget(id, x, y, Math.max(w, 4), Math.max(h, 4), style);
            out.push(p);
            return p;
        }
        if (["RECTANGLE", "ROUNDED_RECTANGLE", "ELLIPSE", "STAR", "POLYGON", "BOOLEAN_OPERATION"].includes(ty)) {
            const imgP = imagePaint(n);
            if (imgP) {
                const bmp = bitmapReg.register(imgP, w, h, imgRadius(groupRadius(), w, h));
                if (bmp) {
                    const iw = imageWidget(id, x, y, w, h, bmp.name, 256, imgRadius(groupRadius(), w, h));
                    out.push(iw);
                    return iw;
                }
                return null;
            }
            const fill = solidFill(n);
            const opa = fillOpa(n);
            const stroke = strokeInfo(n);
            let radius = Math.round(n.cornerRadius || 0);
            if (ty === "ELLIPSE") radius = Math.round(Math.min(w, h) / 2);
            const style = Object.assign({},
                fill ? { bg_color: fill } : { bg_opa: 0 },
                opa < 255 ? { bg_opa: opa } : {},
                radius ? { radius } : {},
                stroke && stroke.width > 0 ? { border_color: stroke.color, border_width: Math.max(1, Math.round(stroke.width * scl)) } : {});
            const p = panelWidget(id, x, y, w, h, style);
            out.push(p);
            return p;
        }
        return null;
    }

    // ---- 逐屏构建 ----
    const pageBox = { x: 0, y: 0 };
    const pagesOut = [];
    const interactionData = []; // no-flow 模式：C 生成所需的路由数据

    screens.forEach((group, gi) => {
        const scrName = sanitize(nodes.get(group[0]).name, "scr");
        const states = group.slice().sort((a, b) => frameOrder.get(a) - frameOrder.get(b));
        const f0 = nodes.get(states[0]);
        const fw = Math.round((f0.size || {}).x), fh = Math.round((f0.size || {}).y);
        if (fw > pageBox.x) pageBox.x = fw;
        if (fh > pageBox.y) pageBox.y = fh;
        const hasStates = states.length > 1;
        console.log(`\n[screen ${gi}] "${scrName}" ${fw}x${fh} 状态=${states.length} (${states.map(s => `"${nodes.get(s).name}"`).join(",")})`);

        // 顶层项（可见、跳过全幅 Mask；z=字节序）
        const itemsByState = states.map(sk => {
            const items = [];
            (kids.get(sk) || []).forEach((c, z) => {
                const n = nodes.get(c);
                if (n.visible === false) return;
                const t = n.transform || {};
                const w = (n.size || {}).x || 0, h = (n.size || {}).y || 0;
                if (n.name === "Mask" && w >= fw - 1 && h >= fh - 1) return;
                items.push({ key: c, node: n, name: n.name || typeStr(n) + z, type: typeStr(n), x: t.m02 || 0, y: t.m12 || 0, w, h, z });
            });
            return items;
        });

        // 跨状态按名匹配
        const itemNames = [];
        const seenNames = new Set();
        for (const items of itemsByState) for (const it of items) if (!seenNames.has(it.name)) { seenNames.add(it.name); itemNames.push(it.name); }
        const matched = itemNames.map(nm => {
            const perState = new Map();
            itemsByState.forEach((items, si) => {
                const it = items.find(i => i.name === nm);
                if (it) perState.set(si, it);
            });
            let animated = false;
            if (perState.size === states.length) {
                const g0 = perState.get(0);
                for (const [, it] of perState) {
                    if (Math.round(it.x) !== Math.round(g0.x) || Math.round(it.y) !== Math.round(g0.y) ||
                        Math.round(it.w) !== Math.round(g0.w) || Math.round(it.h) !== Math.round(g0.h)) { animated = true; break; }
                }
            }
            return { name: nm, perState, animated };
        });

        // 动画项变量 + 状态变量（--no-flow 时跳过——交互走生成的 C 文件）
        const itemVars = new Map();
        if (hasStates && !args.noFlow) {
            for (const m of matched) {
                if (!m.animated) continue;
                const g0 = m.perState.get(0);
                const base = sanitize(m.name, "item");
                itemVars.set(m.name, {
                    x: addVar(base + "_x", Math.round(g0.x)),
                    y: addVar(base + "_y", Math.round(g0.y)),
                    w: addVar(base + "_w", Math.round(g0.w)),
                    h: addVar(base + "_h", Math.round(g0.h))
                });
            }
        }
        const stateVar = (!args.noFlow && hasStates) ? addVar(scrName.toLowerCase() + "_state", 1) : null;

        // ghost 填充组检测（静态项子树内，同名路径跨状态填充差）
        const ghostDefs = [];
        if (hasStates) {
            const pathFillLeaves = (rootKey) => {
                const out = [];
                (function walk(k, path, ox, oy) {
                    const n = nodes.get(k);
                    if (n.visible === false) return;
                    const t = n.transform || {};
                    const x = (t.m02 || 0) + ox, y = (t.m12 || 0) + oy;
                    const ch = (kids.get(k) || []).filter(c => nodes.get(c).visible !== false);
                    const fill = solidFill(n);
                    if (ch.length) {
                        for (const c of ch) walk(c, path + "/" + (nodes.get(c).name || "?"), x, y);
                    } else if (fill && ["VECTOR", "ELLIPSE", "RECTANGLE", "ROUNDED_RECTANGLE"].includes(typeStr(n))) {
                        out.push({ key: k, path, fill, x, y, w: (n.size || {}).x || 4, h: (n.size || {}).y || 4, node: n });
                    }
                })(rootKey, "", 0, 0);
                return out;
            };
            for (const m of matched) {
                if (m.animated) continue;
                const leaves0 = pathFillLeaves(m.perState.get(0).key);
                for (const lf of leaves0) {
                    const perStateFill = [];
                    for (let si = 0; si < states.length; si++) {
                        const it = m.perState.get(si);
                        const hit = it ? pathFillLeaves(it.key).find(l => l.path === lf.path) : null;
                        perStateFill.push(hit ? hit.fill : null);
                    }
                    const uniq = [...new Set(perStateFill.filter(Boolean))];
                    if (uniq.length <= 1) continue;
                    ghostDefs.push({ item: m, leaf0: lf, perStateFill });
                    console.log(`  ghost 填充组: ${m.name}${lf.path} → ${uniq.join(" / ")}`);
                }
            }
        }
        const ghostSkipKeys = new Set(ghostDefs.map(g => g.leaf0.key));

        // ---- 部件发射（state-0 内容） ----
        const screenChildren = [];
        const widgetOfItem = new Map();
        for (const it of itemsByState[0]) {
            let w = null;
            if (it.type === "INSTANCE") {
                const ex = expandInstance(it.node);
                w = panelWidget(uniqId(sanitize(it.name, "inst")), it.x, it.y, it.w, it.h, ex ? ex.panelStyle : { bg_opa: 0 });
                emitInstanceChildren(it.node, w, w.identifier);
            } else {
                w = emitStaticNode(it.key, M3.ident(), screenChildren, ghostSkipKeys);
                if (w) { widgetOfItem.set(it.name, w); continue; }
            }
            if (w) { screenChildren.push(w); widgetOfItem.set(it.name, w); }
        }

        // ghost panels（每状态一块，state0 可见其余 HIDDEN；顶层叠放）
        const ghosts = ghostDefs.map(gd => {
            const lf = gd.leaf0;
            // 符号矢量（箭头等）→ label ghost：形状用 MSR 符号，每状态一块按色切换
            const symName = vectorSymbol(lf.node, lf.w, lf.h) || siblingMetricSymbol(lf.node, kids, nodes);
            const symCh = symName && codepoints[symName] ? String.fromCodePoint(codepoints[symName]) : null;
            if (symCh) {
                const fsz = Math.max(8, Math.min(16, Math.round(Math.max(lf.w, lf.h))));
                const fname = "iconsym_" + fsz;
                if (!fontUse.has(fname)) fontUse.set(fname, new Set());
                fontUse.get(fname).add(symCh);
                const iw2 = Math.max(fsz + 2, 12), ih2 = Math.max(fsz + 2, 12);
                const panels = gd.perStateFill.map((fill, si) => {
                    const lbl = labelWidget(uniqId(`ghost_${sanitize(gd.item.name + lf.path, "g")}_${si}`),
                        lf.x - (iw2 - lf.w) / 2, lf.y - (ih2 - lf.h) / 2, iw2, ih2, symCh,
                        { text_color: fill || "#191919", text_font: fname });
                    if (si > 0) lbl.widgetFlags = (lbl.widgetFlags ? lbl.widgetFlags + "|" : "") + "HIDDEN";
                    screenChildren.push(lbl);
                    return lbl;
                });
                return { panels, name: gd.item.name + lf.path };
            }
            const nearSquare = Math.abs(lf.w - lf.h) <= Math.max(lf.w, lf.h) * 0.15;
            const radius = nearSquare ? Math.round(Math.min(lf.w, lf.h) / 2) : 0;
            const panels = gd.perStateFill.map((fill, si) => {
                const p = panelWidget(uniqId(`ghost_${sanitize(gd.item.name + lf.path, "g")}_${si}`),
                    lf.x, lf.y, Math.max(lf.w, 4), Math.max(lf.h, 4),
                    Object.assign({}, fill ? { bg_color: fill } : { bg_opa: 0 }, radius ? { radius } : {}));
                if (si > 0) p.widgetFlags = (p.widgetFlags ? p.widgetFlags + "|" : "") + "HIDDEN";
                screenChildren.push(p);
                return p;
            });
            return { panels, name: gd.item.name + lf.path };
        });

        // ---- 组件与连线 ----
        const components = [];
        const connectionLines = [];
        // 页面 meta 必须为 0,0,displayW,displayH —— Studio 运行视图的
        // putImageData 直接拿页面 left/top/width/height 当 blit 几何（脏矩形），
        // 摆位坐标会让整个画面永远刷不出来
        const page = {
            objID: oid(), name: scrName,
            left: 0, top: 0, width: fw, height: fh,
            createAtStart: true, deleteOnScreenUnload: false, isUsedAsUserWidget: false,
            localVariables: [], componentGroups: [], userProperties: [],
            connectionLines, components: []
        };
        let compTop = 60;
        const placeComp = c => { c.left = 460; c.top = compTop; compTop += 100; };

        const chainByEdge = new Map();
        function guardComp(B) {
            const c = {
                objID: oid(), type: "CompareActionComponent",
                left: 0, top: 0, width: 150, height: 70,
                customInputs: [], customOutputs: [],
                A: stateVar, B: String(B), operator: "="
            };
            placeComp(c);
            components.push(c);
            return c;
        }

        function buildChain(srcSi, dstSi, duration) {
            const key = `${srcSi}>${dstSi}`;
            if (chainByEdge.has(key)) return chainByEdge.get(key);
            const comp = {
                objID: oid(), type: "LVGLActionComponent",
                left: 0, top: 0, width: 190, height: 70,
                customInputs: [], customOutputs: [], actions: []
            };
            placeComp(comp);
            const setv = {
                objID: oid(), type: "SetVariableActionComponent",
                left: 0, top: 0, width: 190, height: 54,
                customInputs: [], customOutputs: [], entries: []
            };
            placeComp(setv);
            // 1) z 重排：目标 z 序逐项 moveForeground
            for (const it of itemsByState[dstSi]) {
                if (!widgetOfItem.has(it.name) || !itemVars.has(it.name)) continue;
                comp.actions.push({
                    objID: oid(), action: "objMoveForeground",
                    object: widgetOfItem.get(it.name).identifier, objectType: "literal"
                });
            }
            // 2) ghost 切换
            for (const g of ghosts) {
                comp.actions.push({
                    objID: oid(), action: "objSetFlagHidden",
                    object: g.panels[srcSi].identifier, objectType: "literal",
                    hidden: true, hiddenType: "literal"
                });
                comp.actions.push({
                    objID: oid(), action: "objSetFlagHidden",
                    object: g.panels[dstSi].identifier, objectType: "literal",
                    hidden: false, hiddenType: "literal"
                });
            }
            // 3) 几何动画（start=跟踪变量，end=目标）
            for (const it of itemsByState[dstSi]) {
                const vars = itemVars.get(it.name);
                const wgt = widgetOfItem.get(it.name);
                if (!vars || !wgt) continue;
                const mk = action => ({
                    objID: oid(), action,
                    object: wgt.identifier, objectType: "literal",
                    startType: "expression", endType: "literal",
                    delay: 0, delayType: "literal", time: duration, timeType: "literal",
                    relative: false, relativeType: "literal",
                    instant: false, instantType: "literal",
                    path: "EASE_IN_OUT", pathType: "literal",
                    repeatCount: 0, repeatCountType: "literal", playback: false, playbackType: "literal"
                });
                const ax = mk("animX"); ax.start = vars.x; ax.end = Math.round(it.x);
                const ay = mk("animY"); ay.start = vars.y; ay.end = Math.round(it.y);
                const aw = mk("animWidth"); aw.start = vars.w; aw.end = Math.round(it.w);
                const ah = mk("animHeight"); ah.start = vars.h; ah.end = Math.round(it.h);
                comp.actions.push(ax, ay, aw, ah);
                setv.entries.push({ objID: oid(), variable: vars.x, value: String(Math.round(it.x)) });
                setv.entries.push({ objID: oid(), variable: vars.y, value: String(Math.round(it.y)) });
                setv.entries.push({ objID: oid(), variable: vars.w, value: String(Math.round(it.w)) });
                setv.entries.push({ objID: oid(), variable: vars.h, value: String(Math.round(it.h)) });
            }
            setv.entries.push({ objID: oid(), variable: stateVar, value: String(dstSi + 1) });
            components.push(comp, setv);
            connectionLines.push({ objID: oid(), source: comp.objID, output: "@seqout", target: setv.objID, input: "@seqin" });
            chainByEdge.set(key, comp);
            console.log(`  链 ${srcSi + 1}->${dstSi + 1}: ${comp.actions.length} 动作`);
            return comp;
        }

        // SCREEN_LOADED 复位链（多状态屏回到状态 1；no-flow 由 C 侧 ghosts_show(1) 初始化替代）
        let screenLoadedComp = null;
        if (!args.noFlow && hasStates) {
            const comp = {
                objID: oid(), type: "LVGLActionComponent",
                left: 0, top: 0, width: 190, height: 70,
                customInputs: [], customOutputs: [], actions: []
            };
            placeComp(comp);
            const setv = {
                objID: oid(), type: "SetVariableActionComponent",
                left: 0, top: 0, width: 190, height: 54,
                customInputs: [], customOutputs: [], entries: []
            };
            placeComp(setv);
            for (const it of itemsByState[0]) {
                const vars = itemVars.get(it.name);
                const wgt = widgetOfItem.get(it.name);
                if (!vars || !wgt) continue;
                comp.actions.push({ objID: oid(), action: "objSetX", object: wgt.identifier, objectType: "literal", x: Math.round(it.x), xType: "literal" });
                comp.actions.push({ objID: oid(), action: "objSetY", object: wgt.identifier, objectType: "literal", y: Math.round(it.y), yType: "literal" });
                comp.actions.push({ objID: oid(), action: "objSetWidth", object: wgt.identifier, objectType: "literal", width: Math.round(it.w), widthType: "literal" });
                comp.actions.push({ objID: oid(), action: "objSetHeight", object: wgt.identifier, objectType: "literal", height: Math.round(it.h), heightType: "literal" });
                setv.entries.push({ objID: oid(), variable: vars.x, value: String(Math.round(it.x)) });
                setv.entries.push({ objID: oid(), variable: vars.y, value: String(Math.round(it.y)) });
                setv.entries.push({ objID: oid(), variable: vars.w, value: String(Math.round(it.w)) });
                setv.entries.push({ objID: oid(), variable: vars.h, value: String(Math.round(it.h)) });
            }
            for (const g of ghosts) {
                g.panels.forEach((p, si) => comp.actions.push({
                    objID: oid(), action: "objSetFlagHidden",
                    object: p.identifier, objectType: "literal",
                    hidden: si !== 0, hiddenType: "literal"
                }));
            }
            setv.entries.push({ objID: oid(), variable: stateVar, value: "1" });
            components.push(comp, setv);
            connectionLines.push({ objID: oid(), source: comp.objID, output: "@seqout", target: setv.objID, input: "@seqin" });
            screenLoadedComp = comp;
        }

        // changeScreen 组件（PUSH 边）
        function pushComp(e) {
            const fade = PUSH_TO_FADE[e.nav] || "FADE_IN";
            const comp = {
                objID: oid(), type: "LVGLActionComponent",
                left: 0, top: 0, width: 190, height: 54,
                customInputs: [], customOutputs: [],
                actions: [{
                    objID: oid(), action: "changeScreen",
                    screen: sanitize(nodes.get(e.targetFrame).name, "scr"), screenType: "literal",
                    fadeMode: fade, fadeModeType: "literal",
                    speed: e.duration, speedType: "literal",
                    delay: 0, delayType: "literal",
                    useStack: false, useStackType: "literal" // wasm 解释器仅接通 replacePageHook 通路
                }]
            };
            placeComp(comp);
            components.push(comp);
            console.log(`  PUSH: --${e.nav}--> "${nodes.get(e.targetFrame).name}" (${fade})`);
            return comp;
        }

        const screenWidget = panelWidget("screen", 0, 0, fw, fh, { bg_color: solidFill(nodes.get(states[0])) || "#000000" }, "CLICKABLE"); // 无 GESTURE_BUBBLE——LVGL 中该旗标=手势跳过此对象, screen 须亲自接收
        screenWidget.type = "LVGLScreenWidget";
        screenWidget.children = screenChildren;
        const scrHandlers = [];
        if (!args.noFlow && screenLoadedComp) scrHandlers.push({ objID: oid(), eventName: "SCREEN_LOADED", handlerType: "flow", userData: 0 });

        // 交互边归类：PUSH 一律屏级（Figma 的 DRAG 挂 Mask/Button/Timer 等载体上，
        // 真机语义=全屏手势；对象级收窄反致"只能底部滑/右滑无响应"）；
        // SMART 轮盘边挂卡（项级）
        const itemEdges = new Map();
        const screenEdges = [];
        for (const e of [...keptStateEdges, ...pushEdges]) {
            if (screenIndexOfFrame.get(e.srcFrame) !== gi) continue;
            if (e.push && e.srcFrame !== e.targetFrame) {
                screenEdges.push(e);
            } else {
                let srcName = null;
                for (const items of itemsByState) {
                    const it = items.find(i => i.key === e.srcKey);
                    if (it) { srcName = it.name; break; }
                }
                if (srcName && srcName !== "Mask") {
                    if (!itemEdges.has(srcName)) itemEdges.set(srcName, []);
                    itemEdges.get(srcName).push(e);
                } else {
                    screenEdges.push(e);
                }
            }
        }

        // 交互路由数据（flow 模式挂线 / no-flow 模式生成 C 共用）
        interactionData.push({ gi, scrName, states, hasStates, itemsByState, matched, ghosts, widgetOfItem, itemEdges, screenEdges });

        if (!args.noFlow) {
            // a) 项级交互（轮盘卡 CLICKED+GESTURE；守卫=源状态；PUSH 直连）
            for (const [nm, edges] of itemEdges) {
                const wgt = widgetOfItem.get(nm);
                if (!wgt) continue;
                wgt.widgetFlags = HIT_FLAGS;
                for (const e of edges) {
                    const srcSi = states.indexOf(e.srcFrame);
                    const dstSi = states.indexOf(e.targetFrame);
                    const shared = (dstSi < 0 || !hasStates) ? pushComp(e) : null;
                for (const evName of (e.trigger === "DRAG" ? ["GESTURE"] : ["CLICKED"])) {
                        wgt.eventHandlers.push({ objID: oid(), eventName: evName, handlerType: "flow", userData: 0 });
                        if (shared) {
                            connectionLines.push({ objID: oid(), source: wgt.objID, output: evName, target: shared.objID, input: "@seqin" });
                        } else {
                            const guard = guardComp(srcSi + 1);
                            connectionLines.push({ objID: oid(), source: wgt.objID, output: evName, target: guard.objID, input: "@seqin" });
                            const chain = buildChain(srcSi, dstSi, e.duration);
                            connectionLines.push({ objID: oid(), source: guard.objID, output: "True", target: chain.objID, input: "@seqin" });
                        }
                    }
                }
            }

            // b) 屏级交互（Mask/帧级拖拽：屏空白区点击/滑动；多状态屏加状态守卫）
            for (const e of screenEdges) {
                const srcSi = states.indexOf(e.srcFrame);
                const dstSi = states.indexOf(e.targetFrame);
                let entryComp = null;
                if (dstSi >= 0 && hasStates) {
                    const guard = guardComp(srcSi + 1);
                    const chain = buildChain(srcSi, dstSi, e.duration);
                    connectionLines.push({ objID: oid(), source: guard.objID, output: "True", target: chain.objID, input: "@seqin" });
                    entryComp = guard;
                } else {
                    entryComp = pushComp(e);
                }
                for (const evName of (e.trigger === "DRAG" ? ["GESTURE"] : ["CLICKED"])) {
                    scrHandlers.push({ objID: oid(), eventName: evName, handlerType: "flow", userData: 0 });
                    connectionLines.push({ objID: oid(), source: screenWidget.objID, output: evName, target: entryComp.objID, input: "@seqin" });
                }
            }
        } else {
            // no-flow：交互部件保持 CLICKABLE（C 侧手势命中），不挂任何 handler
            for (const nm of itemEdges.keys()) {
                const wgt = widgetOfItem.get(nm);
                if (wgt) wgt.widgetFlags = HIT_FLAGS;
            }
        }

        screenWidget.eventHandlers = scrHandlers;
        page.components.push(screenWidget, ...(args.noFlow ? [] : components));
        pagesOut.push(page);
    });

    const proj = newProject(args, { x: 0, y: 0 }, []);
    proj.bitmaps.push(...bitmapReg.bitmaps);
    proj.userPages = pagesOut;
    proj.variables.globalVariables = globalVars;
    if (args.noFlow) {
        // 注意：flowSupport 必须保持 true——F5 Run 预览（wasm 解释器）只认
        // flow 形态资产，false 会让 Run 黑屏。no-flow 的含义只是"不生成
        // flow 交互组件"（0 组件 0 连线），画面照常进 Run，交互在 C 文件
        // 里由 F7 模拟器/固件编译执行。
    }
    proj.settings.general.displayWidth = pageBox.x;
    proj.settings.general.displayHeight = pageBox.y;
    for (const p of proj.userPages) { p.width = pageBox.x; p.height = pageBox.y; }

    applyFontsFrom(proj, fontUse, args);

    const out = args.out || args._[0].replace(/\.fig$/i, "") + ".eez-project";
    fs.mkdirSync(path.dirname(path.resolve(out)), { recursive: true });
    fs.writeFileSync(out, JSON.stringify(proj, null, 2), "utf8");
    const totalComps = proj.userPages.reduce((a, p) => a + p.components.length - 1, 0);
    const totalLines = proj.userPages.reduce((a, p) => a + p.connectionLines.length, 0);
    console.log(`\n产出: ${out}`);
    console.log(`屏 ${proj.userPages.length} (${proj.userPages.map(p => p.name).join(", ")}) | 变量 ${globalVars.length} | 组件 ${totalComps} | 连线 ${totalLines}`);

    // ---- no-flow：生成交互 C 文件（原生 LVGL，无 flow 引擎依赖） ----
    if (args.noFlow) {
        const stem = path.basename(out, path.extname(out)).replace(/[^\w]/g, "_").toLowerCase() || "ui";
        const guard = stem.toUpperCase() + "_INTERACTIONS_H";
        const scrNameOf = frameKey => sanitize(nodes.get(frameKey).name, "scr");
        const PUSH_TO_DIR = { PUSH_FROM_RIGHT: "LV_DIR_LEFT", PUSH_FROM_LEFT: "LV_DIR_RIGHT", PUSH_FROM_TOP: "LV_DIR_BOTTOM", PUSH_FROM_BOTTOM: "LV_DIR_TOP" };
        const PUSH_TO_ANIM = { PUSH_FROM_RIGHT: "LV_SCR_LOAD_ANIM_MOVE_LEFT", PUSH_FROM_LEFT: "LV_SCR_LOAD_ANIM_MOVE_RIGHT", PUSH_FROM_TOP: "LV_SCR_LOAD_ANIM_MOVE_BOTTOM", PUSH_FROM_BOTTOM: "LV_SCR_LOAD_ANIM_MOVE_TOP" };

        const L = [];
        L.push(`// ${stem}_interactions.c — figma2eez 生成（交互不走 flow，原生 LVGL）`);
        L.push(`// 手势路由 + lv_anim 状态链（EASE_IN_OUT ${230}ms）+ lv_screen_load_anim 转场`);
        L.push(`#include "lvgl.h"`);
        L.push(`#include <stdio.h>`);
        L.push(`#include "screens.h"`);
        L.push(``);
        L.push(`extern objects_t objects;`);
        L.push(``);
        L.push(`static void wq_set_x(void *o, int32_t v) { lv_obj_set_x((lv_obj_t *)o, v); }`);
        L.push(`static void wq_set_y(void *o, int32_t v) { lv_obj_set_y((lv_obj_t *)o, v); }`);
        L.push(`static void wq_set_w(void *o, int32_t v) { lv_obj_set_width((lv_obj_t *)o, v); }`);
        L.push(`static void wq_set_h(void *o, int32_t v) { lv_obj_set_height((lv_obj_t *)o, v); }`);
        L.push(`static void wq_set_scale(void *o, int32_t v) { lv_obj_set_style_transform_scale_x((lv_obj_t *)o, v, LV_PART_MAIN | LV_STATE_DEFAULT); lv_obj_set_style_transform_scale_y((lv_obj_t *)o, v, LV_PART_MAIN | LV_STATE_DEFAULT); }`);
        L.push(``);
        L.push(`#define WQ_ANIM(obj, fn, cur, end) do { \\`);
        L.push(`    lv_anim_del((obj), (fn)); /* 清旧动画：连滑中断时避免并存竞态/起点错读 */ \\`);
        L.push(`    lv_anim_t a_; lv_anim_init(&a_); \\`);
        L.push(`    lv_anim_set_var(&a_, (obj)); \\`);
        L.push(`    lv_anim_set_exec_cb(&a_, (fn)); \\`);
        L.push(`    lv_anim_set_values(&a_, (cur), (end)); \\`);
        L.push(`    lv_anim_set_duration(&a_, 230); \\`);
        L.push(`    lv_anim_set_path_cb(&a_, lv_anim_path_ease_in_out); \\`);
        L.push(`    lv_anim_start(&a_); \\`);
        L.push(`} while (0)`);
        L.push(`#define WQ_TO_X(o, e) WQ_ANIM((o), wq_set_x, lv_obj_get_x_aligned(o), (e))`);
        L.push(`#define WQ_TO_Y(o, e) WQ_ANIM((o), wq_set_y, lv_obj_get_y_aligned(o), (e))`);
        L.push(`#define WQ_TO_SCALE(o, e) WQ_ANIM((o), wq_set_scale, lv_obj_get_style_transform_scale_x(o, LV_PART_MAIN), (e))`);
        L.push(`#define WQ_TO_H(o, e) WQ_ANIM((o), wq_set_h, lv_obj_get_height(o), (e))`);
        L.push(``);

        const initLines = [];
        for (const d of interactionData) {
            const fn = d.scrName;
            const itemWidgets = [...d.widgetOfItem.entries()].filter(([, w]) => w.identifier);
            const objRef = id => `objects.${String(id).toLowerCase()}`;

            if (d.hasStates) {
                L.push(`// ==== ${fn}：${d.states.length} 状态轮盘 ====`);
                L.push(`static int s_${fn}_state = 1;`);
                L.push(``);
                // ghost 显隐
                if (d.ghosts.length) {
                    L.push(`static void ${fn}_ghosts_show(int st) {`);
                    for (const g of d.ghosts) {
                        for (const p of g.panels) L.push(`    lv_obj_add_flag(${objRef(p.identifier)}, LV_OBJ_FLAG_HIDDEN);`);
                    }
                    L.push(`    switch (st) {`);
                    d.states.forEach((sk, si) => {
                        const shows = d.ghosts.map(g => g.panels[si]).filter(Boolean);
                        if (!shows.length) return;
                        L.push(`    case ${si + 1}:`);
                        for (const p of shows) L.push(`        lv_obj_clear_flag(${objRef(p.identifier)}, LV_OBJ_FLAG_HIDDEN);`);
                        L.push(`        break;`);
                    });
                    L.push(`    }`);
                    L.push(`}`);
                    L.push(``);
                }
                // 状态链
                // 诊断回调：收敛后打印几何（生成于 go 尾部引用）
                const diagVars = d.states[0] ? d.itemsByState[0].filter(it => {
                    const m = d.matched.find(mm => mm.name === it.name);
                    return m && m.animated && d.widgetOfItem.get(it.name);
                }) : [];
                if (diagVars.length) {
                    L.push(`static int s_${fn}_diag_state = 1;`);
                    L.push(`static lv_timer_t *s_${fn}_diag_timer = 0;`);
                    L.push(`static void ${fn}_diag_cb(lv_timer_t *t) {`);
                    L.push(`    (void)t;`);
                    L.push(`    s_${fn}_diag_timer = 0; /* repeat=1 由 LVGL 自删，防悬空 double-free */`);
                    L.push(`    printf("[aw]${fn} settled st=%d", s_${fn}_diag_state);`);
                    for (const it of diagVars) {
                        const w = d.widgetOfItem.get(it.name);
                        L.push(`; printf(" ${it.name}=(%d,%d,%d,%d)", lv_obj_get_x(${objRef(w.identifier)}), lv_obj_get_y(${objRef(w.identifier)}), lv_obj_get_width(${objRef(w.identifier)}), lv_obj_get_height(${objRef(w.identifier)}))`);
                        // 首个文本 label 坐标（label 层漂移检测）
                        const lbl = (w.children || []).find(c => c.type === "LVGLLabelWidget");
                        if (lbl) {
                            L.push(`; printf(" ${it.name}T=(%d,%d)", lv_obj_get_x(${objRef(lbl.identifier)}), lv_obj_get_y(${objRef(lbl.identifier)}))`);
                        }
                    }
                    L.push(`; printf("\\n");`);
                    L.push(`}`);
                    L.push(``);
                }
                L.push(`static void ${fn}_go(int st) {`);
                L.push(`    printf("[aw]${fn} go st=%d\\n", st);`);
                L.push(`    s_${fn}_state = st;`);
                L.push(`    switch (st) {`);
                d.states.forEach((sk, si) => {
                    L.push(`    case ${si + 1}: {`);
                    // z 重排（动画项按目标 z 序）
                    for (const it of d.itemsByState[si]) {
                        const m = d.matched.find(mm => mm.name === it.name);
                        if (!m || !m.animated) continue;
                        const w = d.widgetOfItem.get(it.name);
                        if (w) L.push(`        lv_obj_move_foreground(${objRef(w.identifier)});`);
                    }
                    // 几何动画：x/y 平移 + 整体 scale 缩放（transform_scale, pivot=左上）。
                    // 不用 set_width/height：那只缩卡面，内容不随动——动画中间帧内容
                    // 相对卡面错位（Figma Smart Animate 语义=卡与内容一起缩放）
                    for (const it of d.itemsByState[si]) {
                        const m = d.matched.find(mm => mm.name === it.name);
                        if (!m || !m.animated) continue;
                        const w = d.widgetOfItem.get(it.name);
                        if (!w) continue;
                        const g0 = m.perState.get(0);
                        L.push(`        WQ_TO_X(${objRef(w.identifier)}, ${Math.round(it.x)});`);
                        L.push(`        WQ_TO_Y(${objRef(w.identifier)}, ${Math.round(it.y)});`);
                        L.push(`        WQ_TO_SCALE(${objRef(w.identifier)}, ${Math.round(256 * it.w / (g0.w || it.w))});`);
                    }
                    L.push(`        break;`);
                    L.push(`    }`);
                });
                L.push(`    }`);
                if (d.ghosts.length) L.push(`    ${fn}_ghosts_show(st);`);
                // 诊断：动画收敛后打印各动画项实际几何（300ms 一次性 timer）
                const diagItems = d.states[0] ? d.itemsByState[0].filter(it => {
                    const m = d.matched.find(mm => mm.name === it.name);
                    return m && m.animated && d.widgetOfItem.get(it.name);
                }) : [];
                if (diagItems.length) {
                    L.push(`    {`);
                    L.push(`        s_${fn}_diag_state = st;`);
                    L.push(`        if (s_${fn}_diag_timer) lv_timer_del(s_${fn}_diag_timer);`);
                    L.push(`        s_${fn}_diag_timer = lv_timer_create(${fn}_diag_cb, 300, 0);`);
                    L.push(`        lv_timer_set_repeat_count(s_${fn}_diag_timer, 1);`);
                    L.push(`    }`);
                }
                L.push(`}`);
                L.push(``);
                // 部件手势路由（卡轮盘 + 部件 PUSH）
                const hasItemGesture = [...d.itemEdges.values()].some(edges => edges.length);
                if (hasItemGesture) {
                    L.push(`static void ${fn}_items_gesture_cb(lv_event_t *e) {`);
                    L.push(`    lv_obj_t *obj = lv_event_get_target(e);`);
                    for (const [nm, edges] of d.itemEdges) {
                        const w = d.widgetOfItem.get(nm);
                        if (!w) continue;
                        L.push(`    if (obj == ${objRef(w.identifier)}) {`);
                        const seen = new Set();
                        for (const ed of edges) {
                            const srcSi = d.states.indexOf(ed.srcFrame);
                            const dstSi = d.states.indexOf(ed.targetFrame);
                            if (dstSi >= 0 && d.hasStates) {
                                const key = `${srcSi}>${dstSi}`;
                                if (seen.has(key)) continue;
                                seen.add(key);
                                L.push(`        if (s_${fn}_state == ${srcSi + 1}) { ${fn}_go(${dstSi + 1}); return; }`);
                            } else if (ed.push && ed.srcFrame !== ed.targetFrame) {
                                const dir = PUSH_TO_DIR[ed.nav] || "LV_DIR_LEFT";
                                const anim = PUSH_TO_ANIM[ed.nav] || "LV_SCR_LOAD_ANIM_FADE_IN";
                                L.push(`        if (lv_indev_get_gesture_dir(lv_indev_active()) == ${dir}) { lv_screen_load_anim(${objRef(scrNameOf(ed.targetFrame))}, ${anim}, ${ed.duration}, 0, false); return; }`);
                            }
                        }
                        L.push(`        return;`);
                        L.push(`    }`);
                    }
                    L.push(`}`);
                    L.push(``);
                    for (const [nm] of d.itemEdges) {
                        const w = d.widgetOfItem.get(nm);
                        if (w) initLines.push(`    lv_obj_add_event_cb(${objRef(w.identifier)}, ${fn}_items_gesture_cb, LV_EVENT_GESTURE, NULL);`);
                    }
                }
            } else {
                // 单状态屏：PUSH 边已全部并入屏级手势（对象级不再生成）
            }
            // 屏级手势（全屏任意位置；Figma DRAG 无 dragDirection=任意方向，
            // 单目标方向集时任意拖触发，双向屏（middle）按方向消歧）
            const scrPush = d.screenEdges.filter(ed => ed.push && ed.srcFrame !== ed.targetFrame);
            if (scrPush.length) {
                const byDir = new Map(); // dir → 第一条边
                for (const ed of scrPush) {
                    const dir = PUSH_TO_DIR[ed.nav] || "LV_DIR_LEFT";
                    if (!byDir.has(dir)) byDir.set(dir, ed);
                }
                L.push(`static void ${fn}_screen_gesture_cb(lv_event_t *e) {`);
                if (byDir.size === 1) {
                    const [dir, ed] = [...byDir.entries()][0];
                    const anim = PUSH_TO_ANIM[ed.nav] || "LV_SCR_LOAD_ANIM_FADE_IN";
                    L.push(`    (void)e; (void)lv_indev_get_gesture_dir(lv_indev_active());`);
                    L.push(`    lv_screen_load_anim(${objRef(scrNameOf(ed.targetFrame))}, ${anim}, ${ed.duration}, 0, false);`);
                } else {
                    L.push(`    lv_dir_t d = lv_indev_get_gesture_dir(lv_indev_active());`);
                    L.push(`    (void)e;`);
                    for (const [dir, ed] of byDir.entries()) {
                        const anim = PUSH_TO_ANIM[ed.nav] || "LV_SCR_LOAD_ANIM_FADE_IN";
                        L.push(`    if (d == ${dir}) { lv_screen_load_anim(${objRef(scrNameOf(ed.targetFrame))}, ${anim}, ${ed.duration}, 0, false); return; }`);
                    }
                }
                L.push(`}`);
                L.push(``);
                initLines.push(`    lv_obj_add_event_cb(${objRef(fn)}, ${fn}_screen_gesture_cb, LV_EVENT_GESTURE, NULL);`);
            }
            if (d.hasStates && d.ghosts.length) {
                initLines.push(`    ${fn}_ghosts_show(1);`);
            }
            // scale 动画的卡：pivot 固定左上（x/y 语义与 Figma 帧坐标一致）
            if (d.hasStates) {
                for (const it of (d.itemsByState[0] || [])) {
                    const m = d.matched.find(mm => mm.name === it.name);
                    if (!m || !m.animated) continue;
                    const w = d.widgetOfItem.get(it.name);
                    if (!w) continue;
                    initLines.push(`    lv_obj_set_style_transform_pivot_x(${objRef(w.identifier)}, 0, LV_PART_MAIN | LV_STATE_DEFAULT);`);
                    initLines.push(`    lv_obj_set_style_transform_pivot_y(${objRef(w.identifier)}, 0, LV_PART_MAIN | LV_STATE_DEFAULT);`);
                }
            }
        }

        L.push(`void ${stem}_interactions_init(void) {`);
        L.push(`    printf("[aw] interactions init v2\\n");`);
        L.push(...initLines);
        L.push(`}`);
        L.push(``);

        const cPath = path.join(path.dirname(path.resolve(out)), stem + "_interactions.c");
        const hPath = path.join(path.dirname(path.resolve(out)), stem + "_interactions.h");
        fs.writeFileSync(cPath, L.join("\r\n"), "utf8");
        fs.writeFileSync(hPath, `#ifndef ${guard}\r\n#define ${guard}\r\n\r\n#ifdef __cplusplus\r\nextern "C" {\r\n#endif\r\n\r\nvoid ${stem}_interactions_init(void);\r\n\r\n#ifdef __cplusplus\r\n}\r\n#endif\r\n\r\n#endif // ${guard}\r\n`, "utf8");
        console.log(`交互 C: ${cPath}`);
        console.log(`        ${hPath}`);
    }
}

// ---------- 主流程 ----------
function main() {
    const args = parseArgs(process.argv);
    const { raw, nodes, order } = parseFig(args._[0]);
    const { kids } = buildTree({ nodes, order });
    const ctx = { raw, nodes, order, kids };
    const codepoints = loadCodepoints();

    const pages = order.map(k => nodes.get(k)).filter(n => typeStr(n) === "CANVAS");
    let page = null, pageKey = null;
    if (args.page) {
        pageKey = order.find(k => nodes.get(k) === pages.find(p => p.name === args.page));
        page = nodes.get(pageKey);
    } else {
        page = pages.find(p => (p.name || "") !== "Internal Only Canvas") || pages[0];
        pageKey = order.find(k => nodes.get(k) === page);
    }
    if (!page) { console.error("未找到画布页面" + (args.page ? ` "${args.page}"` : "")); process.exit(1); }

    // 引擎选择：组件集 → v1；否则多帧 → v2
    const frameChildren = (kids.get(pageKey) || []).filter(k => typeStr(nodes.get(k)) === "FRAME");
    const hasComponentSet = frameChildren.some(fk => {
        const ch = kids.get(fk) || [];
        return ch.length >= 2 && ch.every(c => ["SYMBOL", "COMPONENT"].includes(typeStr(nodes.get(c))));
    });
    const manyPlainFrames = frameChildren.filter(fk => typeStr(nodes.get(fk)) === "FRAME").length >= 2;

    if (hasComponentSet && !args.multiframe) {
        console.log(`页面 "${page.name}" → v1 组件集引擎`);
        runV1(args, pageKey, page, ctx, codepoints);
    } else if (manyPlainFrames) {
        console.log(`页面 "${page.name}" → v2 多帧状态组引擎（${args.noFlow === false ? "flow" : "no-flow 直出 C"}）`);
        if (args.noFlow !== false) args.noFlow = true; // 默认：交互不走 flow，直出原生 LVGL C
        runV2(args, pageKey, page, ctx, codepoints);
    } else {
        console.log(`页面 "${page.name}" → v1 单帧引擎`);
        runV1(args, pageKey, page, ctx, codepoints);
    }
}

main();
