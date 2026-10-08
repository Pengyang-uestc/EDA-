#include "editor/DrawingCanvas.h"
#include <wx/dcmemory.h>
#include <iostream>

class CanvasTestApp : public wxApp {
public:
    bool OnInit() override { return true; }
};
wxIMPLEMENT_APP_NO_MAIN(CanvasTestApp);

static int failures = 0;
static void Check(const char* name, bool ok) {
    std::cout << (ok ? "[PASS] " : "[FAIL] ") << name << std::endl;
    if (!ok) ++failures;
}
static void Flush(DrawingCanvas* canvas) {
    wxYield();
    canvas->Update();
}
static void Mouse(DrawingCanvas* canvas, wxEventType type, int x, int y) {
    wxMouseEvent event(type);
    event.SetPosition(wxPoint(x, y));
    canvas->GetEventHandler()->ProcessEvent(event);
}
static wxImage Capture(DrawingCanvas* canvas) {
    wxSize size = canvas->GetClientSize();
    wxBitmap bitmap(size.x, size.y);
    wxMemoryDC memory(bitmap);
    wxClientDC screen(canvas);
    memory.Blit(0, 0, size.x, size.y, &screen, 0, 0);
    memory.SelectObject(wxNullBitmap);
    return bitmap.ConvertToImage();
}
static int Differences(const wxImage& a, const wxImage& b, wxRect ignored = wxRect()) {
    int count = 0;
    for (int y = 0; y < a.GetHeight(); ++y)
        for (int x = 0; x < a.GetWidth(); ++x) {
            if (ignored.Contains(x, y)) continue;
            if (a.GetRed(x,y) != b.GetRed(x,y) || a.GetGreen(x,y) != b.GetGreen(x,y) || a.GetBlue(x,y) != b.GetBlue(x,y)) ++count;
        }
    return count;
}
int main(int argc, char** argv) {
    if (!wxEntryStart(argc, argv) || !wxTheApp->CallOnInit()) return 2;
    auto frame = new wxFrame(nullptr, wxID_ANY, "MyEDA canvas regression checks", wxPoint(50,50), wxSize(850,650));
    auto canvas = new DrawingCanvas(frame);
    frame->Show();
    frame->Raise();
    Flush(canvas);
    canvas->SetPlaceType(SW_INPUT);
    Mouse(canvas, wxEVT_LEFT_DOWN, 200, 250);
    canvas->SetWireMode();
    Flush(canvas);
    auto baseline = Capture(canvas);
    Mouse(canvas, wxEVT_LEFT_DOWN, 222, 250);
    Flush(canvas);
    Check("first wire click does not draw towards stale origin", Differences(baseline, Capture(canvas), wxRect(218,246,9,9)) == 0);
    Mouse(canvas, wxEVT_MOTION, 500, 400);
    Flush(canvas);
    Check("moving mouse displays wire preview", Differences(baseline, Capture(canvas)) > 20);
    wxKeyEvent escape(wxEVT_KEY_DOWN);
    escape.m_keyCode = WXK_ESCAPE;
    canvas->GetEventHandler()->ProcessEvent(escape);
    Flush(canvas);
    Check("Escape clears preview pixels", Differences(baseline, Capture(canvas)) == 0);
    Mouse(canvas, wxEVT_MOTION, 600, 450);
    Flush(canvas);
    Check("Escape keeps preview cancelled on later mouse movement", Differences(baseline, Capture(canvas)) == 0);
    Mouse(canvas, wxEVT_LEFT_DOWN, 222, 250);
    Mouse(canvas, wxEVT_MOTION, 500, 400);
    Flush(canvas);
    canvas->SetPlaceType(GATE_AND);
    Flush(canvas);
    Check("switching tools clears preview immediately", Differences(baseline, Capture(canvas)) == 0);
    canvas->SetWireMode();
    Mouse(canvas, wxEVT_LEFT_DOWN, 222, 250);
    const wxPoint endpoints[] = {{80,80}, {600,80}, {80,500}, {600,500}};
    bool noTrails = true;
    for (const auto& point : endpoints) {
        Mouse(canvas, wxEVT_MOTION, point.x, point.y);
        Flush(canvas);
        auto partial = Capture(canvas);
        canvas->Refresh(false);
        Flush(canvas);
        noTrails = noTrails && Differences(partial, Capture(canvas)) == 0;
    }
    Check("partial refresh matches full repaint in all directions", noTrails);
    canvas->GetEventHandler()->ProcessEvent(escape);
    canvas->SetPlaceType(GATE_AND);
    Mouse(canvas, wxEVT_LEFT_DOWN, 480, 300);
    Flush(canvas);
    auto beforeWire = Capture(canvas);
    canvas->SetWireMode();
    Mouse(canvas, wxEVT_LEFT_DOWN, 222, 250);
    Mouse(canvas, wxEVT_LEFT_DOWN, 460, 290);
    Flush(canvas);
    auto afterWire = Capture(canvas);
    Check("completed wire is painted into scene", Differences(beforeWire, afterWire) > 20);
    canvas->Undo();
    Flush(canvas);
    Check("undo invalidates cached wires", Differences(beforeWire, Capture(canvas)) == 0);
    canvas->Redo();
    Flush(canvas);
    Check("redo restores cached wires", Differences(afterWire, Capture(canvas)) == 0);
    const auto originalSize = frame->GetSize();
    frame->SetSize(originalSize + wxSize(60,40));
    Flush(canvas);
    frame->SetSize(originalSize);
    Flush(canvas);
    Check("resize preserves scene after rebuilding buffer", Differences(afterWire, Capture(canvas)) == 0);
    frame->Destroy();
    wxYield();
    wxTheApp->OnExit();
    wxEntryCleanup();
    return failures ? 1 : 0;
}
