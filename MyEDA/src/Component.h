#pragma once
#include <wx/wx.h>

// 元件类型(任务3的元件库,先支持三种基本门)
enum GateType {
    GATE_AND,
    GATE_OR,
    GATE_NOT,
};

// 画布上的一个元件:类型 + 位置 + 编号。
// 以后会加引脚、连接关系等,画图/保存/网表都从这个数据出发——
// 记住原则:数据是核心,界面只是把数据"画"出来
struct Component {
    GateType type;
    int x, y;   // 元件中心在画布上的坐标
    int id;     // 全局编号 1,2,3...(网表里叫 U1、U2...)
};

// 一根连线:起点(元件编号+引脚号) → 终点(元件编号+引脚号)
struct Wire {
    int comp1, pin1;
    int comp2, pin2;
};

// 引脚编号约定:与门/或门 0=上输入 1=下输入 2=输出;非门 0=输入 1=输出
inline wxPoint GetPinPos(const Component& c, int pin) {
    if (c.type == GATE_AND) {
        if (pin == 0) return wxPoint(c.x - 20, c.y - 10);
        if (pin == 1) return wxPoint(c.x - 20, c.y + 10);
        return wxPoint(c.x + 20, c.y);
    }
    if (c.type == GATE_OR) {
        if (pin == 0) return wxPoint(c.x - 22, c.y - 9);
        if (pin == 1) return wxPoint(c.x - 22, c.y + 9);
        return wxPoint(c.x + 20, c.y);
    }
    // GATE_NOT
    if (pin == 0) return wxPoint(c.x - 16, c.y);
    return wxPoint(c.x + 19, c.y);
}

// 给树控件挂的数据:点哪个树节点,就能查出对应哪种门
#include <wx/treectrl.h>
class GateItemData : public wxTreeItemData {
public:
    GateItemData(GateType t) : type(t) {}
    GateType type;
};
