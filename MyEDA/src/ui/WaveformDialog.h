#pragma once
#include <wx/wx.h>
#include <wx/timer.h>
#include <algorithm>
#include "editor/DrawingCanvas.h"

// ================================================================
// 仿真波形对话框:把每次 SimTick 采集到的各元件输出值画成时序方波。
// 数据来自 DrawingCanvas::waveHistory(每步一行,0/1),本对话框只负责绘制:
//   · 左侧:轨迹名(位号+型号)
//   · 右侧:数字波形(高电平=1,低电平=0),仿真运行中每 250ms 自动刷新
// 菜单入口:仿真(S) → 仿真波形(W)...,快捷键 F7。
// 绘制靠 Bind(wxEVT_PAINT, ...) 接上 OnPaint——两个都要有,窗口才画得出东西
// (test_canvas.cpp 里有对应的渲染检查)。
// ================================================================
class WaveformDialog : public wxDialog {
public:
    WaveformDialog(wxWindow* parent, DrawingCanvas* cv)
        : wxDialog(parent, wxID_ANY, "仿真波形 — 时序图",
                   wxDefaultPosition, wxSize(980, 560),
                   wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
          canvas(cv) {
        SetBackgroundStyle(wxBG_STYLE_PAINT);            // 全部内容自己画,不让系统先擦背景(防闪烁)
        Bind(wxEVT_PAINT, &WaveformDialog::OnPaint, this);
        refreshTimer.Bind(wxEVT_TIMER, [this](wxTimerEvent&) { Refresh(); });
        refreshTimer.Start(250);
        Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent&) {
            refreshTimer.Stop();
            Destroy();
        });
    }

private:
    // 版面参数(逻辑像素):左侧名字列宽、波形区上留白、每一步的宽度、页边距
    enum { NameW = 150, Top = 44, StepW = 12, Margin = 12 };
    static const wxColour& TraceColour() { static wxColour c(15, 118, 110); return c; }

    // 行高随轨迹数收缩,保证所有轨迹都落在窗口里(轨迹少时不超过 34)
    int RowHeight() const {
        const int traces = (int)canvas->waveNames.size();
        const int room = GetClientSize().y - Top - 24;      // 底部留给刻度
        return std::min(34, std::max(14, traces > 0 ? room / traces : 34));
    }

    // 第 t 步、第 r 条轨迹的电平。下标越界(电路在两次采样之间变过)时按 0 处理,不越界访问
    int Sample(int t, int r) const {
        const auto& hist = canvas->waveHistory;
        if (t < 0 || t >= (int)hist.size() || r < 0 || r >= (int)hist[t].size()) return 0;
        return hist[t][r];
    }

    void OnPaint(wxPaintEvent&) {
        wxPaintDC dc(this);
        const wxSize size = GetClientSize();
        dc.SetBackground(*wxWHITE_BRUSH);
        dc.Clear();

        dc.SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "微软雅黑"));
        dc.SetTextForeground(*wxBLACK);
        dc.DrawText("仿真波形:每一格 = 一次仿真步进,高电平 = 1,低电平 = 0", Margin, 10);

        const auto& names = canvas->waveNames;
        const auto& hist  = canvas->waveHistory;
        if (hist.empty() || names.empty()) {
            dc.SetTextForeground(wxColour(120, 120, 120));
            dc.DrawText("还没有波形数据:按 F5 开始仿真(F6 停止)。时钟源、D 触发器电路的波形最明显。",
                        Margin, Top + 16);
            return;
        }

        const int rowH  = RowHeight();
        const int total = (int)hist.size();
        int visSteps = (size.x - NameW - Margin) / StepW;   // 一屏能放下的步数,放不下时只画最近的
        if (visSteps < 1) visSteps = 1;
        const int from  = std::max(0, total - visSteps);
        const int steps = total - from;

        // 左侧:轨迹名 + 行分隔线
        dc.SetPen(wxPen(wxColour(225, 232, 230), 1));
        for (int r = 0; r < (int)names.size(); r++) {
            const int y0 = Top + r * rowH;
            dc.DrawLine(Margin, y0, size.x - Margin, y0);
            dc.DrawText(names[r], Margin, y0 + std::max(0, (rowH - 17) / 2));
        }

        // 右侧:数字方波。水平段一直画到本步的右边界,电平在边界处变化就画一条竖线
        dc.SetPen(wxPen(TraceColour(), 2));
        for (int r = 0; r < (int)names.size(); r++) {
            const int y0 = Top + r * rowH;
            const int hi = y0 + std::max(3, rowH / 5);       // 高电平线
            const int lo = y0 + rowH - std::max(3, rowH / 5); // 低电平线
            int prevX = NameW;
            int prevY = Sample(from, r) ? hi : lo;
            for (int t = 1; t < steps; t++) {
                const int x = NameW + t * StepW;
                const int y = Sample(from + t, r) ? hi : lo;
                dc.DrawLine(prevX, prevY, x, prevY);          // 上一个电平延续到这一步的边界
                if (prevY != y) dc.DrawLine(x, prevY, x, y);  // 跳变沿
                prevX = x; prevY = y;
            }
            dc.DrawLine(prevX, prevY, prevX + StepW, prevY);  // 最后一步也占满一格
        }

        // 底部:步序号刻度
        const int axisY = Top + (int)names.size() * rowH + 4;
        dc.SetTextForeground(wxColour(120, 120, 120));
        dc.SetPen(*wxGREY_PEN);
        for (int t = 0; t < steps; t += 10) {
            const int x = NameW + t * StepW;
            dc.DrawLine(x, axisY - 2, x, axisY + 2);
            dc.DrawText(wxString::Format("%d", from + t), x, axisY + 4);
        }
    }

    DrawingCanvas* canvas;
    wxTimer refreshTimer;
};
