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

// ================================================================
// 导入:解析 KiCad 网表,反向重建电路。
// 说明:网表只含"电气信息"(元件类型/编号、引脚连接),不含坐标和开关状态,
// 所以导入后的元件按网格自动排布、开关全为"关"——这是网表这种格式的天然限制
// ================================================================
inline bool ParseKiCadNetlist(const wxString& text,
                              wxVector<Component>& comps, wxVector<Wire>& wires) {
    comps.clear();
    wires.clear();

    std::string t = std::string((const char*)text.utf8_str());
    std::regex compRe("\\(comp \\(ref \"U(\\d+)\"\\)\\s+\\(value \"(AND|OR|NOT|XOR|SW|LED)\"\\)");
    std::regex nodeRe("\\(node \\(ref \"U(\\d+)\"\\) \\(pin \"(\\d+)\"\\)\\)");
    auto end = std::sregex_iterator();

    // 元件:ref 还原编号,value 还原类型;坐标按导入顺序排成网格
    int idx = 0;
    for (auto it = std::sregex_iterator(t.begin(), t.end(), compRe); it != end; ++it) {
        Component c;
        c.id = std::stoi((*it)[1]);
        c.type = GateTypeFromName((*it)[2]);
        c.x = 180 + (idx % 3) * 230;
        c.y = 160 + (idx / 3) * 170;
        comps.push_back(c);
        idx++;
    }

    // 节点序列:收集全部 (ref, pin),相邻两个节点拼成一根连线。
    // 我们导出时每个网络正好 2 节点;别的网表一个网络可能多节点,
    // 相邻配对是一种可行的重建方式(教学从简)
    std::vector<std::pair<int,int>> nodes;
    for (auto it = std::sregex_iterator(t.begin(), t.end(), nodeRe); it != end; ++it)
        nodes.push_back({ std::stoi((*it)[1]), std::stoi((*it)[2]) });
    for (size_t i = 0; i + 1 < nodes.size(); i += 2)
        wires.push_back({ nodes[i].first, nodes[i].second,
                          nodes[i+1].first, nodes[i+1].second });
    return true;
}

// 读文件并解析
inline bool LoadNetlist(const wxString& path,
                        wxVector<Component>& comps, wxVector<Wire>& wires) {
    wxFile f;
    if (!f.Open(path, wxFile::read))
        return false;
    wxString content;
    if (!f.ReadAll(&content, wxConvUTF8))
        return false;
    f.Close();
    return ParseKiCadNetlist(content, comps, wires);
}
