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
    Bind(wxEVT_MENU, &MyFrame::OnCustomGate,     this, ID_MENU_CUSTOM_GATE);
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
    projectMenu->Append(ID_MENU_CUSTOM_GATE, "新建自定义元件(&C)...", "用真值表定义一个新元件");
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
    componentTree->AppendItem(gates, "异或门 XOR", -1, -1, new GateItemData(GATE_XOR));
    componentTree->AppendItem(gates, "与非门 NAND", -1, -1, new GateItemData(GATE_NAND));
    componentTree->AppendItem(gates, "或非门 NOR", -1, -1, new GateItemData(GATE_NOR));

    wxTreeItemId io = componentTree->AppendItem(root, "输入/输出");
    componentTree->AppendItem(io, "开关", -1, -1, new GateItemData(SW_INPUT));
    componentTree->AppendItem(io, "指示灯", -1, -1, new GateItemData(SW_LED));

    // 自定义元件分支:一个"新建"入口,以后每定义一个就往这里挂一个节点
    customRootItem = componentTree->AppendItem(root, "自定义元件");
    componentTree->AppendItem(customRootItem, "＋ 新建自定义元件...",
                              -1, -1, new GateItemData(GATE_AND, -1, true));

    componentTree->Expand(root);
    componentTree->Expand(gates);
    componentTree->Expand(customRootItem);

    // 点击树节点 → 告诉画布"接下来要放这种元件"
    componentTree->Bind(wxEVT_TREE_SEL_CHANGED, &MyFrame::OnTreeSelect, this);

    // ---------- 中:绘图区(自定义控件) ----------
    canvas = new DrawingCanvas(this);

    // ---------- 右:属性表 ----------
    propertyList = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(230, -1),
                                  wxLC_REPORT | wxLC_SINGLE_SEL);
    propertyList->InsertColumn(0, "属性", wxLIST_FORMAT_LEFT, 90);
    propertyList->InsertColumn(1, "值", wxLIST_FORMAT_LEFT, 130);

    // 画布上选中/改动元件时,回调这里刷新属性表
    canvas->SetSelectionCallback([this]() { RefreshPropertyTable(); });
    RefreshPropertyTable();

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
// 属性表:显示当前选中元件的属性(没选中就显示提示)
// ================================================================
void MyFrame::RefreshPropertyTable()
{
    propertyList->DeleteAllItems();          // 每次全部重建,行数不固定也不怕
    const Component* c = canvas->GetSelected();
    if (!c) {
        long row = propertyList->InsertItem(0, "选中");
        propertyList->SetItem(row, 1, "(未选中元件)：点画布上的元件查看");
        return;
    }

    auto addRow = [this](const wxString& k, const wxString& v) {
        long r = propertyList->InsertItem(propertyList->GetItemCount(), k);
        propertyList->SetItem(r, 1, v);
    };

    addRow("名称", wxString::Format("U%d", c->id));
    addRow("类型", (c->type == GATE_CUSTOM)
                            ? wxString::Format("自定义(%s)", c->customName)
                            : GateTypeName(c->type));
    addRow("位置", wxString::Format("(%d, %d)", c->x, c->y));

    if (c->type == GATE_CUSTOM) {
        // 真值表拆成四行显示,最直观:每种输入组合对应什么输出
        const char* combos[4] = { "真值 00", "真值 01", "真值 10", "真值 11" };
        for (int i = 0; i < 4; i++)
            addRow(combos[i], ((c->truth >> i) & 1) ? "输出 1" : "输出 0");
    }

    if (c->type == SW_INPUT)
        addRow("状态", c->state ? "开(1)" : "关(0)");
    else if (c->type == SW_LED)
        addRow("状态", canvas->IsSimRunning()
                                ? (canvas->GetPinValue(c->id, 0) ? "亮(1)" : "灭(0)")
                                : "未仿真");
    else
        addRow("输出", canvas->IsSimRunning()
                                ? wxString::Format("%d", canvas->GetOutputValue(c->id))
                                : "未仿真");
}

// ================================================================
// 自定义元件:弹对话框→用户填名字勾真值表
// →存进 customDefs →挂到树上(以后可反复放置)
// ================================================================
void MyFrame::DefineCustomGate()
{
    // 预填上一个定义,方便"改改再做一个新的"
    wxString defName = wxString::Format("MYGATE%d", (int)customDefs.size() + 1);
    int defTruth = 0;
    if (customDefs.size() > 0) {
        defName = customDefs.back().name + "2";
        defTruth = customDefs.back().truth;
    }

    CustomGateDialog dlg(this, defName, defTruth);
    if (dlg.ShowModal() != wxID_OK) return;

    CustomDef def;
    def.name = dlg.GetGateName();
    def.name.Trim(true).Trim(false);        // 去掉首尾空格
    if (def.name.empty())
        def.name = wxString::Format("自定义%d", (int)customDefs.size() + 1);
    def.truth = dlg.GetTruth();
    customDefs.push_back(def);

    wxTreeItemId item = componentTree->AppendItem(customRootItem, def.name, -1, -1,
        new GateItemData(GATE_CUSTOM, (int)customDefs.size() - 1));
    componentTree->Expand(customRootItem);
    componentTree->SelectItem(item);   // 选中→触发 OnTreeSelect→进入放置模式
    SetStatusText(wxString::Format("自定义元件「%s」已定义,点击画布放置", def.name), 1);
}

// ================================================================
// 菜单事件处理
// ================================================================
void MyFrame::OnNew(wxCommandEvent&)
{
    // 新建 = 清空画布。目前没有“未保存提醒”,大家自己注意先存盘
    canvas->NewDocument();
    SetStatusText("已新建(画布清空)");
}

void MyFrame::OnOpen(wxCommandEvent&)
{
    wxFileDialog dlg(this, "打开电路文件", wxGetCwd(), "",
                     "MyEDA 电路文件 (*.eda)|*.eda",
                     wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (dlg.ShowModal() != wxID_OK) return;   // 用户点了取消
    OpenPath(dlg.GetPath());
}

void MyFrame::OnSave(wxCommandEvent&)
{
    // 标准做法:从没存过 → 转到另存为;存过 → 直接覆盖
    if (canvas->GetFilePath().empty()) {
        wxCommandEvent e;
        OnSaveAs(e);
        return;
    }
    if (canvas->SaveFile(canvas->GetFilePath()))
        SetStatusText("已保存:" + canvas->GetFilePath());
    else
        SetStatusText("保存失败!");
}

void MyFrame::OnSaveAs(wxCommandEvent&)
{
    wxFileDialog dlg(this, "另存电路文件", wxGetCwd(), "未命名.eda",
                     "MyEDA 电路文件 (*.eda)|*.eda",
                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (dlg.ShowModal() != wxID_OK) return;
    if (canvas->SaveFile(dlg.GetPath()))
        SetStatusText("已保存:" + dlg.GetPath());
    else
        SetStatusText("保存失败!");
}
void MyFrame::OnExportNetlist(wxCommandEvent&)
{
    wxFileDialog dlg(this, "导出 KiCad 网表", wxGetCwd(), "未命名.net",
                     "KiCad 网表 (*.net)|*.net",
                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (dlg.ShowModal() != wxID_OK) return;
    if (canvas->ExportNetlist(dlg.GetPath()))
        SetStatusText("网表已导出:" + dlg.GetPath());
    else
        SetStatusText("导出失败!");
}
void MyFrame::OnImportNetlist(wxCommandEvent&)
{
    // 预填默认名“未命名.net”:刚导出的网表就是这个名,直接点打开即可
    wxFileDialog dlg(this, "导入 KiCad 网表", wxGetCwd(), "未命名.net",
                     "KiCad 网表 (*.net)|*.net",
                     wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (dlg.ShowModal() != wxID_OK) return;
    if (canvas->ImportNetlist(dlg.GetPath()))
        SetStatusText("网表已导入(元件按网格重新排布):" + dlg.GetPath());
    else
        SetStatusText("导入失败!");
}
void MyFrame::OnUndo(wxCommandEvent&)         { SetStatusText(canvas->Undo() ? "已撤销" : "没有可撤销的操作"); }
void MyFrame::OnRedo(wxCommandEvent&)         { SetStatusText(canvas->Redo() ? "已重做" : "没有可重做的操作"); }
void MyFrame::OnCut(wxCommandEvent&)          { canvas->CutSelected(); SetStatusText("已剪切"); }
void MyFrame::OnCopy(wxCommandEvent&)         { canvas->CopySelected(); SetStatusText("已复制"); }
void MyFrame::OnPaste(wxCommandEvent&)        { canvas->PasteClipboard(); SetStatusText("已粘贴"); }
void MyFrame::OnDelete(wxCommandEvent&)       { canvas->DeleteSelected(); SetStatusText("删除选中元件"); }
void MyFrame::OnSimulateStart(wxCommandEvent&){ canvas->StartSim(); SetStatusText("仿真运行中:点击开关切换状态"); }
void MyFrame::OnSimulateStop(wxCommandEvent&) { canvas->StopSim(); SetStatusText("仿真已停止"); }
void MyFrame::OnAbout(wxCommandEvent&)
{
    wxMessageBox("工业软件创新训练 I\n电路原理图编辑器\n\n版本 0.1",
                 "关于", wxOK | wxICON_INFORMATION, this);
}
void MyFrame::OnCustomGate(wxCommandEvent&){ DefineCustomGate(); }
void MyFrame::OnExit(wxCommandEvent&)         { Close(true); }

// 打开文件(菜单和启动参数共用)
void MyFrame::OpenPath(const wxString& path)
{
    if (canvas->LoadFile(path))
        SetStatusText("已打开:" + path);
    else
        SetStatusText("打开失败:" + path);
}

// ================================================================
// 元件库树:点击节点 → 画布切换到对应的放置工具
// ================================================================
void MyFrame::OnTreeSelect(wxTreeEvent& e)
{
    GateItemData* data = (GateItemData*)componentTree->GetItemData(e.GetItem());
    if (!data) {
        // 没挂数据的节点 = 分支节点(例如"基本门电路"),它们只能展开不能放置
        SetStatusText("请展开分支选择具体元件", 1);
        return;
    }

    // 点了"新建自定义元件...":弹对话框让用户定义
    if (data->defineNew) {
        DefineCustomGate();
        return;
    }

    // 点了已定义的自定义元件:进入放置模式
    if (data->type == GATE_CUSTOM && data->customIdx >= 0 && data->customIdx < (int)customDefs.size()) {
        const CustomDef& def = customDefs[data->customIdx];
        canvas->SetCustomPlace(def.name, def.truth);
        SetStatusText("放置自定义元件：" + def.name + ",点击画布放置", 1);
        return;
    }

    canvas->SetPlaceType(data->type);
    wxString name[] = { "与门", "或门", "非门", "异或门", "与非门", "或非门",
                        "自定义", "开关", "指示灯" };
    SetStatusText("放置:" + name[data->type] + ",点击画布放置", 1);
}

// ================================================================
// 工具栏事件处理
// ================================================================
void MyFrame::OnToolSelect(wxCommandEvent&)   { SetStatusText("当前工具：选择", 1); }
void MyFrame::OnToolWire(wxCommandEvent&)     { canvas->SetWireMode(); SetStatusText("连线:先点起点引脚,再点终点引脚", 1); }
void MyFrame::OnToolDelete(wxCommandEvent&)   { SetStatusText("执行：删除", 1); }
void MyFrame::OnToolAnd(wxCommandEvent&)      { canvas->SetPlaceType(GATE_AND); SetStatusText("放置：与门,点击画布放置", 1); }
void MyFrame::OnToolOr(wxCommandEvent&)       { canvas->SetPlaceType(GATE_OR);  SetStatusText("放置：或门,点击画布放置", 1); }
void MyFrame::OnToolNot(wxCommandEvent&)      { canvas->SetPlaceType(GATE_NOT); SetStatusText("放置：非门,点击画布放置", 1); }
void MyFrame::OnToolSimulate(wxCommandEvent&) {
    if (canvas->IsSimRunning()) { canvas->StopSim(); SetStatusText("仿真已停止"); }
    else { canvas->StartSim(); SetStatusText("仿真运行中:点击开关切换状态"); }
}
