#pragma once
#include <wx/wx.h>
#include <wx/treectrl.h>   // wxTreeCtrl 树控件
#include <wx/listctrl.h>   // wxListCtrl 列表/表格控件
#include "DrawingCanvas.h"

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

private:
    // 创建函数
    void CreateMenuBar();
    void CreateToolBar();
    void CreateStatusBar();
    void CreateClientArea();   // 中间三栏:元件库树 + 绘图区 + 属性表

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
    void OnImportNetlist(wxCommandEvent& event);
    void OnUndo(wxCommandEvent& event);
    void OnRedo(wxCommandEvent& event);
    void OnCut(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnSimulateStart(wxCommandEvent& event);
    void OnSimulateStop(wxCommandEvent& event);
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
