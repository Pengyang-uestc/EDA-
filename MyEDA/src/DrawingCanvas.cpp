#include "DrawingCanvas.h"

DrawingCanvas::DrawingCanvas(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    Bind(wxEVT_PAINT, &DrawingCanvas::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &DrawingCanvas::OnLeftDown, this);
    Bind(wxEVT_MOTION, &DrawingCanvas::OnMotion, this);
    Bind(wxEVT_LEFT_UP, &DrawingCanvas::OnLeftUp, this);
    Bind(wxEVT_KEY_DOWN, &DrawingCanvas::OnKeyDown, this);
}

// ================================================================
// 画图:每次系统要求重画时,把网格 + 所有元件重新画一遍
// ================================================================
void DrawingCanvas::OnPaint(wxPaintEvent&)
{
    wxPaintDC dc(this);

    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();

    // 淡灰色网格点
    dc.SetPen(*wxLIGHT_GREY_PEN);
    wxSize size = GetClientSize();
    for (int x = 0; x < size.x; x += 25)
        for (int y = 0; y < size.y; y += 25)
            dc.DrawPoint(x, y);

    // 把每个元件画出来
    for (size_t i = 0; i < components.size(); i++)
        DrawGate(dc, components[i]);
}

// ================================================================
// 鼠标左键按下:点中元件=选中准备拖动;点空白=放置新元件
// ================================================================
void DrawingCanvas::OnLeftDown(wxMouseEvent& e)
{
    SetFocus();   // 让画布获得键盘焦点,Delete 键才有效

    int id = HitTest(e.GetX(), e.GetY());
    if (id != -1) {
        // 点中已有元件:选中 + 记住鼠标相对元件中心的偏移,开始拖动
        selectedId = id;
        Component* c = FindById(id);
        dragOffX = e.GetX() - c->x;
        dragOffY = e.GetY() - c->y;
        dragging = true;
        CaptureMouse();   // 拖出画布外鼠标消息也归我们,松开才不会丢
    } else {
        // 点在空白处:放一个当前类型的元件
        Component c = { placeType, e.GetX(), e.GetY(), nextId++ };
        components.push_back(c);
        selectedId = -1;
    }
    Refresh();   // 通知系统重画(触发 OnPaint)
}

// 拖动中:元件跟着鼠标走
void DrawingCanvas::OnMotion(wxMouseEvent& e)
{
    if (!dragging)
        return;
    Component* c = FindById(selectedId);
    if (c) {
        c->x = e.GetX() - dragOffX;
        c->y = e.GetY() - dragOffY;
        Refresh();
    }
}

void DrawingCanvas::OnLeftUp(wxMouseEvent& e)
{
    if (dragging) {
        dragging = false;
        ReleaseMouse();
    }
}

// 按 Delete 删除选中元件
void DrawingCanvas::OnKeyDown(wxKeyEvent& e)
{
    if (e.GetKeyCode() == WXK_DELETE)
        DeleteSelected();
    else
        e.Skip();
}

// ================================================================
// 删除选中元件
// ================================================================
void DrawingCanvas::DeleteSelected()
{
    for (size_t i = 0; i < components.size(); i++) {
        if (components[i].id == selectedId) {
            components.erase(components.begin() + i);
            selectedId = -1;
            Refresh();
            return;
        }
    }
}

// ================================================================
// 命中测试:这个点落在哪个元件上?(用包围盒粗略判断)
// ================================================================
int DrawingCanvas::HitTest(int mx, int my)
{
    // 从后往前找:后画的在上面,先点中"最上层"的
    for (int i = (int)components.size() - 1; i >= 0; i--) {
        const Component& c = components[i];
        if (mx >= c.x - 25 && mx <= c.x + 25 && my >= c.y - 25 && my <= c.y + 25)
            return c.id;
    }
    return -1;
}

Component* DrawingCanvas::FindById(int id)
{
    for (size_t i = 0; i < components.size(); i++)
        if (components[i].id == id)
            return &components[i];
    return nullptr;
}

// ================================================================
// 画一个门电路符号(与门=D形,或门=弯月形,非门=三角+小圆圈)
// 选中状态的元件用蓝色描边
// ================================================================
void DrawingCanvas::DrawGate(wxDC& dc, const Component& c)
{
    dc.SetPen((c.id == selectedId) ? wxPen(*wxBLUE, 2) : wxPen(*wxBLACK, 1));
    dc.SetBrush(*wxWHITE_BRUSH);

    int x = c.x, y = c.y;

    if (c.type == GATE_AND) {
        // 与门:左边一竖 + 上下横线 + 右边半圆,组成"D"形
        dc.DrawLine(x - 20, y - 20, x - 20, y + 20);
        dc.DrawLine(x - 20, y - 20, x, y - 20);
        dc.DrawLine(x - 20, y + 20, x, y + 20);
        dc.DrawArc(x, y + 20, x, y - 20, x, y);
    } else if (c.type == GATE_OR) {
        // 或门:用折线近似弯月形(右边凸出尖角,左边内凹)
        wxPoint pts[6] = {
            wxPoint(x - 22, y - 18),   // 左上
            wxPoint(x + 8,  y - 18),   // 上边
            wxPoint(x + 20, y),        // 右尖
            wxPoint(x + 8,  y + 18),   // 下边
            wxPoint(x - 22, y + 18),   // 左下
            wxPoint(x - 12, y),        // 左边中点(往右凹)
        };
        dc.DrawPolygon(6, pts);
    } else {
        // 非门:三角形 + 右边小圆圈
        dc.DrawLine(x - 16, y - 14, x - 16, y + 14);
        dc.DrawLine(x - 16, y - 14, x + 10, y);
        dc.DrawLine(x - 16, y + 14, x + 10, y);
        dc.DrawCircle(x + 15, y, 4);
    }

    // 编号标签,比如 U1
    dc.DrawText(wxString::Format("U%d", c.id), x - 12, y - 36);
}
