// 仿真引擎单元测试:不弹窗口,直接验证门逻辑(命令行输出)
#include <iostream>
#include "Simulation.h"
#include "CircuitFile.h"

int failures = 0;
void Check(const char* name, int got, int want) {
    std::cout << (got == want ? "[PASS] " : "[FAIL] ") << name
              << "  got=" << got << " want=" << want << "\n";
    if (got != want) failures++;
}

int main() {
    // 与非门:两个开关 + NAND + 灯
    wxVector<Component> comps;
    Component sw1; sw1.type = SW_INPUT; sw1.id = 1;
    Component sw2; sw2.type = SW_INPUT; sw2.id = 2;
    Component nd;  nd.type = GATE_NAND; nd.id = 3;
    Component led; led.type = SW_LED;   led.id = 4;
    comps.push_back(sw1); comps.push_back(sw2); comps.push_back(nd); comps.push_back(led);

    wxVector<Wire> wires;
    wires.push_back({1, 0, 3, 0});   // SW1 → NAND 输入0
    wires.push_back({2, 0, 3, 1});   // SW2 → NAND 输入1
    wires.push_back({3, 2, 4, 0});   // NAND 输出 → 灯

    // 四种输入组合,验证与非门真值表:输出 = !(A且B)
    bool table[4] = {true, true, true, false};   // (0,0)(0,1)(1,0)(1,1) 的期望
    int combo[4][2] = {{0,0},{0,1},{1,0},{1,1}};
    for (int i = 0; i < 4; i++) {
        comps[0].state = combo[i][0] == 1;
        comps[1].state = combo[i][1] == 1;
        auto out = SimulateCircuit(comps, wires);
        std::string name = "NAND(" + std::to_string(combo[i][0]) + "," + std::to_string(combo[i][1]) + ")";
        Check(name.c_str(), out[3], table[i] ? 1 : 0);
    }

    // ============ 自定义元件(真值表驱动)============
    // 用真值表 0b0111 = 7 定义"与非"行为:如果引擎正确,四种组合应与上面的 NAND 完全一致
    comps[2].type = GATE_CUSTOM;
    comps[2].customName = "MYNAND";
    comps[2].truth = 7;                 // bit0..3 = (0,0)=1 (0,1)=1 (1,0)=1 (1,1)=0
    for (int i = 0; i < 4; i++) {
        comps[0].state = combo[i][0] == 1;
        comps[1].state = combo[i][1] == 1;
        auto out = SimulateCircuit(comps, wires);
        std::string name = "CUSTOM(" + std::to_string(combo[i][0]) + "," + std::to_string(combo[i][1]) + ")";
        Check(name.c_str(), out[3], table[i] ? 1 : 0);
    }

    // 换一张真值表 = 换一种元件:0b1000 = 8 → 只有 (1,1) 输出 1(即与门行为)
    comps[2].truth = 8;
    bool andTable[4] = {false, false, false, true};
    for (int i = 0; i < 4; i++) {
        comps[0].state = combo[i][0] == 1;
        comps[1].state = combo[i][1] == 1;
        auto out = SimulateCircuit(comps, wires);
        std::string name = "CUSTOM_AND(" + std::to_string(combo[i][0]) + "," + std::to_string(combo[i][1]) + ")";
        Check(name.c_str(), out[3], andTable[i] ? 1 : 0);
    }

    // ============ 保存/加载往返(自定义元件也要能存住)============
    {
        wxVector<Component> c2;
        wxVector<Component> c1 = comps;
        wxVector<Wire> w2;
        wxString json = CircuitToJson(c1, wires);
        int next = 1;
        JsonToCircuit(std::string((const char*)json.utf8_str()), c2, w2, next);
        Check("ROUNDTRIP count", (int)c2.size(), (int)c1.size());
        Check("ROUNDTRIP name", c2[2].customName == "MYNAND" ? 1 : 0, 1);
        Check("ROUNDTRIP truth", c2[2].truth, 8);
        Check("ROUNDTRIP type", c2[2].type == GATE_CUSTOM ? 1 : 0, 1);
        Check("ROUNDTRIP wires", (int)w2.size(), (int)wires.size());
    }

    // ============ 解析真实存档文本(含自定义元件)============
    {
        const char* fileText =
            "{\n"
            "  \"components\": [\n"
            "    {\"id\":1,\"type\":\"SW\",\"x\":200,\"y\":250,\"state\":1},\n"
            "    {\"id\":3,\"type\":\"CUSTOM\",\"x\":480,\"y\":340,\"state\":0,\"name\":\"MYAND\",\"truth\":8},\n"
            "    {\"id\":5,\"type\":\"LED\",\"x\":700,\"y\":340,\"state\":0}\n"
            "  ],\n"
            "  \"wires\": [\n"
            "    [3,2,5,0]\n"
            "  ]\n"
            "}\n";
        wxVector<Component> pc; wxVector<Wire> pw; int pnext = 0;
        JsonToCircuit(std::string(fileText), pc, pw, pnext);
        Check("PARSE comps", (int)pc.size(), 3);
        Check("PARSE custom type", pc[1].type == GATE_CUSTOM ? 1 : 0, 1);
        Check("PARSE custom name", pc[1].customName == "MYAND" ? 1 : 0, 1);
        Check("PARSE custom truth", pc[1].truth, 8);
        Check("PARSE sw state", pc[0].state ? 1 : 0, 1);
        Check("PARSE nextId", pnext, 6);
    }

    std::cout << (failures == 0 ? "=== ALL PASS ===" : "=== FAILED ===") << "\n";
    return failures;
}
