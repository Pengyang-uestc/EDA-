#pragma once
#include <wx/wx.h>
#include <wx/vector.h>
#include "Component.h"

// 绘图区:显示网格和所有已放置的元件。
// 交互约定:在左边树/工具栏选中元件类型 → 在这里点一下就放一个;
// 点中已有元件则选中它,可以拖动,按 Delete 或点菜单删除
class DrawingCanvas : public wxPanel {
public:
    DrawingCanvas(wxWindow* parent);

    void SetPlaceType(GateType t) { placeType = t; }   // 设置当前要放的元件
    void DeleteSelected();                             // 删除选中元件(编辑菜单/Delete键调用)

private:
    void OnPaint(wxPaintEvent&);
    void OnLeftDown(wxMouseEvent&);
    void OnMotion(wxMouseEvent&);
    void OnLeftUp(wxMouseEvent&);
    void OnKeyDown(wxKeyEvent&);

    int  HitTest(int mx, int my);          // 返回点中的元件编号,没点中返回 -1
    Component* FindById(int id);           // 按编号找元件,找不到返回 nullptr
    void DrawGate(wxDC& dc, const Component& c);

    wxVector<Component> components;   // 画布上所有元件(核心数据!)
    GateType placeType = GATE_AND;    // 当前放置工具
    int nextId = 1;                   // 下一个元件的编号
    int selectedId = -1;              // 当前选中的元件编号,-1 = 没选中
    int dragOffX = 0, dragOffY = 0;   // 拖动时鼠标相对元件中心的偏移
    bool dragging = false;            // 正在拖动?
};
