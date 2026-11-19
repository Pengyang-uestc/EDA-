#pragma once
#include <wx/wx.h>
#include <wx/timer.h>
#include "editor/DrawingCanvas.h"

// ================================================================
// 仿真波形对话框:把每次 SimTick 采集到的各元件输出值画成时序方波。
// 数据来自 DrawingCanvas::waveHistory(每步一行,0/1),本对话框只负责绘制:
//   · 左侧:轨迹名(位号+型号)
//   · 右侧:数字波形(高电平=1,低电平=0),仿真运行中每 250ms 自动刷新
// 菜单入口:仿真(S) → 仿真波形(W)...  (任务 A3:时序仿真波形展示)
// ================================================================
class WaveformDialog : public wxDialog {
public:
    WaveformDialog(wxWindow* parent, DrawingCanvas* cv)
        : wxDialog(parent, wxID_ANY, "仿真波形 — 时序图",
                   wxDefaultPosition, wxSize(980, 560),
                   wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
          canvas(cv) {
        new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS);
        refreshTimer.Bind(wxEVT_TIMER, [this](wxTimerEvent&) { Refresh(); });
        refreshTimer.Start(250);
        Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent&) {
            refreshTimer.Stop();
            Destroy();
        });
    }

private:
    void OnPaint(wxPaintEvent&) {
        wxPaintDC dc(this);
        wxSize size = GetClientSize();
        dc.SetBackground(*wxWHITE_BRUSH);
        dc.Clear();

        const int nameW = 130;          // 左侧名字列宽
        const int rowH   = 30;          // 每条轨迹行高
        const int top    = 40;          // 波形起始 y
        const int stepW  = 12;          // 每步像素宽

        dc.SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "微软雅黑"));
        dc.DrawText("仿真波形(每格 = 一次仿真步进,高电平 = 1,低电平 = 0)", 12, 10);

        const auto& names = canvas->waveNames;
        const auto& hist  = canvas->waveHistory;
        if (hist.empty() || names.empty()) {
            dc.SetTextForeground(wxColour(120, 120, 120));
            dc.DrawText("还没有波形数据:按 F5 启动仿真(时钟源/D 触发器电路效果最佳)", 12, top + 20);
            return;
        }

        // 只画最近能放下的步数
        int visSteps = (size.x - nameW - 20) / stepW;
        if (visSteps < 10) visSteps = 10;
        int from = (int)hist.size() - visSteps;
        if (from < 0) from = 0;
        int steps = (int)hist.size() - from;

        dc.SetTextForeground(*wxBLACK);
        for (size_t r = 0; r < names.size(); r++) {
            int y0 = top + (int)r * rowH;
            dc.DrawLine(nameW - 8, y0 + rowH - 4, size.x - 10, y0 + rowH - 4);   // 行分隔线
            dc.DrawText(names[r], 8, y0 + 8);
        }

        // 波形:逐行画方波
        for (size_t r = 0; r < names.size(); r++) {
            int y0 = top + (int)r * rowH;
            int hi = y0 + 6, lo = y0 + rowH - 10;         // 高/低电平线
            dc.SetPen(wxPen(wxColour(15, 118, 110), 2));
            int prevX = -1, prevV = -1;
            for (int t = 0; t < steps; t++) {
                int v = hist[from + t][r];
                int x = nameW + t * stepW;
                int y = v ? hi : lo;
                if (prevX >= 0) {
                    dc.DrawLine(prevX, prevV, x, prevV);          // 水平段
                    if (prevV != y) dc.DrawLine(x, prevV, x, y);  // 跳变沿
                }
                prevX = x + stepW; prevV = y;
            }
            (void)lo; (void)hi;
        }

        // 底部刻度:步序号
        dc.SetTextForeground(wxColour(120, 120, 120));
        for (int t = 0; t < steps; t += 10) {
            int x = nameW + t * stepW;
            dc.DrawText(wxString::Format("%d", from + t), x, top + (int)names.size() * rowH + 4);
        }
    }

    DrawingCanvas* canvas;
    wxTimer refreshTimer;
};
