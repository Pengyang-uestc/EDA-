#include "MyFrame.h"
#include <wx/stdpaths.h>
#include <wx/filename.h>

MyFrame::MyFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(1200, 800))
{
    CreateMenuBar();
    CreateToolBar();
    CreateStatusBar();
    CreateClientArea();

    SetStatusText("就绪");

    // ============ 绑定菜单事件 ============
    Bind(wxEVT_MENU, &MyFrame::OnNew,            this, ID_MENU_NEW);
    Bind(wxEVT_MENU, &MyFrame::OnOpen,           this, ID_MENU_OPEN);
    Bind(wxEVT_MENU, &MyFrame::OnSave,           this, ID_MENU_SAVE);
    Bind(wxEVT_MENU, &MyFrame::OnSaveAs,         this, ID_MENU_SAVE_AS);
    Bind(wxEVT_MENU, &MyFrame::OnExportNetlist,  this, ID_MENU_EXPORT_NETLIST);
    Bind(wxEVT_MENU, &MyFrame::OnImportNetlist,  this, ID_MENU_IMPORT_NETLIST);
    Bind(wxEVT_MENU, &MyFrame::OnUndo,           this, ID_MENU_UNDO);
    Bind(wxEVT_MENU, &MyFrame::OnRedo,           this, ID_MENU_REDO);
    Bind(wxEVT_MENU, &MyFrame::OnCut,            this, ID_MENU_CUT);
    Bind(wxEVT_MENU, &MyFrame::OnCopy,           this, ID_MENU_COPY);
    Bind(wxEVT_MENU, &MyFrame::OnPaste,          this, ID_MENU_PASTE);
    Bind(wxEVT_MENU, &MyFrame::OnDelete,         this, ID_MENU_DELETE);
    Bind(wxEVT_MENU, &MyFrame::OnSimulateStart,  this, ID_MENU_SIMULATE_START);
    Bind(wxEVT_MENU, &MyFrame::OnSimulateStop,   this, ID_MENU_SIMULATE_STOP);
    Bind(wxEVT_MENU, &MyFrame::OnAbout,          this, ID_MENU_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit,           this, wxID_EXIT);

    // ============ 绑定工具栏事件 ============
    Bind(wxEVT_TOOL, &MyFrame::OnToolSelect,   this, ID_TOOL_SELECT);
    Bind(wxEVT_TOOL, &MyFrame::OnToolWire,     this, ID_TOOL_WIRE);
    Bind(wxEVT_TOOL, &MyFrame::OnToolDelete,   this, ID_TOOL_DELETE);
    Bind(wxEVT_TOOL, &MyFrame::OnToolAnd,      this, ID_TOOL_AND);
    Bind(wxEVT_TOOL, &MyFrame::OnToolOr,       this, ID_TOOL_OR);
    Bind(wxEVT_TOOL, &MyFrame::OnToolNot,      this, ID_TOOL_NOT);
    Bind(wxEVT_TOOL, &MyFrame::OnToolSimulate, this, ID_TOOL_SIMULATE);
}

// ================================================================
// 创建菜单栏
// ================================================================
void MyFrame::CreateMenuBar()
{
    wxMenuBar* menuBar = new wxMenuBar();

    // ---------- 文件 ----------
    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(ID_MENU_NEW,       "新建(&N)\tCtrl+N",  "新建电路");
    fileMenu->Append(ID_MENU_OPEN,      "打开(&O)\tCtrl+O",  "打开电路文件");
    fileMenu->AppendSeparator();
    fileMenu->Append(ID_MENU_SAVE,      "保存(&S)\tCtrl+S",  "保存电路文件");
    fileMenu->Append(ID_MENU_SAVE_AS,   "另存为(&A)\tCtrl+Shift+S", "另存为");
    fileMenu->AppendSeparator();
    fileMenu->Append(ID_MENU_IMPORT_NETLIST, "导入网表(&I)...", "导入标准网表文件");
    fileMenu->Append(ID_MENU_EXPORT_NETLIST, "导出网表(&E)...", "导出标准网表文件（供 PCB 软件使用）");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT,         "退出(&Q)\tCtrl+Q",  "退出程序");
    menuBar->Append(fileMenu, "文件(&F)");

    // ---------- 编辑 ----------
    wxMenu* editMenu = new wxMenu();
    editMenu->Append(ID_MENU_UNDO,   "撤销(&U)\tCtrl+Z",       "撤销上一步操作");
    editMenu->Append(ID_MENU_REDO,   "重做(&R)\tCtrl+Y",       "重做");
    editMenu->AppendSeparator();
    editMenu->Append(ID_MENU_CUT,    "剪切(&T)\tCtrl+X",       "剪切选中元件");
    editMenu->Append(ID_MENU_COPY,   "复制(&C)\tCtrl+C",       "复制选中元件");
    editMenu->Append(ID_MENU_PASTE,  "粘贴(&P)\tCtrl+V",       "粘贴元件");
    editMenu->Append(ID_MENU_DELETE, "删除(&D)\tDelete",       "删除选中元件");
    menuBar->Append(editMenu, "编辑(&E)");

    // ---------- 工程 ----------
    wxMenu* projectMenu = new wxMenu();
    projectMenu->Append(ID_MENU_NEW, "新建工程(&N)...", "新建工程");
    menuBar->Append(projectMenu, "工程(&P)");

    // ---------- 仿真 ----------
    wxMenu* simulateMenu = new wxMenu();
    simulateMenu->Append(ID_MENU_SIMULATE_START, "开始仿真(&S)\tF5", "开始信号传播仿真");
    simulateMenu->Append(ID_MENU_SIMULATE_STOP,  "停止仿真(&T)\tF6", "停止仿真");
    menuBar->Append(simulateMenu, "仿真(&S)");

    // ---------- 窗口 ----------
    wxMenu* windowMenu = new wxMenu();
    windowMenu->Append(ID_MENU_RESET_VIEW, "复位视图(&R)", "复位画布视图");
    menuBar->Append(windowMenu, "窗口(&W)");

    // ---------- 帮助 ----------
    wxMenu* helpMenu = new wxMenu();
    helpMenu->Append(ID_MENU_ABOUT, "关于(&A)...", "关于本软件");
    menuBar->Append(helpMenu, "帮助(&H)");

    SetMenuBar(menuBar);
}

// ================================================================
// 创建工具栏
// ================================================================
void MyFrame::CreateToolBar()
{
    wxToolBar* toolBar = wxFrame::CreateToolBar(wxTB_HORIZONTAL | wxTB_FLAT | wxTB_TEXT);
    toolBar->SetToolBitmapSize(wxSize(24, 24));

    // 找到 res 目录（跟可执行文件同级）
    wxFileName exeFile(wxStandardPaths::Get().GetExecutablePath());
    wxString resDir = exeFile.GetPath() + "/res/";

    // 加载图标；失败就用空白 24x24 位图兜底
    auto loadBmp = [&](const wxString& name) -> wxBitmap {
        wxBitmap bmp(resDir + name, wxBITMAP_TYPE_PNG);
        if (!bmp.IsOk()) bmp = wxBitmap(24, 24);
        return bmp;
    };

    wxBitmap bmpNew       = loadBmp("new.png");
    wxBitmap bmpOpen      = loadBmp("open.png");
    wxBitmap bmpSave      = loadBmp("save.png");
    wxBitmap bmpSelect    = loadBmp("select.png");
    wxBitmap bmpWire      = loadBmp("wire.png");
    wxBitmap bmpDelete    = loadBmp("delete.png");
    wxBitmap bmpAnd       = loadBmp("and.png");
    wxBitmap bmpOr        = loadBmp("or.png");
    wxBitmap bmpNot       = loadBmp("not.png");
    wxBitmap bmpSimulate  = loadBmp("simulate.png");

    // 文件操作
    toolBar->AddTool(ID_MENU_NEW,    "新建", bmpNew,  "新建电路");
    toolBar->AddTool(ID_MENU_OPEN,   "打开", bmpOpen, "打开电路文件");
    toolBar->AddTool(ID_MENU_SAVE,   "保存", bmpSave, "保存电路文件");
    toolBar->AddSeparator();

    // 编辑工具
    toolBar->AddTool(ID_TOOL_SELECT, "选择", bmpSelect, "选择元件");
    toolBar->AddTool(ID_TOOL_WIRE,   "连线", bmpWire,   "绘制连线");
    toolBar->AddTool(ID_TOOL_DELETE, "删除", bmpDelete, "删除选中元件");
    toolBar->AddSeparator();

    // 元件放置
    toolBar->AddTool(ID_TOOL_AND, "与门", bmpAnd, "放置与门");
    toolBar->AddTool(ID_TOOL_OR,  "或门", bmpOr,  "放置或门");
    toolBar->AddTool(ID_TOOL_NOT, "非门", bmpNot, "放置非门");
    toolBar->AddSeparator();

    // 仿真
    toolBar->AddTool(ID_TOOL_SIMULATE, "仿真", bmpSimulate, "开始/停止仿真");

    toolBar->Realize();
}

// ================================================================
// 创建中间三栏:元件库树 + 绘图区 + 属性表
// ================================================================
void MyFrame::CreateClientArea()
{
    // ---------- 左:元件库树 ----------
    componentTree = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(200, -1),
                                   wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT);
    wxTreeItemId root = componentTree->AddRoot("元件库");

    wxTreeItemId gates = componentTree->AppendItem(root, "基本门电路");
    // 给每个树节点挂上"这是什么门"的数据,点击树时就能查到(见 OnTreeSelect)
    componentTree->AppendItem(gates, "与门 AND", -1, -1, new GateItemData(GATE_AND));
    componentTree->AppendItem(gates, "或门 OR",  -1, -1, new GateItemData(GATE_OR));
    componentTree->AppendItem(gates, "非门 NOT", -1, -1, new GateItemData(GATE_NOT));

    wxTreeItemId io = componentTree->AppendItem(root, "输入/输出");
    componentTree->AppendItem(io, "开关");
    componentTree->AppendItem(io, "指示灯");

    componentTree->Expand(root);
    componentTree->Expand(gates);

    // 点击树节点 → 告诉画布"接下来要放这种元件"
    componentTree->Bind(wxEVT_TREE_SEL_CHANGED, &MyFrame::OnTreeSelect, this);

    // ---------- 中:绘图区(自定义控件) ----------
    canvas = new DrawingCanvas(this);

    // ---------- 右:属性表 ----------
    propertyList = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(220, -1),
                                  wxLC_REPORT | wxLC_SINGLE_SEL);
    propertyList->InsertColumn(0, "属性", wxLIST_FORMAT_LEFT, 100);
    propertyList->InsertColumn(1, "值", wxLIST_FORMAT_LEFT, 110);
    long row = propertyList->InsertItem(0, "名称");
    propertyList->SetItem(row, 1, "(未选中元件)");

    // ---------- 用 sizer 把三块拼起来 ----------
    // Add(控件, 比例, 样式):比例 0 = 保持自己的固定宽度,比例 1 = 分走所有剩余空间
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(componentTree, 0, wxEXPAND);
    sizer->Add(canvas,         1, wxEXPAND);
    sizer->Add(propertyList,   0, wxEXPAND);
    SetSizer(sizer);
}

// ================================================================
// 创建状态栏
// ================================================================
void MyFrame::CreateStatusBar()
{
    wxFrame::CreateStatusBar(2);
    int widths[] = { -1, 200 };
    SetStatusWidths(2, widths);
    SetStatusText("就绪", 0);
    SetStatusText("工具：无", 1);
}

// ================================================================
// 菜单事件处理
// ================================================================
void MyFrame::OnNew(wxCommandEvent&)          { SetStatusText("新建电路"); }
void MyFrame::OnOpen(wxCommandEvent&)         { SetStatusText("打开文件"); }
void MyFrame::OnSave(wxCommandEvent&)         { SetStatusText("保存文件"); }
void MyFrame::OnSaveAs(wxCommandEvent&)       { SetStatusText("另存为"); }
void MyFrame::OnExportNetlist(wxCommandEvent&){ SetStatusText("导出网表"); }
void MyFrame::OnImportNetlist(wxCommandEvent&){ SetStatusText("导入网表"); }
void MyFrame::OnUndo(wxCommandEvent&)         { SetStatusText("撤销"); }
void MyFrame::OnRedo(wxCommandEvent&)         { SetStatusText("重做"); }
void MyFrame::OnCut(wxCommandEvent&)          { SetStatusText("剪切"); }
void MyFrame::OnCopy(wxCommandEvent&)         { SetStatusText("复制"); }
void MyFrame::OnPaste(wxCommandEvent&)        { SetStatusText("粘贴"); }
void MyFrame::OnDelete(wxCommandEvent&)       { canvas->DeleteSelected(); SetStatusText("删除选中元件"); }
void MyFrame::OnSimulateStart(wxCommandEvent&){ SetStatusText("开始仿真"); }
void MyFrame::OnSimulateStop(wxCommandEvent&) { SetStatusText("停止仿真"); }
void MyFrame::OnAbout(wxCommandEvent&)
{
    wxMessageBox("工业软件创新训练 I\n电路原理图编辑器\n\n版本 0.1",
                 "关于", wxOK | wxICON_INFORMATION, this);
}
void MyFrame::OnExit(wxCommandEvent&)         { Close(true); }

// ================================================================
// 元件库树:点击节点 → 画布切换到对应的放置工具
// ================================================================
void MyFrame::OnTreeSelect(wxTreeEvent& e)
{
    GateItemData* data = (GateItemData*)componentTree->GetItemData(e.GetItem());
    if (!data) {
        SetStatusText("该元件还没实现,先试试基本门电路", 1);
        return;
    }

    canvas->SetPlaceType(data->type);
    wxString name[] = { "与门", "或门", "非门" };
    SetStatusText("放置:" + name[data->type] + ",点击画布放置", 1);
}

// ================================================================
// 工具栏事件处理
// ================================================================
void MyFrame::OnToolSelect(wxCommandEvent&)   { SetStatusText("当前工具：选择", 1); }
void MyFrame::OnToolWire(wxCommandEvent&)     { SetStatusText("当前工具：连线", 1); }
void MyFrame::OnToolDelete(wxCommandEvent&)   { SetStatusText("执行：删除", 1); }
void MyFrame::OnToolAnd(wxCommandEvent&)      { canvas->SetPlaceType(GATE_AND); SetStatusText("放置：与门,点击画布放置", 1); }
void MyFrame::OnToolOr(wxCommandEvent&)       { canvas->SetPlaceType(GATE_OR);  SetStatusText("放置：或门,点击画布放置", 1); }
void MyFrame::OnToolNot(wxCommandEvent&)      { canvas->SetPlaceType(GATE_NOT); SetStatusText("放置：非门,点击画布放置", 1); }
void MyFrame::OnToolSimulate(wxCommandEvent&) { SetStatusText("仿真切换", 1); }
