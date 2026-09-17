#pragma once
#include <wx/wx.h>
#include <wx/vector.h>
#include <wx/file.h>
#include <fstream>
#include <regex>
#include <string>
#include "Component.h"

// ================================================================
// 电路的保存与加载(JSON 文本格式),与界面完全解耦——
// 只有函数,不碰任何窗口,以后网表导出、仿真也都照这个思路分层
// ================================================================

// 元件类型 ↔ 名字(存进文件的是名字,人能看懂)
inline wxString GateTypeName(GateType t) {
    if (t == GATE_AND) return "AND";
    if (t == GATE_OR)  return "OR";
    if (t == GATE_NOT) return "NOT";
    if (t == GATE_XOR) return "XOR";
    if (t == SW_INPUT) return "SW";
    return "LED";
}
inline GateType GateTypeFromName(const std::string& s) {
    if (s == "OR")  return GATE_OR;
    if (s == "NOT") return GATE_NOT;
    if (s == "XOR") return GATE_XOR;
    if (s == "SW")  return SW_INPUT;
    if (s == "LED") return SW_LED;
    return GATE_AND;
}

// 把整份电路拼成 JSON 文本。
// 格式是我们自己定的,结构固定:一个 components 数组 + 一个 wires 数组
inline wxString CircuitToJson(const wxVector<Component>& comps, const wxVector<Wire>& wires) {
    wxString s = "{\n  \"components\": [\n";
    for (size_t i = 0; i < comps.size(); i++) {
        const Component& c = comps[i];
        s += wxString::Format("    {\"id\":%d,\"type\":\"%s\",\"x\":%d,\"y\":%d,\"state\":%d}%s\n",
                              c.id, GateTypeName(c.type), c.x, c.y, c.state ? 1 : 0,
                              (i + 1 < comps.size()) ? "," : "");
    }
    s += "  ],\n  \"wires\": [\n";
    for (size_t i = 0; i < wires.size(); i++) {
        const Wire& w = wires[i];
        s += wxString::Format("    [%d,%d,%d,%d]%s\n",
                              w.comp1, w.pin1, w.comp2, w.pin2,
                              (i + 1 < wires.size()) ? "," : "");
    }
    s += "  ]\n}\n";
    return s;
}

// 解析 JSON:结构固定,用正则表达式把每个元件/连线"抓"出来即可。
// (真实项目会用现成 JSON 库如 nlohmann/json;这里手写最简版便于教学)
inline bool JsonToCircuit(const std::string& text,
                          wxVector<Component>& comps, wxVector<Wire>& wires, int& nextId) {
    comps.clear();
    wires.clear();

    std::regex compRe("\\{\"id\":(\\d+),\"type\":\"(AND|OR|NOT|XOR|SW|LED)\",\"x\":(-?\\d+),\"y\":(-?\\d+),\"state\":(\\d)\\}");
    std::regex wireRe("\\[(\\d+),(\\d+),(\\d+),(\\d+)\\]");
    auto end = std::sregex_iterator();

    int maxId = 0;
    for (auto it = std::sregex_iterator(text.begin(), text.end(), compRe); it != end; ++it) {
        Component c;
        c.id   = std::stoi((*it)[1]);
        c.type = GateTypeFromName((*it)[2]);
        c.x    = std::stoi((*it)[3]);
        c.y    = std::stoi((*it)[4]);
        c.state = ((*it)[5] == "1");
        comps.push_back(c);
        if (c.id > maxId) maxId = c.id;
    }
    for (auto it = std::sregex_iterator(text.begin(), text.end(), wireRe); it != end; ++it) {
        Wire w;
        w.comp1 = std::stoi((*it)[1]);
        w.pin1  = std::stoi((*it)[2]);
        w.comp2 = std::stoi((*it)[3]);
        w.pin2  = std::stoi((*it)[4]);
        wires.push_back(w);
    }
    nextId = maxId + 1;   // 新元件从最大编号+1 开始,保证编号不重复
    return true;
}

// 写文件
inline bool SaveCircuit(const wxString& path,
                        const wxVector<Component>& comps, const wxVector<Wire>& wires) {
    wxFile f;
    if (!f.Open(path, wxFile::write))
        return false;                       // 打不开(路径不对/没权限)
    wxString json = CircuitToJson(comps, wires);
    f.Write(json, wxConvUTF8);              // JSON 里全是英文,UTF-8 最保险
    f.Close();
    return true;
}

// 读文件
inline bool LoadCircuit(const wxString& path,
                        wxVector<Component>& comps, wxVector<Wire>& wires, int& nextId) {
    wxFile f;
    if (!f.Open(path, wxFile::read))
        return false;
    wxString content;
    if (!f.ReadAll(&content, wxConvUTF8))
        return false;
    f.Close();
    JsonToCircuit(std::string((const char*)content.utf8_str()), comps, wires, nextId);
    return true;
}
