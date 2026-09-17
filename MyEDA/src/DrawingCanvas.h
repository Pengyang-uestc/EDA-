#pragma once
#include <wx/wx.h>
#include <wx/vector.h>
#include "Component.h"
#include "CircuitFile.h"
#include "NetlistExport.h"
#include "Simulation.h"

// 绘图区:显示网格和所有已放置的元件。
// 交互约定:在左边树/工具栏选中元件类型 → 在这里点一下就放一个;
// 点中已有元件则选中它,可以拖动,按 Delete 或点菜单删除
class DrawingCanvas : public wxPanel {
public:
    DrawingCanvas(wxWindow* parent);

    void SetPlaceType(GateType t) { placeType = t; wireMode = false; }  // 设置当前要放的元件(会退出连线模式)
    void SetWireMode()             { wireMode = true; wireFromComp = -1; }  // 进入连线模式
    void DeleteSelected();                             // 删除选中元件(编辑菜单/Delete键调用)

    // 保存/打开:真正干活的是 CircuitFile.h 里的函数,这里只负责"拿数据"和"换数据"
    bool SaveFile(const wxString& path);
    bool LoadFile(const wxString& path);
    void NewDocument();                                // 新建:清空一切,回到初始状态
    const wxString& GetFilePath() const { return filePath; }

    // 导出 KiCad 网表(任务5)
    bool ExportNetlist(const wxString& path) { return SaveNetlist(path, components, wires); }

    // 导入 KiCad 网表:反向重建电路(坐标为默认网格排布,开关状态默认关)
    bool ImportNetlist(const wxString& path) {
        if (!LoadNetlist(path, components, wires))
            return false;
        int maxId = 0;
        for (size_t i = 0; i < components.size(); i++)
            if (components[i].id > maxId) maxId = components[i].id;
        nextId = maxId + 1;
        selectedId = -1;
        wireMode = false;
        wireFromComp = -1;
        simRunning = false;
        simOut.clear();
        filePath = "";   // 网表不是工程文件,之后另存为 .eda
        Refresh();
        return true;
    }

    // 逻辑仿真(任务6)
    void StartSim() { simRunning = true; RunSim(); }
    void StopSim()  { simRunning = false; Refresh(); }
    bool IsSimRunning() const { return simRunning; }

private:
    void OnPaint(wxPaintEvent&);
    void OnLeftDown(wxMouseEvent&);
    void OnMotion(wxMouseEvent&);
    void OnLeftUp(wxMouseEvent&);
    void OnKeyDown(wxKeyEvent&);

    int  HitTest(int mx, int my);          // 返回点中的元件编号,没点中返回 -1
    bool HitPin(int mx, int my, int* compId, int* pin);   // 连线模式:命中的引脚
    Component* FindById(int id);           // 按编号找元件,找不到返回 nullptr
    void DrawGate(wxDC& dc, const Component& c);
    void RunSim();                         // 跑一遍仿真并重画
    int  PinSourceValue(int compId, int pin);   // 某输入引脚连到的来源输出值

    wxVector<Component> components;   // 画布上所有元件(核心数据!)
    wxVector<Wire> wires;             // 所有连线
    GateType placeType = GATE_AND;    // 当前放置工具
    int nextId = 1;                   // 下一个元件的编号
    int selectedId = -1;              // 当前选中的元件编号,-1 = 没选中
    int dragOffX = 0, dragOffY = 0;   // 拖动时鼠标相对元件中心的偏移
    bool dragging = false;            // 正在拖动?

    // 仿真状态
    bool simRunning = false;          // 仿真是否运行中
    std::map<int,int> simOut;         // 每个元件的输出值(仿真结果)

    // 连线模式状态:点第一个引脚记下起点,再点第二个引脚完成连线
    bool wireMode = false;
    int  wireFromComp = -1, wireFromPin = -1;   // 已选的起点引脚(-1=还没选)
    wxPoint wireEnd{ 0, 0 };                    // 橡皮筋终点(鼠标当前位置)

    wxString filePath;                          // 当前电路对应的文件(空=还没保存过)
};
