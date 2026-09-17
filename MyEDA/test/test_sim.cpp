// 仿真引擎单元测试:不弹窗口,直接验证门逻辑(命令行输出)
#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include "Simulation.h"
#include "CircuitFile.h"
#include "NetlistExport.h"

int failures = 0;
void Check(const char* name, int got, int want) {
    std::cout << (got == want ? "[PASS] " : "[FAIL] ") << name
              << "  got=" << got << " want=" << want << "\n";
    if (got != want) failures++;
}

int main(int argc, char** argv) {
    // 小工具:test_sim.exe --export-netlist 电路.eda 输出.net
    // 命令行导出网表(不用开界面,方便批量转换/比对)
    if (argc > 3 && std::string(argv[1]) == "--export-netlist") {
        std::ifstream in(argv[2], std::ios::binary);
        if (!in) { std::cout << "打不开文件: " << argv[2] << "\n"; return 1; }
        std::string buf((std::istreambuf_iterator<char>(in)),
                         std::istreambuf_iterator<char>());
        in.close();
        wxVector<Component> cs; wxVector<Wire> ws; int next = 1;
        JsonToCircuit(buf, cs, ws, next);
        wxString nl = ExportKiCadNetlist(cs, ws);
        std::ofstream out(argv[3], std::ios::binary);
        std::string text((const char*)nl.utf8_str());
        out.write(text.data(), (std::streamsize)text.size());
        out.close();
        std::cout << "已导出: " << argv[3] << "  (元件 " << cs.size()
                  << " 个, 网络 " << BuildNets(ws).size() << " 个)\n";
        return 0;
    }

    // 小工具模式:test_sim.exe --parse-netlist 文件.net
    // 直接解析任意 KiCad 网表并打印摘要(用来验证"我们的解析器能读真实网表")
    if (argc > 2 && std::string(argv[1]) == "--parse-netlist") {
        // 用标准库读文件:这个工具程序没有 wxApp,用 wxFile 会触发
        // Debug 版 wxWidgets 的断言弹窗(在控制台程序里会一直卡住)
        std::ifstream in(argv[2], std::ios::binary);
        if (!in) {
            std::cout << "打不开文件: " << argv[2] << "\n";
            return 1;
        }
        std::string buf((std::istreambuf_iterator<char>(in)),
                         std::istreambuf_iterator<char>());
        in.close();
        wxString content = wxString::FromUTF8(buf.c_str());
        std::cout << "读入 " << buf.size() << " 字节\n";
        wxVector<Component> cs; wxVector<Wire> ws;
        ParseKiCadNetlist(content, cs, ws);
        std::cout << "元件 " << cs.size() << " 个:\n";
        for (size_t i = 0; i < cs.size(); i++)
            std::cout << "  U" << cs[i].id << "  型号=" << cs[i].customName
                      << "  类型=" << (int)cs[i].type << "\n";
        std::cout << "重建连线 " << ws.size() << " 根:\n";
        for (size_t i = 0; i < ws.size(); i++)
            std::cout << "  U" << ws[i].comp1 << "[" << ws[i].pin1 << "] -- U"
                      << ws[i].comp2 << "[" << ws[i].pin2 << "]\n";
        return 0;
    }

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

    // ============ 网表:扇出(一个输出带多个输入)必须合并成一个网络 ============
    // 半加器电路:SW_A 同时接到 XOR 和 AND 的输入 → 电气上是同一个网络,
    // 网表里 U1 的引脚 0 只能出现在一个网络里(出现两次 PCB 软件会报错)
    {
        wxVector<Component> hc;
        const char* types[6] = { "SW", "SW", "XOR", "AND", "LED", "LED" };
        for (int i = 0; i < 6; i++) {
            Component c; c.id = i + 1; c.type = GateTypeFromName(types[i]);
            hc.push_back(c);
        }
        wxVector<Wire> hw;
        hw.push_back({1, 0, 3, 0});   // SW_A → XOR 输入0
        hw.push_back({2, 0, 3, 1});   // SW_B → XOR 输入1
        hw.push_back({1, 0, 4, 0});   // SW_A → AND 输入0   ← 扇出
        hw.push_back({2, 0, 4, 1});   // SW_B → AND 输入1   ← 扇出
        hw.push_back({3, 2, 5, 0});   // XOR → 灯
        hw.push_back({4, 2, 6, 0});   // AND → 灯

        std::vector<std::vector<std::pair<int,int>>> nets = BuildNets(hw);
        Check("NETS count", (int)nets.size(), 4);          // 6 根线 → 4 个电气网络
        int big = 0;
        for (size_t i = 0; i < nets.size(); i++)
            if (nets[i].size() == 3) big++;
        Check("NETS with 3 nodes", big, 2);                // 两个扇出网络各有 3 个节点

        // 每个引脚只能属于一个网络
        std::map<std::pair<int,int>, int> pinCount;
        for (size_t i = 0; i < nets.size(); i++)
            for (size_t k = 0; k < nets[i].size(); k++)
                pinCount[nets[i][k]]++;
        int dup = 0;
        for (auto& kv : pinCount) if (kv.second > 1) dup++;
        Check("NETS no duplicated pin", dup, 0);

        // 导出文本里网络数与 BuildNets 一致
        wxString nl = ExportKiCadNetlist(hc, hw);
        std::string nls((const char*)nl.utf8_str());
        int netMarks = 0; size_t p = 0;
        while ((p = nls.find("(net (code", p)) != std::string::npos) { netMarks++; p += 4; }
        Check("NETLIST net count", netMarks, 4);

        // 网表往返:导出 → 导入 → 再导出,两次文本必须完全一致
        wxVector<Component> rc; wxVector<Wire> rw;
        ParseKiCadNetlist(nl, rc, rw);
        wxString nl2 = ExportKiCadNetlist(rc, rw);
        Check("NETLIST roundtrip identical", (nl == nl2) ? 1 : 0, 1);
        Check("NETLIST roundtrip wires", (int)rw.size(), 6);   // 4 个网络链式重连成 6 根线
    }

    // ============ 互操作:解析 KiCad 自己导出的网表 ============
    // 下面的文本是 `kicad-cli sch export netlist --format kicadsexpr` 真实输出的缩略版,
    // 保留了它的两个"坑":每个字段单独一行(要靠 \s+ 匹配)、ref 不叫 U(R1/R2)、
    // 引脚号从 1 开始(读回来要减 1)
    {
        const char* kicadNet =
            "(export\n"
            "\t(version \"E\")\n"
            "\t(design\n"
            "\t\t(source \"ref.kicad_sch\")\n"
            "\t\t(tool \"Eeschema 10.0.6\")\n"
            "\t)\n"
            "\t(components\n"
            "\t\t(comp\n"
            "\t\t\t(ref \"R1\")\n"
            "\t\t\t(value \"10k\")\n"
            "\t\t\t(fields\n"
            "\t\t\t\t(field (name \"Footprint\"))\n"
            "\t\t\t)\n"
            "\t\t\t(libsource (lib \"Device\") (part \"R\") (description \"\"))\n"
            "\t\t\t(tstamps \"11111111-1111-1111-1111-111111111111\")\n"
            "\t\t)\n"
            "\t\t(comp\n"
            "\t\t\t(ref \"R2\")\n"
            "\t\t\t(value \"10k\")\n"
            "\t\t\t(libsource (lib \"Device\") (part \"R\") (description \"\"))\n"
            "\t\t)\n"
            "\t)\n"
            "\t(libparts\n\t)\n"
            "\t(nets\n"
            "\t\t(net\n"
            "\t\t\t(code \"1\")\n"
            "\t\t\t(name \"Net-(R1-Pad1)\")\n"
            "\t\t\t(class \"Default\")\n"
            "\t\t\t(node\n"
            "\t\t\t\t(ref \"R1\")\n"
            "\t\t\t\t(pin \"1\")\n"
            "\t\t\t\t(pintype \"passive\")\n"
            "\t\t\t)\n"
            "\t\t\t(node\n"
            "\t\t\t\t(ref \"R2\")\n"
            "\t\t\t\t(pin \"2\")\n"
            "\t\t\t\t(pintype \"passive\")\n"
            "\t\t\t)\n"
            "\t\t)\n"
            "\t)\n"
            ")\n";
        wxVector<Component> kc; wxVector<Wire> kw;
        ParseKiCadNetlist(wxString::FromUTF8(kicadNet), kc, kw);
        Check("KICAD-IMPORT comps", (int)kc.size(), 2);
        Check("KICAD-IMPORT ids", (kc.size() == 2 && kc[0].id == 1 && kc[1].id == 2) ? 1 : 0, 1);
        Check("KICAD-IMPORT unknown part->custom", kc[0].type == GATE_CUSTOM ? 1 : 0, 1);
        Check("KICAD-IMPORT custom name", kc[0].customName == "10k" ? 1 : 0, 1);
        Check("KICAD-IMPORT wires", (int)kw.size(), 1);
        // 引脚号 1 → 内部 0
        Check("KICAD-IMPORT pin 1based->0based",
              (kw.size() == 1 && kw[0].comp1 == 1 && kw[0].pin1 == 0 &&
               kw[0].comp2 == 2 && kw[0].pin2 == 1) ? 1 : 0, 1);

        // 同号不同前缀(R1 与 C1)不能打架:第二个应自动分配一个空号
        const char* collide = "(export (components (comp (ref \"R1\") (value \"10k\"))\n"
                              " (comp (ref \"C1\") (value \"100n\")))\n"
                              " (nets (net (code \"1\") (name \"N\")\n"
                              "   (node (ref \"R1\") (pin \"2\"))\n"
                              "   (node (ref \"C1\") (pin \"1\")))))\n";
        wxVector<Component> cc; wxVector<Wire> cw;
        ParseKiCadNetlist(wxString::FromUTF8(collide), cc, cw);
        Check("KICAD-IMPORT collision ids distinct",
              (cc.size() == 2 && cc[0].id != cc[1].id) ? 1 : 0, 1);
        Check("KICAD-IMPORT collision wire ok",
              (cw.size() == 1 && cw[0].comp1 == cc[0].id && cw[0].comp2 == cc[1].id) ? 1 : 0, 1);
    }

    std::cout << (failures == 0 ? "=== ALL PASS ===" : "=== FAILED ===") << "\n";
    return failures;
}
