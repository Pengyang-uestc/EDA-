#include <wx/wx.h>
#include "MyFrame.h"

class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        wxInitAllImageHandlers();   // 注册 PNG/JPEG 等图片解码器,没有它工具栏图标加载失败
        MyFrame* frame = new MyFrame("工业软件创新训练 - 电路原理图编辑器");
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
