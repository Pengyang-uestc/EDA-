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

    // 先画连线(在元件下层)
    for (size_t i = 0; i < wires.size(); i++) {
        Component* a = FindById(wires[i].comp1);
        Component* b = FindById(wires[i].comp2);
        if (!a || !b) continue;
        // 仿真中:高电位的线画红色,低电位画黑色(Logisim 风格)
        if (simRunning && simOut[wires[i].comp1] == 1)
            dc.SetPen(wxPen(*wxRED, 2));
        else
            dc.SetPen(wxPen(*wxBLACK, 2));
        dc.DrawLine(GetPinPos(*a, wires[i].pin1), GetPinPos(*b, wires[i].pin2));
    }

    // 连线模式下的橡皮筋预览:灰色虚线跟着鼠标走
    if (wireMode && wireFromComp != -1) {
        Component* a = FindById(wireFromComp);
        if (a) {
            dc.SetPen(wxPen(*wxLIGHT_GREY, 1, wxPENSTYLE_SHORT_DASH));
            dc.DrawLine(GetPinPos(*a, wireFromPin), wireEnd);
        }
    }

    // 再画元件
    for (size_t i = 0; i < components.size(); i++)
        DrawGate(dc, components[i]);
}

// ================================================================
// 鼠标左键按下:连线模式走引脚逻辑;否则 点中元件=选中拖动,点空白=放置
// ================================================================
void DrawingCanvas::OnLeftDown(wxMouseEvent& e)
{
    SetFocus();   // 让画布获得键盘焦点,Delete 键才有效

    // ----- 仿真运行中:点到开关 = 切换它的开/关,然后重算整个电路 -----
    if (simRunning) {
        int id = HitTest(e.GetX(), e.GetY());
        Component* c = (id != -1) ? FindById(id) : nullptr;
        if (c && c->type == SW_INPUT) {
            c->state = !c->state;   // 切换!
            RunSim();               // 电路变了,重新算一遍
        }
        return;
    }

    // ----- 连线模式 -----
    if (wireMode) {
        int cid, pin;
        if (!HitPin(e.GetX(), e.GetY(), &cid, &pin))
            return;   // 没点到引脚,忽略(不退出连线模式)

        if (wireFromComp == -1) {
            // 第一下:选定起点引脚
            wireFromComp = cid;
            wireFromPin = pin;
        } else {
            // 第二下:完成连线,存入数据
            Wire w = { wireFromComp, wireFromPin, cid, pin };
            wires.push_back(w);
            wireFromComp = -1;   // 可以继续连下一根
        }
        Refresh();
        return;
    }

    // ----- 普通模式 -----
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

// 拖动中:元件跟着鼠标走;连线模式:橡皮筋终点跟着鼠标走
void DrawingCanvas::OnMotion(wxMouseEvent& e)
{
    if (wireMode && wireFromComp != -1) {
        wireEnd = wxPoint(e.GetX(), e.GetY());
        Refresh();
        return;
    }
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
// 删除选中元件(它身上的连线也要一起删,不然连线会"悬空"指向不存在的元件)
// ================================================================
void DrawingCanvas::DeleteSelected()
{
    for (size_t i = 0; i < components.size(); i++) {
        if (components[i].id == selectedId) {
            components.erase(components.begin() + i);

            // 从后往前删连线,避免删除时下标错位
            for (int j = (int)wires.size() - 1; j >= 0; j--) {
                if (wires[j].comp1 == selectedId || wires[j].comp2 == selectedId)
                    wires.erase(wires.begin() + j);
            }
            selectedId = -1;
            Refresh();
            return;
        }
    }
}

// ================================================================
// 命中测试:这个点落在哪个引脚上?(引脚周围 9 像素内算命中)
// ================================================================
bool DrawingCanvas::HitPin(int mx, int my, int* compId, int* pin)
{
    for (int i = (int)components.size() - 1; i >= 0; i--) {
        Component& c = components[i];
        for (int p = 0; p < GatePinCount(c); p++) {
            wxPoint pp = GetPinPos(c, p);
            int dx = mx - pp.x, dy = my - pp.y;
            if (dx * dx + dy * dy <= 9 * 9) {
                *compId = c.id;
                *pin = p;
                return true;
            }
        }
    }
    return false;
}

// ================================================================
// 保存/打开:数据在画布上,文件读写交给 CircuitFile.h
// ================================================================
bool DrawingCanvas::SaveFile(const wxString& path)
{
    if (!SaveCircuit(path, components, wires))
        return false;
    filePath = path;   // 记住存到哪了,下次 Ctrl+S 直接覆盖
    return true;
}

void DrawingCanvas::NewDocument()
{
    components.clear();
    wires.clear();
    nextId = 1;
    selectedId = -1;
    wireMode = false;
    wireFromComp = -1;
    simRunning = false;
    simOut.clear();
    filePath = "";
    Refresh();
}

bool DrawingCanvas::LoadFile(const wxString& path)
{
    if (!LoadCircuit(path, components, wires, nextId))
        return false;
    selectedId = -1;       // 旧电路的选中状态全部作废
    wireMode = false;
    wireFromComp = -1;
    simRunning = false;
    simOut.clear();
    filePath = path;
    Refresh();
    return true;
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

    if (c.type == GATE_AND || c.type == GATE_NAND) {
        // 与门/与非门:D 形;与非门右侧多一个小圆圈
        dc.DrawLine(x - 20, y - 20, x - 20, y + 20);
        dc.DrawLine(x - 20, y - 20, x, y - 20);
        dc.DrawLine(x - 20, y + 20, x, y + 20);
        dc.DrawArc(x, y + 20, x, y - 20, x, y);
        if (c.type == GATE_NAND) dc.DrawCircle(x + 24, y, 4);
    } else if (c.type == GATE_OR || c.type == GATE_XOR || c.type == GATE_NOR) {
        // 或门/异或门:折线弯月形;异或门在左边多画一条凹线
        wxPoint pts[6] = {
            wxPoint(x - 22, y - 18),   // 左上
            wxPoint(x + 8,  y - 18),   // 上边
            wxPoint(x + 20, y),        // 右尖
            wxPoint(x + 8,  y + 18),   // 下边
            wxPoint(x - 22, y + 18),   // 左下
            wxPoint(x - 12, y),        // 左边中点(往右凹)
        };
        dc.DrawPolygon(6, pts);
        if (c.type == GATE_XOR) {
            // 异或门的"第二条输入线":在左边再画一条小凹折线
            wxPoint pts2[4] = {
                wxPoint(x - 30, y - 18),
                wxPoint(x - 22, y - 6),
                wxPoint(x - 22, y + 6),
                wxPoint(x - 30, y + 18),
            };
            dc.DrawLines(4, pts2);
        }
        if (c.type == GATE_NOR) dc.DrawCircle(x + 25, y, 4);
    } else if (c.type == GATE_NOT) {
        // 非门:三角形 + 右边小圆圈
        dc.DrawLine(x - 16, y - 14, x - 16, y + 14);
        dc.DrawLine(x - 16, y - 14, x + 10, y);
        dc.DrawLine(x - 16, y + 14, x + 10, y);
        dc.DrawCircle(x + 15, y, 4);
    } else if (c.type == SW_INPUT) {
        // 开关:圆角方框 + "1"/"0";闭合时底色变绿
        dc.SetBrush(c.state ? wxBrush(wxColour(144, 238, 144)) : *wxWHITE_BRUSH);
        dc.DrawRoundedRectangle(x - 18, y - 14, 36, 28, 4);
        dc.SetTextForeground(*wxBLACK);
        dc.DrawText(c.state ? "1" : "0", x - 5, y - 9);
    } else {
        // 指示灯:圆;仿真中亮=红填充,灭=白
        int v = simRunning ? PinSourceValue(c.id, 0) : 0;
        dc.SetBrush((simRunning && v == 1) ? *wxRED_BRUSH : *wxWHITE_BRUSH);
        dc.DrawCircle(x, y, 14);
        dc.SetTextForeground(*wxBLACK);
    }

    // 编号标签,比如 U1
    dc.DrawText(wxString::Format("U%d", c.id), x - 12, y - 36);
}

// ================================================================
// 仿真:跑一遍并重画
// ================================================================
void DrawingCanvas::RunSim()
{
    simOut = SimulateCircuit(components, wires);
    Refresh();
}

// 某输入引脚连到的来源输出值(画灯的亮灭用)
int DrawingCanvas::PinSourceValue(int compId, int pin)
{
    int src = FindSource(wires, compId, pin);
    if (src < 0) return 0;
    auto it = simOut.find(src);
    return (it != simOut.end()) ? it->second : 0;
}
