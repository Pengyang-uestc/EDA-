#pragma once
#include <wx/wx.h>
#include <wx/vector.h>
#include <map>
#include "Component.h"

// ================================================================
// 逻辑仿真引擎(纯函数,与界面解耦)。
// 算法:迭代传播——
//   1. 初始:开关输出=自己的开关状态,门输出全部=0
//   2. 一轮一轮地:每个门读取输入引脚连到的来源输出值,算出自己的新输出
//   3. 重复"元件数+2"轮,信号一定能从开关传播到最远的灯(组合逻辑无环)
// 返回:每个元件的输出引脚值(compId → 0/1)
// ================================================================

// 沿连线找:某元件某引脚连到谁(返回来源元件编号,-1=没连)
inline int FindSource(const wxVector<Wire>& wires, int compId, int pin) {
    for (size_t i = 0; i < wires.size(); i++) {
        if (wires[i].comp2 == compId && wires[i].pin2 == pin)
            return wires[i].comp1;
    }
    return -1;
}

inline std::map<int,int> SimulateCircuit(const wxVector<Component>& comps,
                                         const wxVector<Wire>& wires) {
    std::map<int,int> out;

    // 第 1 步:初始化
    for (size_t i = 0; i < comps.size(); i++)
        out[comps[i].id] = (comps[i].type == SW_INPUT && comps[i].state) ? 1 : 0;

    // 读某元件某输入引脚当前的值
    struct In {
        std::map<int,int>* out;
        const wxVector<Wire>* wires;
        int operator()(const Component& c, int pin) {
            int src = FindSource(*wires, c.id, pin);
            return (src >= 0) ? (*out)[src] : 0;   // 悬空输入按 0
        }
    } in{ &out, &wires };

    // 第 2 步:迭代传播
    int rounds = (int)comps.size() + 2;
    for (int r = 0; r < rounds; r++) {
        for (size_t i = 0; i < comps.size(); i++) {
            const Component& c = comps[i];
            if (c.type == SW_INPUT || c.type == SW_LED)
                continue;   // 开关是源头,灯只显示,都不产生输出
            int v0 = in(c, 0);
            int res;
            if (c.type == GATE_AND)      res = v0 & in(c, 1);
            else if (c.type == GATE_OR)  res = v0 | in(c, 1);
            else if (c.type == GATE_XOR) res = v0 ^ in(c, 1);
            else if (c.type == GATE_NAND) res = !(v0 & in(c, 1));
            else if (c.type == GATE_NOR)  res = !(v0 | in(c, 1));
            else if (c.type == GATE_CUSTOM) {
                // 自定义元件:查真值表。输入组合编号 = a*2+b,取 truth 的对应二进制位
                int idx = v0 * 2 + in(c, 1);
                res = (c.truth >> idx) & 1;
            }
            else /* GATE_NOT */          res = !v0;
            out[c.id] = res;
        }
    }
    return out;
}

// 指示灯显示的值:沿线找来源元件的输出
inline int LedValue(const Component& led, const wxVector<Wire>& wires,
                    const std::map<int,int>& out) {
    int src = FindSource(wires, led.id, 0);
    if (src < 0) return 0;
    auto it = out.find(src);
    return (it != out.end()) ? it->second : 0;
}
