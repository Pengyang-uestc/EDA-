#pragma once
#include <wx/string.h>
#include <wx/vector.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <algorithm>
#include <map>
#include <vector>
#include <utility>
#include "core/Component.h"
#include "core/Netlist.h"
#include "core/Subcircuit.h"

// ================================================================
// gEDA pcb 对接(老师《任务5分析.pdf》定标,参考文件在
// docs/课程资料/任务5参考-gEDA/)。定标流程:
//   gschem(原理图) → gsch2pcb(网表+初始布局) → pcb(布局布线) → 导出 Gerber/BOM
// 自研原理图程序替代 gschem,按老师 PDF 第 8 页结论必须输出两个文件给 pcb:
//   1) full_adder.net —— 网表:每行 "网络名<TAB>U1-3 U2-5 ..."(引脚=位号-1起编号)
//   2) full_adder.pcb —— 初始布局:Element(封装) 块内含 Pin 定义
// pcb 加载步骤(老师 PDF 2.3 节):File→Load layout data to buffer 选 .pcb →
//   File→Load netlist 选 .net → 按 O 飞线 → Connects→Auto-route all rats →
//   File→Export 导出 bom/gerber。
// 本文件的两个导出与老师样例文件逐字段对齐过(full_adder.net / full_adder.pcb)。
// ================================================================

// 元件映射约定(第一版做减法,见 docs/课程资料/结题阶段计划.md):
//   门/自定义/DFF → DIP14:逻辑脚按 1,2=输入 3=输出(对应 74 系列 slot1),
//                    功率脚按 DIP14 标准自动补:14=Vcc、7=GND(老师样例同样含 Vcc/GND 网络)
//   开关/时钟/LED → SIP2:逻辑脚 → 焊盘 1
// 不实现"同片多门共用 RefDes + slot"语义:每个门当一颗单门芯片(74AHC1G08/32/86),
// 物理上成立且老师 PDF 明确列了这组标准件。
inline bool GedaIsChip(const Component& c) {
    return c.type != SW_INPUT && c.type != SW_CLOCK && c.type != SW_LED;
}
inline const char* GedaFootprintName(const Component& c) {
    return GedaIsChip(c) ? "DIP14" : "SIP2";
}

// 子电路先展平:网表/PCB 描述的是实际元件,不描述层次封装
inline bool GedaFlatten(const wxVector<Component>& comps, const wxVector<Wire>& wires,
                        wxVector<Component>& flat, wxVector<Wire>& flatW) {
    flat = comps; flatW = wires;
    for (size_t i = 0; i < flat.size(); i++)
        if (flat[i].type == GATE_MODULE)
            return FlattenCircuit(comps, wires, flat, flatW);
    return true;
}

// 网络名:gEDA 不允许空格/括号,沿用老师样例的 unnamed_netN 风格,
// 顺序与 BuildNets 的稳定排序一致(同一电路重复导出结果相同,便于比对)
inline wxString ExportGedaNetlistText(const wxVector<Component>& comps, const wxVector<Wire>& wires) {
    wxVector<Component> flat; wxVector<Wire> flatW;
    if (!GedaFlatten(comps, wires, flat, flatW)) return {};
    if (!BuildCircuitNetlist(flat, flatW).valid()) return {};
    std::map<int, const Component*> byId;
    for (size_t i = 0; i < flat.size(); i++) byId[flat[i].id] = &flat[i];

    wxString s;
    int n = 0;
    auto emitNet = [&](const wxString& name, std::vector<NetPin> pins) {
        std::sort(pins.begin(), pins.end());
        s += name + "\t";
        for (size_t k = 0; k < pins.size(); k++)
            s += wxString::Format("U%d-%d ", pins[k].first, pins[k].second + 1);   // 引脚从 1 编号
        s += "\n";
    };
    for (const auto& pins : BuildNets(flatW))
        emitNet(wxString::Format("unnamed_net%d", ++n), pins);
    // 功率网络:DIP14 标准 14=Vcc、7=GND(内部 0 起编号为 13、6)
    std::vector<NetPin> vcc, gnd;
    for (size_t i = 0; i < flat.size(); i++)
        if (GedaIsChip(flat[i])) { vcc.push_back({flat[i].id, 13}); gnd.push_back({flat[i].id, 6}); }
    if (!gnd.empty()) emitNet("GND", gnd);
    if (!vcc.empty()) emitNet("Vcc", vcc);
    return s;
}

// 初始布局:每个元件一个 Element 块。焊盘几何逐字段取自老师样例 full_adder.pcb
// (该文件在 pcb 1.99x 实际加载过,坐标尺度保持一致,勿"修正"):
//   DIP14:左右两列 7 脚,行距 100;SIP2:单列 2 脚,行距 100。
// 位置:老师样例把所有元件叠在 (220,100)(gsch2pcb 的行为,pcb 里再 Disperse 分散);
// 我们直接按网格铺开,不用 Disperse 也能看清。
inline wxString ExportGedaPcbText(const wxVector<Component>& comps, const wxVector<Wire>& wires) {
    wxVector<Component> flat; wxVector<Wire> flatW;
    if (!GedaFlatten(comps, wires, flat, flatW)) return {};

    wxString s =
        "# release: pcb 1.99x\n"
        "# To read pcb files, the pcb version (or the cvs source date) must be >= the file version\n"
        "FileVersion[20070407]\n"
        "PCB[\"MyEDA\" 600000 500000]\n"
        "Grid[10000.000000 0 0 0]\n"
        "Cursor[0 0 0.000000]\n"
        "PolyArea[200000000.000000]\n"
        "Thermal[0.500000]\n"
        "DRC[1000 1000 1000 1000 1500 1000]\n"
        "Flags(\"nameonpcb,uniquename,clearnew,snappin\")\n"
        "Groups(\"1,c:2:3:4:5:6,s:7:8\")\n"
        "Styles[\"Signal,1000,3600,2000,1000:Power,2500,6000,3500,1000:Fat,4000,6000,3500,1000:Skinny,600,2402,1181,600\"]\n"
        "\n";
    int idx = 0;
    for (size_t i = 0; i < flat.size(); i++) {
        const Component& c = flat[i];
        // pcb 文件坐标是 centimil(1/100 mil),DIP14 外形约 40000x70000(400x700 mil)
        // → 网格步距 60000x90000(600x900 mil),从 (60000,40000) 起铺开互不重叠
        const int x = 60000 + (int)(idx % 3) * 60000;         // 每行 3 个
        const int y = 40000 + (int)(idx / 3) * 90000;
        const wxString value = PartName(c);
        s += wxString::Format("Element(0x00 \"%s\" \"U%d\" \"%s\" %d %d 3 100 0x00)\n(\n",
                              GedaFootprintName(c), c.id, value.utf8_str(), x, y);
        if (GedaIsChip(c)) {
            // 关键:.pcb 里的 Pin/ElementLine 坐标是板上"绝对坐标"——gsch2pcb 堆叠元件时
            // 引脚本来就重叠,靠 pcb 里 Disperse 分开;我们直接按网格平移,不用 Disperse。
            // 几何取老师样例再 ×100:DIP14 脚距 10000=100 mil、焊盘 6000=60 mil(centimil)
            for (int pin = 1; pin <= 7; pin++)                 // 左列 1..7,自上而下;首脚标志 0x101(老师样例)
                s += wxString::Format("\tPin(%d %d 6000 2800 \"%d\" 0x%s)\n", x + 5000, y + (pin - 1) * 10000 + 5000, pin,
                                      pin == 1 ? "101" : "01");
            s += "\n";
            for (int pin = 8; pin <= 14; pin++)                // 右列 8..14,自下而上
                s += wxString::Format("\tPin(%d %d 6000 2800 \"%d\" 0x01)\n", x + 35000, y + (14 - pin) * 10000 + 5000, pin);
            s += "\n";
            s += wxString::Format("\tElementLine(%d %d %d %d 10)\n", x, y, x, y + 70000)
                 + wxString::Format("\tElementLine(%d %d %d %d 10)\n", x, y + 70000, x + 40000, y + 70000)
                 + wxString::Format("\tElementLine(%d %d %d %d 10)\n", x + 40000, y + 70000, x + 40000, y)
                 + wxString::Format("\tElementLine(%d %d %d %d 10)\n", x, y, x + 15000, y)
                 + wxString::Format("\tElementLine(%d %d %d %d 10)\n", x + 25000, y, x + 40000, y)
                 + wxString::Format("\tElementArc(%d %d 5000 5000 0 180 10)\n", x + 20000, y)
                 + wxString::Format("\tMark(%d %d)\n", x + 5000, y + 5000);
        } else {
            s += wxString::Format("\tPin(%d %d 6000 2800 \"1\" 0x101)\n", x + 5000, y + 5000)
                 + wxString::Format("\tPin(%d %d 6000 2800 \"2\" 0x01)\n", x + 5000, y + 15000)
                 + "\n"
                 + wxString::Format("\tElementLine(%d %d %d %d 10)\n", x, y, x, y + 20000)
                 + wxString::Format("\tElementLine(%d %d %d %d 10)\n", x, y + 20000, x + 10000, y + 20000)
                 + wxString::Format("\tElementLine(%d %d %d %d 10)\n", x + 10000, y + 20000, x + 10000, y)
                 + wxString::Format("\tElementLine(%d %d %d %d 10)\n", x, y, x + 10000, y)
                 + wxString::Format("\tMark(%d %d)\n", x + 5000, y + 5000);
        }
        s += ")\n\n";
        idx++;
    }
    s += "Layer(1 \"top\")\n(\n)\n"
         "Layer(2 \"ground\")\n(\n)\n"
         "Layer(3 \"signal2\")\n(\n)\n"
         "Layer(4 \"signal3\")\n(\n)\n"
         "Layer(5 \"power\")\n(\n)\n"
         "Layer(6 \"bottom\")\n(\n)\n"
         "Layer(7 \"outline\")\n(\n)\n"
         "Layer(8 \"spare\")\n(\n)\n"
         "Layer(9 \"silk\")\n(\n)\n"
         "Layer(10 \"silk\")\n(\n)\n";
    return s;
}

// 生成并写文件(与 SaveNetlist 同样的 UTF-8 落盘方式)
inline bool SaveGedaNetlist(const wxString& path,
                            const wxVector<Component>& comps, const wxVector<Wire>& wires) {
    const wxString text = ExportGedaNetlistText(comps, wires);
    if (text.empty()) return false;
    wxFile f;
    if (!f.Open(path, wxFile::write)) return false;
    const bool written = f.Write(text, wxConvUTF8);
    return f.Close() && written;
}
inline bool SaveGedaPcb(const wxString& path,
                        const wxVector<Component>& comps, const wxVector<Wire>& wires) {
    const wxString text = ExportGedaPcbText(comps, wires);
    if (text.empty()) return false;
    wxFile f;
    if (!f.Open(path, wxFile::write)) return false;
    const bool written = f.Write(text, wxConvUTF8);
    return f.Close() && written;
}
