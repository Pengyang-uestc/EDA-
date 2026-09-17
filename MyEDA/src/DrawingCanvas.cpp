#include "DrawingCanvas.h"

DrawingCanvas::DrawingCanvas(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    // 告诉系统:需要重画时请调用 OnPaint
    Bind(wxEVT_PAINT, &DrawingCanvas::OnPaint, this);
}

void DrawingCanvas::OnPaint(wxPaintEvent&)
{
    // wxPaintDC 是"画笔",所有画图操作都通过它
    wxPaintDC dc(this);

    // 先用白色把整块面板刷干净
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();

    // 每隔 25 像素画一个淡灰色网格点(模仿原理图软件的网格)
    dc.SetPen(*wxLIGHT_GREY_PEN);
    wxSize size = GetClientSize();
    for (int x = 0; x < size.x; x += 25)
        for (int y = 0; y < size.y; y += 25)
            dc.DrawPoint(x, y);
}
