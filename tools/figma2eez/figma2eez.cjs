#!/usr/bin/env node
/**
 * figma2eez — 通用 Figma .fig → EEZ Studio LVGL 工程转换器
 *
 * 用法:
 *   node figma2eez.cjs <input.fig> [-o out.eez-project] [--frame <名>]
 *                      [--fonts-from <模板工程>] [--variant <变体名>]
 *
 * 能力（v1）:
 *   - 解析 .fig（@open-pencil/fig，fig-kiwi v106）
 *   - 静态树: FRAME/RECT/ROUNDED_RECT/ELLIPSE → panel（填充/描边/圆角/阴影，
 *     绝对坐标拍平 auto-layout，容器 pad=0，圆角容器 clip_corner）
 *   - TEXT → label（字号/颜色/对齐；拉丁 → Montserrat 内置；CJK → 需
 *     --fonts-from 提供（或后续经桥 add_font 生成），否则回退默认字体）
 *   - 图标: lucide 命名的 VECTOR → Material Symbols 符号字体 label
 *     （可变色，随状态渐变）
 *   - 交互: prototypeInteractions 的 ON_CLICK → SWAP_STATE + SMART_ANIMATE
 *     通用 diff 引擎——两变体间同名节点的 x 差 → animSpring（Figma 弹簧
 *     参数 [mass,k,c,v0] 原样传递）；填充色差 → animTextColor SPRING
 *     （逐通道+同弹簧曲线）；AFTER_TIMEOUT → SCREEN_LOADED+Delay 自动链
 *   - 每个 x 动画节点配 <name>_x 变量 + Delay+SetVariable 跟踪（任意
 *     变体互切都从当前位置滑）
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
        else if (a === "--variant") args.variant = argv[++i];
        else if (a === "--fonts-from") args.fontsFrom = argv[++i];
        else if (a === "--icons") args.icons = argv[++i];
        else args._.push(a);
    }
    if (!args._[0]) {
        console.error("用法: node figma2eez.cjs <input.fig> [-o out] [--frame 名] [--fonts-from 模板] [--variant 名]");
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
        openPencil = require("@open-pencil/fig");
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
    return { kids, roots };
}

// ---------- 几何 / 样式 ----------
function absTransform(ctx, kids, key) {
    // 组合父链 transform → 绝对位置/缩放（拍平 auto-layout）
    let m00 = 1, m01 = 0, m02 = 0, m10 = 0, m11 = 1, m12 = 0;
    const chain = [];
    for (let k = key; k; ) {
        const n = ctx.nodes.get(k);
        if (!n) break;
        chain.push(n);
        const p = n.parentIndex && n.parentIndex.guid;
        k = p ? G(p) : null;
        if (k && !ctx.nodes.has(k)) k = null;
    }
    for (const n of chain.reverse()) {
        const t = n.transform || {};
        const a = t.m00 ?? 1, b = t.m01 ?? 0, tx = t.m02 ?? 0;
        const c = t.m10 ?? 0, d = t.m11 ?? 1, ty = t.m12 ?? 0;
        const n00 = a * m00 + b * m10, n01 = a * m01 + b * m11, n02 = a * m02 + b * m11 * 0 + tx + (a * m02 + b * m12) * 0;
        // 正确的 2D 仿射组合: [a b tx; c d ty] × M
        const r00 = a * m00 + b * m10;
        const r01 = a * m01 + b * m11;
        const r02 = a * m02 + b * m12 + tx;
        const r10 = c * m00 + d * m10;
        const r11 = c * m01 + d * m11;
        const r12 = c * m02 + d * m12 + ty;
        m00 = r00; m01 = r01; m02 = r02; m10 = r10; m11 = r11; m12 = r12;
    }
    return { x: m02, y: m12, sx: m00, sy: m11 };
}

function absBox(ctx, kids, key) {
    const n = ctx.nodes.get(key);
    const t = absTransform(ctx, kids, key);
    const s = n.size || { x: 0, y: 0 };
    return { x: t.x, y: t.y, w: s.x * Math.abs(t.sx || 1), h: s.y * Math.abs(t.sy || 1) };
}

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
const hexToInt = c => c ? parseInt(c.slice(1, 7), 16) : 0x191919;

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

function panelWidget(id, x, y, w, h, styleProps) {
    const d = Object.assign({ pad_left: 0, pad_right: 0, pad_top: 0, pad_bottom: 0, border_width: 0 }, styleProps);
    if (d.radius > 0) d.clip_corner = true;
    return makeWidget("LVGLPanelWidget", {
        identifier: id, left: Math.round(x), top: Math.round(y),
        width: Math.round(w), height: Math.round(h),
        localStyles: { objID: oid(), definition: { MAIN: { DEFAULT: d } } }
    });
}

function labelWidget(id, x, y, w, h, text, styleProps) {
    return makeWidget("LVGLLabelWidget", {
        identifier: id, left: Math.round(x), top: Math.round(y),
        width: Math.round(w), height: Math.round(h),
        text,
        localStyles: { objID: oid(), definition: { MAIN: { DEFAULT: Object.assign({ text_align: "CENTER" }, styleProps) } } }
    });
}

function imageWidget(id, x, y, w, h, bitmap) {
    return makeWidget("LVGLImageWidget", {
        identifier: id, left: Math.round(x), top: Math.round(y),
        width: Math.round(w), height: Math.round(h),
        widthUnit: "content", heightUnit: "content",
        image: bitmap,
        angle: 0, zoom: 256, pivotX: 0, pivotY: 0,
        value: 0, valueType: "literal", previewValue: 0, setPivot: false,
        widgetFlags: "ADV_HITTEST|CLICK_FOCUSABLE|GESTURE_BUBBLE|PRESS_LOCK|SCROLL_CHAIN_HOR|SCROLL_CHAIN_VER|SCROLL_ELASTIC|SCROLL_MOMENTUM|SCROLL_WITH_ARROW|SNAPPABLE"
    });
}

////////////////////////////////////////////////////////////////////////////////

// lucide 名 → Material Symbols 名/码位（可扩展；码位表在 figma2eez 同目录 codepoints）
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

// ---------- 主流程 ----------
function main() {
    const args = parseArgs(process.argv);
    const { raw, nodes, order } = parseFig(args._[0]);
    const { kids, roots } = buildTree({ nodes, order });
    const ctx = { nodes, order, kids };
    const codepoints = loadCodepoints();

    // 1) 找页面与目标 frame（CANVAS 直接从合并表取，不经 roots）
    const pages = order.map(k => nodes.get(k)).filter(n => typeStr(n) === "CANVAS");
    const page = pages.find(p => (p.name || "") !== "Internal Only Canvas") || pages[0];
    if (!page) { console.error("未找到画布页面"); process.exit(1); }
    const pageKey = order.find(k => nodes.get(k) === page);
    let frames = (kids.get(pageKey) || []).map(k => ({ key: k, node: nodes.get(k) })).filter(f => typeStr(f.node) === "FRAME");
    if (args.frame) frames = frames.filter(f => f.node.name === args.frame);
    if (!frames.length) { console.error("未找到 FRAME（--frame 指定名字？）"); process.exit(1); }

    // 2) 组件集（variant 集）识别：children 全为 SYMBOL/COMPONENT 的 frame
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

    // 3) 收集全部交互
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

    // 4) 变体几何提取（通用 diff 数据）
    // variantBBox(vKey) → Map(name → {x,y,w,h,fill,text,icon}) 变体内一级节点
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
        walk(vKey, 0, 0); // 变体内容相对变体原点（实例位置由 holder 承载）
        return m;
    }
    const variants = [];
    for (const cs of componentSets) {
        for (const v of cs.variants) {
            variants.push({ key: v, node: nodes.get(v), content: variantContent(v), set: cs });
        }
    }

    // 5) 选默认变体（--variant 或第一个）
    let defaultVariant = null;
    if (args.variant) defaultVariant = variants.find(v => v.node.name === args.variant);
    if (!defaultVariant && variants.length) {
        // 优先带交互起点语义的（其内节点出现在 interactions.src 里）
        defaultVariant = variants.find(v => [...v.content.values()].some(c => interactions.some(i => i.srcKey === c.key))) || variants[0];
    }

    // TEXT 项名 → fill 是否跨变体有差（供图标联动判断）
    const fillDiffNames = new Set();
    if (variants.length >= 2) {
        const names0 = new Set();
        for (const v of variants) for (const nm of v.content.keys()) names0.add(nm);
        for (const nm of names0) {
            const fills = variants.map(v => v.content.get(nm)).filter(Boolean).map(c => c.fill).filter(Boolean);
            if (new Set(fills).size > 1) fillDiffNames.add(nm);
        }
    }

    // 6) 静态树生成（从 rootFrame，组件实例位置用默认变体的内容替换）
    const fontUse = new Map(); // "size" -> Set(chars) for CJK
    const widgets = [];
    const idUsed = new Set();
    const uniqId = base => { let i = 1, id = base; while (idUsed.has(id)) id = base + i++; idUsed.add(id); return id; };

    function textFont(size, chars) {
        const hasCJK = /[\u4e00-\u9fff]/.test(chars || "");
        const sz = Math.max(8, Math.min(48, Math.round(size || 15)));
        if (hasCJK) {
            const key = "cjk" + sz;
            if (!fontUse.has(key)) fontUse.set(key, new Set());
            for (const ch of chars) fontUse.get(key).add(ch);
            return { font: key, cjk: true, size: sz };
        }
        // Montserrat 内置档位
        const steps = [8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48];
        const ms = steps.reduce((a, b) => Math.abs(b - sz) < Math.abs(a - sz) ? b : a);
        return { font: "MONTSERRAT_" + ms, cjk: false, size: ms };
    }

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
            const p = panelWidget(id, x, y, w, h, Object.assign({}, fill ? { bg_color: fill } : {}, radius ? { radius } : {}));
            out.push(p);
            emitChildren(k, x, y, p.children, hitCollector);
            hitCollector.push({ key: k, node: n, type: ty, widget: p, x, y, w, h });
            return p;
        }
        if (ty === "TEXT") {
            const chars = ((n.textData || {}).characters) || "";
            const fill = solidFill(n) || "#191919";
            const { font } = textFont(n.fontSize, chars);
            const id = uniqId("label");
            const lw = Math.max(w, 12), lh = Math.max(h, 14);
            const lbl = labelWidget(id, x, y + (h > lh ? 0 : 0), lw, lh, chars, { text_color: fill, text_font: font });
            out.push(lbl);
            hitCollector.push({ key: k, node: n, type: ty, widget: lbl, x, y, w, h });
            return lbl;
        }
        if (ty === "VECTOR" || ty === "LINE") {
            // lucide 图标名匹配 → 符号字体 label（可变色）
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
            // 回退：占位 panel
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

    // rootFrame 静态树；组件集实例（在 rootFrame 里 type INSTANCE/SYMBOL 且指向组件集）→ 用默认变体内容展开
    const hitAreas = [];
    const screenChildren = [];
    const widgetByItemName = new Map(); // 变体内容名 → 部件（链的目标查找）
    const iconFollowText = new Map();  // 符号 label 部件 → 跟随的 TEXT 项名
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
                // 组件实例 → 默认变体内容（一次性展开）
                const instFill = solidFill(n);
                const holder = panelWidget(uniqId("inst"), t.m02 || 0, t.m12 || 0,
                    (n.size || {}).x || 0, (n.size || {}).y || 0,
                    Object.assign({}, instFill ? { bg_color: instFill } : { bg_opa: 0 },
                        n.cornerRadius ? { radius: Math.round(n.cornerRadius) } : {}));
                const vt = nodes.get(defaultVariant.key).transform || {};
                for (const [name, cc] of defaultVariant.content) {
                    // 变体内容 → 部件（相对实例原点）
                    emitVariantItem(cc, holder, name, hitAreas);
                }
                out.push(holder);
            } else {
                emitNode(c, 0, 0, out, hitAreas);
            }
        }
    }

    function emitVariantItem(cc, holder, name, hitCollector) {
        // 变体项落在 holder（实例）内，坐标 = cc.x - vt.m02 …已含在 variantContent 相对变体原点
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
                // 记录跟随目标：同 x 列（同 tab 组）颜色有差的 TEXT 项
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
    const rt = rootFrame.node.transform || {};
    emitFrameContent(rootFrame.key, screenChildren);

    // 7) 交互链生成（通用 Smart Animate diff）
    const components = [];
    const connectionLines = [];
    const variables = [];
    const screenKey = rootFrame.k;

    // 7a) 每个 x-动画节点配变量（在变体间 x 有差的同名节点）
    const xVarByNodeName = new Map();
    const colorAnimNodes = new Set(); // name set: fill 在变体间有差的 TEXT
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

    // 部件按 identifier 索引（identifier 唯一）
    const widgetByName = new Map();
    const indexWidgets = w => { widgetByName.set(w.identifier, w); (w.children || []).forEach(indexWidgets); };
    screenChildren.forEach(indexWidgets);

    // 变体名 → 目标几何（用于链）
    // 变体内节点名 → 部件 identifier 映射（通过 hitAreas 顺序对齐太脆；用位置最近匹配）
    function matchWidget(v, itemName) {
        if (widgetByItemName.has(itemName)) return widgetByItemName.get(itemName);
        const item = v.content.get(itemName);
        if (!item) return null;
        // 在全部叶子部件中找位置/尺寸最接近的
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

    // 7b) goChain(variant)：动画到该变体的几何/颜色
    const chainByVariant = new Map();
    const linkVar = {};  // 跨链去重：变量只声明一次
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
            const inter = interactions.find(i => i.targetKey && variants.some(vv => vv.key === i.targetKey)) ||
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
            const inter = interactions.find(i => i.targetKey && variants.some(vv => vv.key === i.targetKey)) ||
                { duration: 500, k: 170, c: 15 };
            // 同组联动的符号图标（Figma 里选中组图标跟文字一起变色）
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
        // 延迟到动画结束再写变量（保护当前位置跟踪）
        const maxTime = Math.max(500, ...comp.actions.map(a => a.time || 0));
        const delayComp = {
            objID: oid(), type: "DelayActionComponent",
            left: comp.left, top: comp.top + 180, width: 190, height: 54,
            customInputs: [], customOutputs: [], milliseconds: String(maxTime)
        };
        components.push(comp, delayComp, setv);
        connectionLines.push({ objID: oid(), source: comp.objID, output: "@seqout", target: delayComp.objID, input: "@seqin" });
        connectionLines.push({ objID: oid(), source: delayComp.objID, output: "@seqout", target: setv.objID, input: "@seqin" });
        chainByVariant.set(v.key, comp);
        return comp;
    }

    // 7c) 交互源 → 命中区挂 CLICKED + 连线
    let screenHandlers = [];
    for (const it of interactions) {
        if (!it.targetKey || !it.smart) continue;
        const targetVariant = variants.find(v => v.key === it.targetKey);
        if (!targetVariant) continue;
        const chain = goChain(targetVariant);
        // 源 hit：key 直配；否则几何回退（源节点在其所属变体内求绝对位置，就近匹配）
        let hit = hitAreas.find(h => h.key === it.srcKey);
        if (!hit) {
            let x = 0, y = 0, found = false;
            for (let k = it.srcKey; k; ) {
                const n = nodes.get(k);
                if (!n) break;
                if (variants.some(v => v.key === k)) { found = true; break; }
                const t = n.transform || {};
                x += t.m02 || 0; y += t.m12 || 0;
                const p = n.parentIndex && n.parentIndex.guid;
                k = p ? G(p) : null;
            }
            if (found) {
                let best = null, bestD = 1e9;
                for (const h of hitAreas) {
                    const d = Math.abs(h.x - x) + Math.abs(h.y - y);
                    if (d < bestD) { bestD = d; best = h; }
                }
                if (best && bestD < 60) hit = best;
            }
        }
        if (hit) {
            const w = hit.widget;
            if (it.trigger === "AFTER_TIMEOUT" || it.trigger === "ON_LOAD") {
                // 页面加载自动链
                screenHandlers.push({ objID: oid(), eventName: "SCREEN_LOADED", handlerType: "flow", userData: 0 });
            } else {
                if (w.type === "LVGLPanelWidget") {
                    w.widgetFlags = "CLICKABLE";
                }
                w.eventHandlers.push({ objID: oid(), eventName: "CLICKED", handlerType: "flow", userData: 0 });
            }
            connectionLines.push({ objID: oid(), source: w.objID, output: "CLICKED", target: chain.objID, input: "@seqin" });
        } else {
            console.warn(`  [warn] 交互源未映射到部件: ${it.srcNode.name}`);
        }
    }
    // 去重 SCREEN_LOADED
    if (screenHandlers.length) screenHandlers = [screenHandlers[0]];

    // 8) 组装工程
    const proj = {
        themesVersion: 1,
        objID: oid(),
        settings: {
            objID: oid(),
            general: {
                objID: oid(), projectType: "lvgl", projectVersion: "v3",
                lvglVersion: "9.4.0", extensions: [], imports: [], flowSupport: true,
                displayWidth: Math.round(rootBox.x), displayHeight: Math.round(rootBox.y),
                description: `figma2eez import: ${path.basename(args._[0])}`
            },
            build: {
                objID: oid(),
                configurations: [{ objID: oid(), name: "Default" }],
                imageExportMode: "source"
            }
        },
        themes: [{ objID: oid(), name: "Default", colors: [] }],
        lvglStyles: { objID: oid(), styles: [] },
        fonts: [], bitmaps: [],
        variables: { objID: oid(), globalVariables: variables, structures: [], enums: [] },
        actions: [],
        userPages: [{
            objID: oid(), name: "Main",
            left: 0, top: 0, width: Math.round(rootBox.x), height: Math.round(rootBox.y),
            createAtStart: true, deleteOnScreenUnload: false, isUsedAsUserWidget: false,
            localVariables: [], componentGroups: [], userProperties: [],
            connectionLines,
            components: []
        }]
    };

    // screen 部件
    const screen = panelWidget("screen", 0, 0, rootBox.x, rootBox.y, { bg_color: solidFill(rootFrame.node) || "#FFFFFF" });
    screen.type = "LVGLScreenWidget";
    screen.children = screenChildren;
    screen.eventHandlers = screenHandlers;
    proj.userPages[0].components.push(screen, ...components);

    // 9) 字体
    const cjkNeeds = [...fontUse.entries()].filter(([k]) => k.startsWith("cjk"));
    if (cjkNeeds.length || true) {
        if (args.fontsFrom) {
            try {
                const tpl = JSON.parse(fs.readFileSync(args.fontsFrom, "utf8"));
                for (const f of tpl.fonts || []) proj.fonts.push(f);
                // 字体名映射：cjkNN → 模板 CJK 字体（最近字号）；icon 用的 MONTSERRAT → msym 模板字体
                const cjkFonts = proj.fonts.filter(f => /msyh|cjk|han|noto/i.test(f.name));
                const symFonts = proj.fonts.filter(f => /msym|symbol/i.test(f.name));
                const remap = {};
                for (const [k] of fontUse) {
                    if (k.startsWith("cjk")) {
                        const size = parseInt(k.slice(3)) || 15;
                        if (cjkFonts.length) {
                            const best = cjkFonts.reduce((a, b) => Math.abs((b.source?.size||15)-size) < Math.abs((a.source?.size||15)-size) ? b : a);
                            remap[k] = best.name;
                        }
                    }
                }
                const remapFont = w => {
                    if (!w.localStyles) return;
                    const d = w.localStyles.definition;
                    if (!d.MAIN || !d.MAIN.DEFAULT) return;
                    const t = d.MAIN.DEFAULT.text_font;
                    if (t && remap[t]) d.MAIN.DEFAULT.text_font = remap[t];
                    // 符号 label（PUA 字符）→ msym 字体
                    if (t && /^MONTSERRAT_/.test(t) && w.text && /[-]/.test(w.text) && symFonts.length) {
                        d.MAIN.DEFAULT.text_font = symFonts[0].name;
                    }
                };
                const walkRemap = w => { remapFont(w); (w.children || []).forEach(walkRemap); };
                screenChildren.forEach(walkRemap);
                console.log(`字体: 模板 ${proj.fonts.length} 个，映射 ${JSON.stringify(remap)}${symFonts.length ? " + 符号→" + symFonts[0].name : ""}`);
            } catch (e) { console.warn("  [warn] --fonts-from 读取失败:", e.message); }
        } else {
            console.warn(`  [warn] 含中文文本 ${[...new Set([].concat(...cjkNeeds.map(([, s]) => [...s])))].join("")} —— 需要 CJK 字体：`);
            console.warn("        经 MCP 桥 add_font 生成（ttc 需先提 ttf），或 --fonts-from 模板工程复制，否则中文不显示");
        }
    }

    // 10) 输出
    const out = args.out || args._[0].replace(/\.fig$/i, "") + ".eez-project";
    fs.mkdirSync(path.dirname(path.resolve(out)), { recursive: true });
    fs.writeFileSync(out, JSON.stringify(proj, null, 2), "utf8");
    console.log(`\n产出: ${out}`);
    console.log(`部件 ${countWidgets(screenChildren)} | 组件 ${components.length} | 连线 ${connectionLines.length} | 变量 ${variables.length} | 交互链 ${chainByVariant.size}`);
}
function countWidgets(ws) { let n = 0; for (const w of ws) { n++; n += countWidgets(w.children || []); } return n; }

main();
