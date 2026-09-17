#include <wx/wx.h>
#include "MyFrame.h"

class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        MyFrame* frame = new MyFrame("工业软件创新训练 - 电路原理图编辑器");
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
