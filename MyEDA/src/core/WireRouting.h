#pragma once
#include <wx/gdicmn.h>
#include <wx/vector.h>
#include <cmath>
#include <algorithm>
constexpr int CircuitGrid = 10;
inline wxPoint SnapGrid(const wxPoint& p) {
    return wxPoint((int)std::round((double)p.x / CircuitGrid) * CircuitGrid,
                   (int)std::round((double)p.y / CircuitGrid) * CircuitGrid);
}
inline wxVector<wxPoint> BuildWirePath(wxPoint start, const wxVector<wxPoint>& bends, wxPoint end) {
    wxVector<wxPoint> path;
    path.push_back(SnapGrid(start));
    auto append = [&](wxPoint point) {
        point = SnapGrid(point);
        wxPoint last = path.back();
        if (last == point) return;
        if (last.x != point.x && last.y != point.y)
            path.push_back(wxPoint(point.x, last.y));
        path.push_back(point);
    };
    // Use a short departure run for an automatic route, rather than stacking
    // every vertical segment directly over the target pins.
    const wxPoint target = SnapGrid(end);
    const wxPoint source = path.front();
    if (bends.empty() && source.x != target.x && source.y != target.y) {
        const int dx = std::abs(target.x - source.x);
        const int dy = std::abs(target.y - source.y);
        const int run = std::min(dx / 2, std::max(CircuitGrid * 2, dy / 2));
        const int direction = target.x > source.x ? 1 : -1;
        const int channel = SnapGrid(wxPoint(source.x + direction * run, 0)).x;
        append(wxPoint(channel, source.y));
        append(wxPoint(channel, target.y));
    }
    for (const auto& point : bends) append(point);
    append(end);
    return path;
}
