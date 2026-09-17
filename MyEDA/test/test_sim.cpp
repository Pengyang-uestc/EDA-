// 仿真引擎单元测试:不弹窗口,直接验证门逻辑(命令行输出)
#include <iostream>
#include "Simulation.h"

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
    std::cout << (failures == 0 ? "=== ALL PASS ===" : "=== FAILED ===") << "\n";
    return failures;
}
