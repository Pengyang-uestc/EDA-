#pragma once
#include <wx/wx.h>
#include <wx/vector.h>
#include <wx/file.h>
#include "Component.h"

// ================================================================
// KiCad 网表导出(S 表达式格式,KiCad 6+)。
// 网表 = 电路的"电气描述":有哪些元件、哪些引脚彼此相连。
// PCB 软件(KiCad)读进网表,就知道该把哪些焊盘连在一起。
// 我们约定:每根连线 = 一个网络(两个节点:起点引脚 + 终点引脚)
// ================================================================

// 把整份电路导出为 KiCad 网表文本
inline wxString ExportKiCadNetlist(const wxVector<Component>& comps, const wxVector<Wire>& wires) {
    wxString s = "(export (version \"E\")\n";
    s += "  (design\n";
    s += "    (source \"MyEDA schematic\")\n";
    s += "    (date \"2026-09-17\")\n";
    s += "    (tool \"MyEDA 0.1\")\n";
    s += "  )\n";

    // ---------- 元件清单 ----------
    s += "  (components\n";
    for (size_t i = 0; i < comps.size(); i++) {
        const Component& c = comps[i];
        s += wxString::Format("    (comp (ref \"U%d\")\n", c.id);
        s += wxString::Format("      (value \"%s\")\n", GateTypeName(c.type));
        s += "      (footprint \"\")\n";
        s += wxString::Format("      (libsource (lib \"MyEDA\") (part \"%s\") (description \"\"))\n",
                              GateTypeName(c.type));
        s += "      (sheetpath (names \"/\") (tstamps \"/\"))\n";
        s += "    )\n";
    }
    s += "  )\n";

    // 空电路也要有这两个空段,KiCad 解析器要求
    s += "  (libparts\n  )\n";

    // ---------- 网络:每根连线一个网络 ----------
    s += "  (nets\n";
    for (size_t i = 0; i < wires.size(); i++) {
        const Wire& w = wires[i];
        s += wxString::Format("    (net (code \"%d\") (name \"Net-U%d_%d_U%d_%d\")\n",
                              (int)i + 1, w.comp1, w.pin1, w.comp2, w.pin2);
        s += wxString::Format("      (node (ref \"U%d\") (pin \"%d\"))\n", w.comp1, w.pin1);
        s += wxString::Format("      (node (ref \"U%d\") (pin \"%d\"))\n", w.comp2, w.pin2);
        s += "    )\n";
    }
    s += "  )\n";
    s += ")\n";
    return s;
}

// 生成并写文件
inline bool SaveNetlist(const wxString& path,
                        const wxVector<Component>& comps, const wxVector<Wire>& wires) {
    wxFile f;
    if (!f.Open(path, wxFile::write))
        return false;
    f.Write(ExportKiCadNetlist(comps, wires), wxConvUTF8);
    f.Close();
    return true;
}
