#pragma once
#include <wx/wx.h>
#include <wx/treectrl.h>   // wxTreeCtrl 树控件
#include <wx/listctrl.h>   // wxListCtrl 列表/表格控件
#include "DrawingCanvas.h"
#include "CustomDialog.h"  // 自定义元件对话框

// ============ 菜单 ID ============
enum {
    ID_MENU_NEW = wxID_HIGHEST + 1,
    ID_MENU_OPEN,
    ID_MENU_SAVE,
    ID_MENU_SAVE_AS,
    ID_MENU_EXPORT_NETLIST,
    ID_MENU_IMPORT_NETLIST,
    ID_MENU_UNDO,
    ID_MENU_REDO,
    ID_MENU_CUT,
    ID_MENU_COPY,
    ID_MENU_PASTE,
    ID_MENU_DELETE,
    ID_MENU_SIMULATE_START,
    ID_MENU_SIMULATE_STOP,
    ID_MENU_ABOUT,
    ID_MENU_RESET_VIEW,
    ID_MENU_CUSTOM_GATE,   // 工程菜单:新建自定义元件
    ID_MENU_EXPORT_SCH,    // 文件菜单:导出 KiCad 原理图
};

// ============ 工具栏 ID ============
enum {
    ID_TOOL_SELECT = wxID_HIGHEST + 100,
    ID_TOOL_WIRE,
    ID_TOOL_DELETE,
    ID_TOOL_AND,
    ID_TOOL_OR,
    ID_TOOL_NOT,
    ID_TOOL_SIMULATE,
};

class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title);

    // 打开指定文件(菜单"打开"和启动参数共用)
    void OpenPath(const wxString& path);

private:
    // 创建函数
    void CreateMenuBar();
    void CreateToolBar();
    void CreateStatusBar();
    void CreateClientArea();   // 中间三栏:元件库树 + 绘图区 + 属性表

    // 用户自定义元件:一个定义 = 名字 + 真值表
    struct CustomDef {
        wxString name;
        int truth;
    };
    wxVector<CustomDef> customDefs;      // 用户定义过的所有自定义元件
    wxTreeItemId customRootItem;         // 树里"自定义元件"根节点
    void DefineCustomGate();             // 弹出对话框让用户定义一个新元件
    void RefreshPropertyTable();         // 属性表:显示当前选中元件的属性

    // 中间三栏控件
    wxTreeCtrl* componentTree;   // 左:元件库树
    DrawingCanvas* canvas;       // 中:绘图区
    wxListCtrl* propertyList;    // 右:属性表

    // 事件处理函数
    void OnNew(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);
    void OnExportNetlist(wxCommandEvent& event);
    void OnExportKicadSch(wxCommandEvent& event);
    void OnImportNetlist(wxCommandEvent& event);
    void OnUndo(wxCommandEvent& event);
    void OnRedo(wxCommandEvent& event);
    void OnCut(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnSimulateStart(wxCommandEvent& event);
    void OnSimulateStop(wxCommandEvent& event);
    void OnCustomGate(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);

    // 元件库树事件
    void OnTreeSelect(wxTreeEvent& event);

    // 工具栏事件
    void OnToolSelect(wxCommandEvent& event);
    void OnToolWire(wxCommandEvent& event);
    void OnToolDelete(wxCommandEvent& event);
    void OnToolAnd(wxCommandEvent& event);
    void OnToolOr(wxCommandEvent& event);
    void OnToolNot(wxCommandEvent& event);
    void OnToolSimulate(wxCommandEvent& event);
};
