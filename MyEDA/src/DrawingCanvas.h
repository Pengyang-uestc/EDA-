#pragma once
#include <wx/wx.h>

// 绘图区:以后元件画在这块面板上,目前先画出网格点
class DrawingCanvas : public wxPanel {
public:
    DrawingCanvas(wxWindow* parent);

private:
    void OnPaint(wxPaintEvent& event);
};
