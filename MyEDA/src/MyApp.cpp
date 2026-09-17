#include <wx/wx.h>
#include "MyFrame.h"

class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        wxInitAllImageHandlers();   // 注册 PNG/JPEG 等图片解码器,没有它工具栏图标加载失败
        MyFrame* frame = new MyFrame("工业软件创新训练 - 电路原理图编辑器");
        frame->Show(true);
        // 命令行带文件路径时直接打开(例如: MyEDA.exe demo.eda)
        if (argc > 1)
            frame->OpenPath(argv[1]);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
