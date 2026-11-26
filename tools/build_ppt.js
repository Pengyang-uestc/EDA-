// MyEDA 结题答辩 PPT 生成(pptxgenjs)
// 用法: node build_ppt.js   输出 MyEDA结题答辩.pptx
const pptxgen = require("pptxgenjs");
const path = require("path");
const A = p => path.join(__dirname, "ppt_assets", p);

const W = 13.33, H = 7.5, M = 0.5;
// 配色:BACKGROUND 深海军蓝(PCB 阻焊) / PRIMARY 结构蓝 / ACCENT 示波器青
const C = { bg: "0E2A47", pri: "17457E", acc: "0D9488", txt: "1E293B", mut: "64748B",
            lite: "EAF1F8", line: "D8E2EE", white: "FFFFFF" };
const F = "微软雅黑";
const TOTAL = 15;

let p = new pptxgen();
p.layout = "LAYOUT_WIDE";
p.author = "MyEDA 项目组";
p.title = "MyEDA 结题答辩";

const bu = () => ({ code: "2022", indent: 12 });
const sh = () => ({ type: "outer", color: "1E293B", blur: 8, offset: 2, angle: 90, opacity: 0.16 });

function pageNum(s, i) {
  s.addText(`${String(i).padStart(2, "0")} / ${TOTAL}`,
    { x: W - 1.4, y: H - 0.42, w: 1.1, h: 0.3, fontSize: 11, fontFace: F, color: C.mut, align: "right", margin: 0 });
}
function title(s, t, sub) {
  s.addText(t, { x: M, y: 0.35, w: W - 2 * M, h: 0.62, fontSize: 29, bold: true, fontFace: F, color: C.pri, margin: 0 });
  if (sub) s.addText(sub, { x: M, y: 0.98, w: W - 2 * M, h: 0.34, fontSize: 13.5, fontFace: F, color: C.mut, margin: 0 });
}
function imgBox(s, img, x, y, w, h) {
  s.addShape(p.shapes.ROUNDED_RECTANGLE, { x: x - 0.05, y: y - 0.05, w: w + 0.1, h: h + 0.1,
    rectRadius: 0.07, fill: { color: C.white }, line: { color: C.line, width: 1 }, shadow: sh() });
  s.addImage({ path: img, x, y, w, h, sizing: { type: "contain", w, h } });
}
function card(s, x, y, w, h, fill) {
  s.addShape(p.shapes.ROUNDED_RECTANGLE, { x, y, w, h, rectRadius: 0.09,
    fill: { color: fill || C.lite }, line: { color: C.line, width: 0.75 } });
}

/* ---------- 1 封面 ---------- */
let s = p.addSlide(); s.background = { color: C.bg };
s.addText("MyEDA", { x: 1.1, y: 1.7, w: 11, h: 1.4, fontSize: 66, bold: true, fontFace: F, color: C.white, margin: 0 });
s.addText("数字电路原理图编辑与组合逻辑仿真软件", { x: 1.15, y: 3.15, w: 11, h: 0.6, fontSize: 24, fontFace: F, color: "9DC3E6", margin: 0 });
s.addText("进阶式挑战性综合项目 I · 结题答辩", { x: 1.15, y: 3.9, w: 11, h: 0.45, fontSize: 16, fontFace: F, color: C.acc, margin: 0 });
s.addText([
  { text: "小组成员", options: { color: "9DC3E6", breakLine: true } },
  { text: "丰鹏扬(组长) · 郑晨曦 · 王际超 · 杨谨淳", options: { color: C.white, bold: true } },
], { x: 1.15, y: 5.1, w: 8, h: 0.9, fontSize: 16, fontFace: F, margin: 0, paraSpaceAfter: 6 });
s.addText("2026 年 11 月", { x: 1.15, y: 6.3, w: 8, h: 0.4, fontSize: 13, fontFace: F, color: "6E8CAx".slice(0,6) === "6E8CAx" ? "7E9CBC" : "7E9CBC", margin: 0 });

/* ---------- 2 项目概览 ---------- */
s = p.addSlide(); s.background = { color: C.white };
title(s, "项目概览:一条完整的数字电路设计链路",
  "C++17 + wxWidgets + CMake · 画电路 → 仿真 → 存档 → 导出 KiCad / gEDA → 真实 PCB 工具布线");
const stats = [
  ["227", "项自动化测试全通过", "test_sim 89 + modules 53 + canvas 85"],
  ["4", "大功能模块", "界面 / 画布 / 文件 / 仿真"],
  ["2", "条工业 EDA 互操作链路", "KiCad 与 gEDA 双格式导出"],
  ["8/8", "pcb-rnd 网络自动布通", "0 短路 · 13 个 Gerber 导出"],
];
stats.forEach((st, i) => {
  const x = M + i * 3.13;
  card(s, x, 1.7, 2.93, 2.5, i === 3 ? "E6F4F1" : C.lite);
  s.addText(st[0], { x: x + 0.15, y: 1.95, w: 2.63, h: 1.1, fontSize: 54, bold: true, fontFace: F,
    color: i === 3 ? C.acc : C.pri, align: "center", margin: 0 });
  s.addText(st[1], { x: x + 0.15, y: 3.1, w: 2.63, h: 0.5, fontSize: 14, bold: true, fontFace: F, color: C.txt, align: "center", margin: 0 });
  s.addText(st[2], { x: x + 0.15, y: 3.62, w: 2.63, h: 0.5, fontSize: 10.5, fontFace: F, color: C.mut, align: "center", margin: 0 });
});
s.addText([
  { text: "面向数字电路课程实验场景:画得出、连得对、仿得准、交得出。", options: { breakLine: true } },
  { text: "源码约 4000 行,任务 1–6 全部完成,任务 5(gEDA 定标)已按老师 PDF 全链路实机验证。" },
], { x: M, y: 4.7, w: W - 2 * M, h: 1.2, fontSize: 15, fontFace: F, color: C.txt, margin: 0, paraSpaceAfter: 8 });
pageNum(s, 2);

/* ---------- 3 系统架构 ---------- */
s = p.addSlide(); s.background = { color: C.white };
title(s, "系统架构:公共数据模型 + 四模块分层",
  "核心原则:数据是核心,界面只是把数据画出来 —— 渲染 / 仿真 / 导出共用同一权威模型");
imgBox(s, A("module_diagram.png"), 3.4, 1.5, 6.5, 5.4);
s.addText([
  { text: "界面层(A) 主窗口/菜单/属性表", options: { bullet: bu(), breakLine: true } },
  { text: "画布层(B) 渲染/选择/撤销重做", options: { bullet: bu(), breakLine: true } },
  { text: "文件层(C) 存档校验/网表导出", options: { bullet: bu(), breakLine: true } },
  { text: "仿真层(D) 传播引擎/测试集成", options: { bullet: bu() } },
], { x: 10.1, y: 2.0, w: 2.9, h: 3.2, fontSize: 13, fontFace: F, color: C.txt, margin: 0, paraSpaceAfter: 10 });
pageNum(s, 3);

/* ---------- 4 复杂工程问题 ---------- */
s = p.addSlide(); s.background = { color: C.white };
title(s, "归纳的三个复杂工程问题", "来自需求分析,贯穿设计、实现与验证全程");
const probs = [
  ["01", "图形 ↔ 电气一致性", "原理图是图形(符号/拐点),仿真与导出依赖电气(网络/引脚)。拖动、增删连线同时影响两个视图,必须以单一权威数据模型驱动渲染、仿真与导出。"],
  ["02", "编辑状态跨模块一致", "撤销栈、未保存标记、只读约束、元件库变更横跨四个模块,任何一处不同步都会导致“撤销后不一致”“仿真中可编辑”等缺陷。"],
  ["03", "工业格式互操作", "KiCad 与 gEDA 格式迥异且无完整官方文档,需逆向对齐文本格式、坐标量级与封装模板,并保证导出文件被真实工具接受、进入布线流程。"],
];
probs.forEach((pr, i) => {
  const x = M + i * 4.28;
  card(s, x, 1.6, 4.05, 4.9, C.lite);
  s.addText(pr[0], { x: x + 0.25, y: 1.85, w: 1.4, h: 0.9, fontSize: 44, bold: true, fontFace: F, color: C.acc, margin: 0 });
  s.addText(pr[1], { x: x + 0.25, y: 2.85, w: 3.55, h: 0.5, fontSize: 17, bold: true, fontFace: F, color: C.pri, margin: 0 });
  s.addText(pr[2], { x: x + 0.25, y: 3.45, w: 3.55, h: 2.9, fontSize: 12.5, fontFace: F, color: C.txt, margin: 0, lineSpacingMultiple: 1.15 });
});
pageNum(s, 4);

/* ---------- 5 功能演示:原理图与仿真 ---------- */
s = p.addSlide(); s.background = { color: C.white };
title(s, "功能演示:原理图编辑与组合逻辑仿真",
  "6 种逻辑门 + 开关/指示灯/时钟源/D 触发器/自定义元件 · 点开关实时重算、导线按电位着色");
imgBox(s, A("shot_main.png"), M, 1.6, 6.1, 4.3);
s.addText("三栏主界面:元件库树 / 画布 / 属性表", { x: M, y: 6.0, w: 6.1, h: 0.35, fontSize: 12, fontFace: F, color: C.mut, align: "center", margin: 0 });
imgBox(s, A("shot_sim.png"), 6.95, 1.6, 5.9, 4.3);
s.addText("半加器仿真:00/01/10/11 真值表全覆盖", { x: 6.95, y: 6.0, w: 5.9, h: 0.35, fontSize: 12, fontFace: F, color: C.mut, align: "center", margin: 0 });
pageNum(s, 5);

/* ---------- 6 功能演示:时序波形 ---------- */
s = p.addSlide(); s.background = { color: C.white };
title(s, "功能演示:时序仿真波形(F7)",
  "每个仿真步进记录各元件输出,自动刷新;开关切换产生跳变,XOR / AND 输出同步翻转");
imgBox(s, A("shot_wave.png"), M, 1.55, 8.2, 4.9);
s.addText([
  { text: "波形窗口与仿真联动,每 250ms 自动刷新", options: { bullet: bu(), breakLine: true } },
  { text: "方波绘制:水平段延伸到步进边界,电平变化处画竖直跳变沿", options: { bullet: bu(), breakLine: true } },
  { text: "实机验证:真实点击开关,逐像素核对三条轨迹跳变沿同列对齐", options: { bullet: bu(), breakLine: true } },
  { text: "9 项渲染自动化测试覆盖绘制逻辑", options: { bullet: bu() } },
], { x: 9.0, y: 2.0, w: 3.9, h: 4.0, fontSize: 13, fontFace: F, color: C.txt, margin: 0, paraSpaceAfter: 12 });
pageNum(s, 6);

/* ---------- 7 关键设计决策 ---------- */
s = p.addSlide(); s.background = { color: C.white };
title(s, "关键设计决策", "每个决策都对应一个真实踩过的坑");
const decisions = [
  ["快照式撤销", "JSON 快照压栈,不写反操作 —— 撤销/重做天然一致,元件库变更纳入同一状态管理"],
  ["并查集网络归并", "连线拐点/引脚扇出正确归并成网络 —— 解决“导出网表与图不符”的隐性错误"],
  ["双缓冲 + 静态缓存", "静态内容缓存、仅刷新预览区域 —— 消除拖动/连线时的闪烁"],
  ["提示回调(依赖倒置)", "画布不依赖主窗口,状态栏提示经回调上抛 —— 消灭“静默失败”"],
];
decisions.forEach((d, i) => {
  const y = 1.55 + i * 1.32;
  card(s, M, y, 12.33, 1.12, i % 2 ? C.lite : "F5F8FB");
  s.addText(String(i + 1), { x: M + 0.18, y: y + 0.18, w: 0.8, h: 0.8, fontSize: 34, bold: true, fontFace: F, color: C.acc, margin: 0 });
  s.addText(d[0], { x: M + 1.1, y: y + 0.16, w: 2.9, h: 0.8, fontSize: 17, bold: true, fontFace: F, color: C.pri, valign: "middle", margin: 0 });
  s.addText(d[1], { x: M + 4.1, y: y + 0.12, w: 8.0, h: 0.9, fontSize: 12.5, fontFace: F, color: C.txt, valign: "middle", margin: 0 });
});
pageNum(s, 7);

/* ---------- 8 任务5 定标与链路 ---------- */
s = p.addSlide(); s.background = { color: C.white };
title(s, "任务 5 定标:接入 gEDA 开源 EDA 工具链",
  "老师《任务5分析.pdf》:自研原理图程序必须输出 .net 网表 + .pcb 初始布局,交给 pcb 布线制板");
const steps = ["MyEDA\n画原理图", "导出\n.net + .pcb", "pcb-rnd\n加载布局", "网表导入\n生成飞线", "自动布线\nAddRats+Route", "导出\nGerber/BOM"];
steps.forEach((t, i) => {
  const x = M + i * 2.12;
  card(s, x, 2.0, 1.92, 1.5, i === 4 ? "E6F4F1" : C.lite);
  s.addText(t, { x: x + 0.06, y: 2.08, w: 1.8, h: 1.34, fontSize: 12.5, bold: i === 4, fontFace: F,
    color: i === 4 ? C.acc : C.pri, align: "center", valign: "middle", margin: 0 });
  if (i < steps.length - 1) s.addText("→", { x: x + 1.9, y: 2.45, w: 0.25, h: 0.6, fontSize: 18, fontFace: F, color: C.mut, align: "center", margin: 0 });
});
s.addText([
  { text: "自研程序替代 gschem + gsch2pcb,输出与老师样例逐字段对齐", options: { bullet: bu(), breakLine: true } },
  { text: "批处理脚本与 GUI 菜单两条路径均实测走通,证据可复现", options: { bullet: bu(), breakLine: true } },
  { text: "GUI 真实操作复核归档:课程资料/任务5实机验证/", options: { bullet: bu() } },
], { x: M, y: 4.0, w: W - 2 * M, h: 1.6, fontSize: 14, fontFace: F, color: C.txt, margin: 0, paraSpaceAfter: 10 });
card(s, M, 5.6, 12.33, 1.0, "FFF4E5");
s.addText("旧结论更正:“pcb-rnd 自动布线报 stale 属工具缺陷”是误判 —— 真实原因是脚本漏了先生成飞线(AddRats)这一步;补齐后我方文件与老师参考文件都能布通。",
  { x: M + 0.2, y: 5.7, w: 11.9, h: 0.8, fontSize: 13, fontFace: F, color: "8A4B08", valign: "middle", margin: 0 });
pageNum(s, 8);

/* ---------- 9 gEDA 导出实现 ---------- */
s = p.addSlide(); s.background = { color: C.white };
title(s, "gEDA 导出实现:按真实工具样例逐字段逆向",
  "格式细节决定成败 —— 一次坐标量级误判(放大 100 倍)曾导致元件重叠、988 处短路,已按老师样例纠正");
const gpts = [
  ["封装模板", "门/触发器 → DIP14(脚距 100,焊盘直径 60)\n开关/灯 → SIP2(单列 2 脚)", "1,2=输入,3=输出,14=Vcc,7=GND"],
  ["功率网络自动补", "原理图不画电源,导出时自动生成 Vcc / GND 网络", "与老师样例 .net 行为一致"],
  ["初始布局", "按网格铺开元件(步距大于最大封装),加载后无需 Disperse", "Mark=元件原点,文字位=位号位置"],
  ["确定性", "网络名 unnamed_netN 顺序稳定,同电路重复导出逐字节一致", "便于与老师样例逐行比对"],
];
gpts.forEach((g, i) => {
  const x = M + (i % 2) * 6.3, y = 1.7 + Math.floor(i / 2) * 2.5;
  card(s, x, y, 6.0, 2.25, C.lite);
  s.addText(g[0], { x: x + 0.22, y: y + 0.16, w: 5.5, h: 0.45, fontSize: 16, bold: true, fontFace: F, color: C.pri, margin: 0 });
  s.addText(g[1], { x: x + 0.22, y: y + 0.68, w: 5.6, h: 1.0, fontSize: 12.5, fontFace: F, color: C.txt, margin: 0 });
  s.addText(g[2], { x: x + 0.22, y: y + 1.7, w: 5.6, h: 0.4, fontSize: 11, fontFace: F, color: C.mut, margin: 0 });
});
pageNum(s, 9);

/* ---------- 10 gEDA 实机验证 ---------- */
s = p.addSlide(); s.background = { color: C.white };
title(s, "gEDA 实机验证:pcb-rnd 全网络自动布通",
  "pcb-rnd 3.1.4 (WSL Ubuntu) 实测 —— 脚本与 GUI 真实操作两条证据链,归档于 任务5实机验证/");
imgBox(s, A("shot_geda.png"), M, 1.6, 7.3, 4.9);
s.addText("全加器 .pcb + .net → 自动布线渲染(红色走线)", { x: M, y: 6.55, w: 7.3, h: 0.35, fontSize: 12, fontFace: F, color: C.mut, align: "center", margin: 0 });
const gv = [["8/8", "网络布通"], ["0", "短路"], ["68", "段走线 / 0 过孔"], ["13", "个 Gerber 文件"]];
gv.forEach((g, i) => {
  const y = 1.75 + i * 1.2;
  card(s, 8.35, y, 4.45, 1.05, i === 0 ? "E6F4F1" : C.lite);
  s.addText(g[0], { x: 8.5, y: y + 0.1, w: 1.7, h: 0.85, fontSize: 30, bold: true, fontFace: F, color: i === 0 ? C.acc : C.pri, valign: "middle", margin: 0 });
  s.addText(g[1], { x: 10.3, y: y + 0.1, w: 2.4, h: 0.85, fontSize: 15, fontFace: F, color: C.txt, valign: "middle", margin: 0 });
});
pageNum(s, 10);

/* ---------- 11 KiCad 互操作 ---------- */
s = p.addSlide(); s.background = { color: C.white };
title(s, "KiCad 互操作:真实网络号布线,DRC 干净",
  "Lee 网格布线器读真实焊盘与网络号,自动绕开其他焊盘/走线;推翻初版“全写同一网络号且穿焊盘”的无效布线");
imgBox(s, A("shot_kicad.png"), M, 1.6, 7.3, 4.9);
s.addText("布线板顶视图(KiCad 10 实测)", { x: M, y: 6.55, w: 7.3, h: 0.35, fontSize: 12, fontFace: F, color: C.mut, align: "center", margin: 0 });
const kv = [
  ["110", "段走线,按 4 个网络正确分布(25/51/12/22)"],
  ["0", "DRC 错误 · 0 未连通项(kicad-cli pcb drc)"],
  ["0", "错误加载:网表导入 KiCad 无错误"],
];
kv.forEach((g, i) => {
  const y = 1.8 + i * 1.5;
  card(s, 8.35, y, 4.45, 1.3, C.lite);
  s.addText(g[0], { x: 8.55, y: y + 0.18, w: 1.5, h: 0.95, fontSize: 34, bold: true, fontFace: F, color: C.acc, valign: "middle", margin: 0 });
  s.addText(g[1], { x: 10.15, y: y + 0.14, w: 2.55, h: 1.05, fontSize: 12.5, fontFace: F, color: C.txt, valign: "middle", margin: 0 });
});
pageNum(s, 11);

/* ---------- 12 测试体系 ---------- */
s = p.addSlide(); s.background = { color: C.white };
title(s, "测试体系:227 项自动化检查,分层回归",
  "每次提交全部通过;新增功能必须附带测试 —— 分层覆盖算法、模块与界面渲染");
s.addChart(p.charts.BAR, [{
  name: "检查项数", labels: ["仿真与文件\n(test_sim)", "模块单元\n(test_modules)", "画布交互渲染\n(test_canvas)"],
  values: [89, 53, 85],
}], {
  x: M, y: 1.7, w: 7.4, h: 4.6, barDir: "col",
  chartColors: ["17457E", "1F6FB2", "0D9488"], varyColors: true,
  chartArea: { fill: { color: "FFFFFF" } },
  catAxisLabelColor: C.mut, valAxisLabelColor: C.mut, catAxisLabelFontFace: F, valAxisLabelFontFace: F,
  catAxisLabelFontSize: 12, valAxisLabelFontSize: 11,
  valGridLine: { color: "E2E8F0", size: 0.5 }, catGridLine: { style: "none" },
  showValue: true, dataLabelPosition: "outEnd", dataLabelColor: C.txt, dataLabelFontFace: F, dataLabelFontSize: 14,
  showLegend: false, valAxisMaxVal: 100,
});
s.addText([
  { text: "单元 → GUI 自动化 → 端到端 三层验证", options: { bullet: bu(), breakLine: true } },
  { text: "半加器真值表 00/01/10/11 全覆盖", options: { bullet: bu(), breakLine: true } },
  { text: "波形渲染 9 项:逐像素核对跳变沿", options: { bullet: bu(), breakLine: true } },
  { text: "真实工具实测:pcb-rnd 8/8 布通\nKiCad DRC 0 错误", options: { bullet: bu() } },
], { x: 8.3, y: 2.1, w: 4.5, h: 3.6, fontSize: 13.5, fontFace: F, color: C.txt, margin: 0, paraSpaceAfter: 14 });
pageNum(s, 12);

/* ---------- 13 四人分工 ---------- */
s = p.addSlide(); s.background = { color: C.white };
title(s, "分工协作:按模块划地盘,公共接口联集成", "core/ 数据模型全组共用;跨模块故障联合排查,集成以测试套件验收");
const team = [
  ["丰鹏扬", "A · 界面", "主窗口/菜单工具栏\n属性表联动\n自定义元件对话框\n波形窗口入口", "src/ui/"],
  ["郑晨曦", "B · 画布", "渲染/网格吸附\n直角连线/选择编辑\n撤销重做/缩放平移\n双缓冲防闪烁", "src/editor/"],
  ["王际超", "C · 文件", "JSON 存档校验\n网表导出导入\nKiCad / gEDA 对接\n任务5 主实现", "src/file/"],
  ["杨谨淳", "D · 仿真", "传播引擎/电位着色\n227 项测试体系\n构建脚本/演示验证\n版本与证据汇总", "src/sim/ test/"],
];
team.forEach((t, i) => {
  const x = M + i * 3.13;
  card(s, x, 1.6, 2.93, 4.9, C.lite);
  s.addText(t[0], { x: x + 0.15, y: 1.8, w: 2.63, h: 0.55, fontSize: 20, bold: true, fontFace: F, color: C.pri, align: "center", margin: 0 });
  s.addText(t[1], { x: x + 0.15, y: 2.4, w: 2.63, h: 0.4, fontSize: 13, bold: true, fontFace: F, color: C.acc, align: "center", margin: 0 });
  s.addText(t[2], { x: x + 0.2, y: 2.95, w: 2.53, h: 2.3, fontSize: 11.5, fontFace: F, color: C.txt, margin: 0, lineSpacingMultiple: 1.15 });
  s.addText(t[3], { x: x + 0.15, y: 5.85, w: 2.63, h: 0.4, fontSize: 10.5, fontFace: F, color: C.mut, align: "center", margin: 0 });
});
pageNum(s, 13);

/* ---------- 14 困难与解决 ---------- */
s = p.addSlide(); s.background = { color: C.white };
title(s, "遇到的困难与解决", "全部来自真实开发过程,以自动化测试 + 真实工具实测闭环定位");
const hard = [
  ["编辑状态跨模块泄漏", "新建工程后只读标志未复位 → 元件放置失败、波形崩溃", "文档状态集中管理,新建/切换随数据一并复位,补回归测试"],
  ["波形窗口不显示/错画", "绘制事件未绑定 + 遮挡面板 + 跳变沿分段错误", "绑定 Paint、移除遮挡、重写方波分段;9 项渲染测试 + 像素级核对"],
  ["gEDA 布线误判", "报 stale 被误归因为工具缺陷,差点放弃自动布线", "逐层复测定位漏 AddRats;补齐后 8/8 网络布通,纠正结论"],
  ["KiCad 布线无效", "初版走线全写同一网络号且穿焊盘,未过 DRC", "改 Lee 网格布线器:真实网络号 + 避让 + 补板框,DRC 0 错误"],
];
hard.forEach((hh, i) => {
  const y = 1.5 + i * 1.36;
  card(s, M, y, 12.33, 1.18, i % 2 ? C.lite : "F5F8FB");
  s.addText(hh[0], { x: M + 0.2, y: y + 0.12, w: 2.6, h: 0.95, fontSize: 14.5, bold: true, fontFace: F, color: C.pri, valign: "middle", margin: 0 });
  s.addText([{ text: "问题  ", options: { bold: true, color: "B45309" } }, { text: hh[1] }],
    { x: M + 3.0, y: y + 0.1, w: 4.6, h: 1.0, fontSize: 11.5, fontFace: F, color: C.txt, valign: "middle", margin: 0 });
  s.addText([{ text: "解决  ", options: { bold: true, color: C.acc } }, { text: hh[2] }],
    { x: M + 7.75, y: y + 0.1, w: 4.4, h: 1.0, fontSize: 11.5, fontFace: F, color: C.txt, valign: "middle", margin: 0 });
});
pageNum(s, 14);

/* ---------- 15 总结(深底) ---------- */
s = p.addSlide(); s.background = { color: C.bg };
s.addText("总结", { x: 1.1, y: 0.9, w: 6, h: 0.8, fontSize: 36, bold: true, fontFace: F, color: C.white, margin: 0 });
s.addText([
  { text: "六个任务全部完成;任务 5(gEDA 定标)全链路实机验证", options: { bullet: bu(), breakLine: true, color: C.white } },
  { text: "227 项自动化测试全 PASS;KiCad DRC 0 错误;pcb-rnd 8/8 网络布通", options: { bullet: bu(), breakLine: true, color: C.white } },
  { text: "分层架构 + 快照撤销 + 并查集归并 + 真实工具闭环验证", options: { bullet: bu(), breakLine: true, color: C.white } },
  { text: "证据可复现:verify_geda_pcb-rnd.sh / route_half_adder_kicad.py / 任务5实机验证归档", options: { bullet: bu(), color: C.white } },
], { x: 1.1, y: 1.9, w: 11, h: 2.2, fontSize: 16, fontFace: F, margin: 0, paraSpaceAfter: 12 });
card(s, 1.1, 4.3, 11.1, 0.95, "15385C");
s.addText("汇报口径:Gerber 已导出 ≠ 已送厂制板验证", { x: 1.3, y: 4.42, w: 10.7, h: 0.7, fontSize: 15, bold: true, fontFace: F, color: "F5B14C", valign: "middle", margin: 0 });
s.addText("谢谢观看  ·  Q & A", { x: 1.1, y: 5.9, w: 11, h: 0.8, fontSize: 30, bold: true, fontFace: F, color: C.acc, margin: 0 });
pageNum(s, 15);

p.writeFile({ fileName: path.join(__dirname, "MyEDA结题答辩.pptx") }).then(f => console.log("done:", f));
