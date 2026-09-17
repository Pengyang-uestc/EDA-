#pragma once
#include <wx/wx.h>

// 元件类型(元件库注册表:新增自定义元件,在 4 个"注册点"各加一行——
// ①这里的枚举 ②GateTypeName/FromName 名字表 ③DrawGate 绘制 ④SimulateCircuit 仿真逻辑)
enum GateType {
    GATE_AND,
    GATE_OR,
    GATE_NOT,
    GATE_XOR,
    GATE_NAND,   // 与非 = 与门+非门(自定义扩展示例)
    GATE_NOR,    // 或非 = 或门+非门
    SW_INPUT,   // 开关:用鼠标点击切换 0/1,输出引脚
    SW_LED,     // 指示灯:亮=1 灭=0,输入引脚
};

// 画布上的一个元件:类型 + 位置 + 编号 + 状态。
// 以后会加引脚、连接关系等,画图/保存/网表都从这个数据出发——
// 记住原则:数据是核心,界面只是把数据"画"出来
struct Component {
    GateType type;
    int x, y;   // 元件中心在画布上的坐标
    int id;     // 全局编号 1,2,3...(网表里叫 U1、U2...)
    bool state = false;   // 开关的开/关(其他元件不用)
};

// 一根连线:起点(元件编号+引脚号) → 终点(元件编号+引脚号)
struct Wire {
    int comp1, pin1;
    int comp2, pin2;
};

// 引脚编号约定:
//   与门/或门/异或门/与非/或非: 0=上输入 1=下输入 2=输出
//   非门: 0=输入 1=输出
//   开关/指示灯: 0(开关=输出,指示灯=输入)
inline int GatePinCount(const Component& c) {
    if (c.type == SW_INPUT || c.type == SW_LED) return 1;
    if (c.type == GATE_NOT) return 2;
    return 3;
}

inline wxPoint GetPinPos(const Component& c, int pin) {
    if (c.type == GATE_AND) {
        if (pin == 0) return wxPoint(c.x - 20, c.y - 10);
        if (pin == 1) return wxPoint(c.x - 20, c.y + 10);
        return wxPoint(c.x + 20, c.y);
    }
    if (c.type == GATE_NAND) {   // 与非门:输入同与门,输出在小圆圈右侧
        if (pin == 0) return wxPoint(c.x - 20, c.y - 10);
        if (pin == 1) return wxPoint(c.x - 20, c.y + 10);
        return wxPoint(c.x + 29, c.y);
    }
    if (c.type == GATE_NOR) {    // 或非门:输入同或门,输出在小圆圈右侧
        if (pin == 0) return wxPoint(c.x - 22, c.y - 9);
        if (pin == 1) return wxPoint(c.x - 22, c.y + 9);
        return wxPoint(c.x + 29, c.y);
    }
    if (c.type == GATE_OR || c.type == GATE_XOR) {
        if (pin == 0) return wxPoint(c.x - 22, c.y - 9);
        if (pin == 1) return wxPoint(c.x - 22, c.y + 9);
        return wxPoint(c.x + 20, c.y);
    }
    if (c.type == GATE_NOT) {
        if (pin == 0) return wxPoint(c.x - 16, c.y);
        return wxPoint(c.x + 19, c.y);
    }
    if (c.type == SW_INPUT)  return wxPoint(c.x + 22, c.y);   // 开关:输出在右
    return wxPoint(c.x - 20, c.y);                            // 指示灯:输入在左
}

// 给树控件挂的数据:点哪个树节点,就能查出对应哪种门
#include <wx/treectrl.h>
class GateItemData : public wxTreeItemData {
public:
    GateItemData(GateType t) : type(t) {}
    GateType type;
};
