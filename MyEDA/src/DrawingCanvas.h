#pragma once
#include <wx/wx.h>
#include <wx/vector.h>
#include <functional>
#include <map>
#include "Component.h"
#include "CircuitFile.h"
#include "NetlistExport.h"
#include "KicadSchExport.h"
#include "Simulation.h"

// 绘图区:显示网格和所有已放置的元件。
// 交互约定:
//   在左边树/工具栏选中元件类型 → 在这里点一下就放一个;
//   点中已有元件则选中它,可以拖动,按 Delete 或点菜单删除;
//   连线模式下点两个引脚完成一根连线;
//   仿真模式下点开关切换 0/1 并实时重算
class DrawingCanvas : public wxPanel {
public:
    DrawingCanvas(wxWindow* parent);

    void SetPlaceType(GateType t) { placeType = t; wireMode = false; }   // 设置当前要放的元件
    void SetCustomPlace(const wxString& name, int truth) {               // 放置用户自定义元件
        placeType = GATE_CUSTOM; placeCustomName = name; placeCustomTruth = truth; wireMode = false;
    }
    void SetWireMode()             { wireMode = true; wireFromComp = -1; }  // 进入连线模式
    void DeleteSelected();                             // 删除选中元件(编辑菜单/Delete键调用)

    // 保存/打开:真正干活的是 CircuitFile.h 里的函数,这里只负责"拿数据"和"换数据"
    bool SaveFile(const wxString& path);
    bool LoadFile(const wxString& path);
    void NewDocument();                                // 新建:清空一切,回到初始状态
    const wxString& GetFilePath() const { return filePath; }

    // "有未保存的修改"标记:关闭/新建/打开前用它提醒用户存盘
    bool IsDirty() const { return dirty; }

    // 自定义元件库(属于文档数据:跟着电路一起存盘)
    wxVector<CustomDef>& CustomDefs() { return customDefs; }
    void RebuildCustomTreeRequested() {}   // 占位:重建树由主窗口负责

    // 自定义元件的管理(配合主窗口的"管理自定义元件"对话框)
    int  CountCustomInstances(const wxString& name) const;
    void UpdateCustomInstances(const wxString& oldName, const wxString& newName, int truth);

    // 导出/导入 KiCad 网表(任务5)
    bool ExportNetlist(const wxString& path) { return SaveNetlist(path, components, wires); }
    // 导出 KiCad 原理图(.kicad_sch):在 KiCad 里打开后按 F8 即可"从原理图更新 PCB"
    bool ExportKicadSch(const wxString& path) {
        wxFile f;
        if (!f.Open(path, wxFile::write)) return false;
        f.Write(ExportKicadSchematic(components, wires), wxConvUTF8);
        f.Close();
        return true;
    }
    bool ImportNetlist(const wxString& path);

    // 逻辑仿真(任务6)
    void StartSim() { simRunning = true; RunSim(); NotifyChanged(); }
    void StopSim()  { simRunning = false; NotifyChanged(); }
    bool IsSimRunning() const { return simRunning; }

    // 编辑操作(撤销/重做/复制/剪切/粘贴)
    bool Undo();
    bool Redo();
    bool CanUndo() const { return undoStack.size() > 0; }
    bool CanRedo() const { return redoStack.size() > 0; }
    void CopySelected();
    void CutSelected();
    void PasteClipboard();
    bool HasClipboard() const { return hasClipboard; }

    // 给右侧属性表用的查询接口(属性表由 MyFrame 更新,画布只提供数据)
    const Component* GetSelected() const;
    int  GetOutputValue(int compId) const;      // 某元件输出引脚的仿真值
    int  GetPinValue(int compId, int pin) const;// 某输入引脚连到的来源值
    void SetSelectionCallback(std::function<void()> cb) { onSelection = cb; }

private:
    void OnPaint(wxPaintEvent&);
    void OnLeftDown(wxMouseEvent&);
    void OnMotion(wxMouseEvent&);
    void OnLeftUp(wxMouseEvent&);
    void OnKeyDown(wxKeyEvent&);

    int  HitTest(int mx, int my);          // 返回点中的元件编号,没点中返回 -1
    bool HitPin(int mx, int my, int* compId, int* pin);   // 连线模式:命中的引脚
    Component* FindById(int id);           // 按编号找元件,找不到返回 nullptr
    void DrawGate(wxDC& dc, const Component& c);
    void RunSim();                         // 跑一遍仿真并重画
    void NotifyChanged();                  // 重画 + 通知属性表刷新
    void PushUndo();                       // 改动前存档(快照式撤销)
    void CommitSnapshot(const wxString& snap);   // 外部算好的快照入栈(拖动用)
    void RestoreSnapshot(const wxString& snap);

    wxVector<Component> components;   // 画布上所有元件(核心数据!)
    wxVector<CustomDef> customDefs;   // 用户自定义元件的库(名字+真值表)
    wxVector<Wire> wires;             // 所有连线
    GateType placeType = GATE_AND;    // 当前放置工具
    wxString placeCustomName;         // 当前要放置的自定义元件名
    int placeCustomTruth = 0;         // 当前要放置的自定义元件真值表
    int nextId = 1;                   // 下一个元件的编号
    int selectedId = -1;              // 当前选中的元件编号,-1 = 没选中
    int dragOffX = 0, dragOffY = 0;   // 拖动时鼠标相对元件中心的偏移
    bool dragging = false;            // 正在拖动?
    wxString dragStartSnapshot;       // 拖动开始前的快照(松手时若真的移动了才入栈)

    // 仿真状态
    bool simRunning = false;          // 仿真是否运行中
    std::map<int,int> simOut;         // 每个元件的输出值(仿真结果)

    // 连线模式状态:点第一个引脚记下起点,再点第二个引脚完成连线
    bool wireMode = false;
    int  wireFromComp = -1, wireFromPin = -1;   // 已选的起点引脚(-1=还没选)
    wxPoint wireEnd{ 0, 0 };                    // 橡皮筋终点(鼠标当前位置)

    // 撤销/重做:存的是电路 JSON 文本快照(JSON 往返已验证无损,所以快照=完整状态)
    wxVector<wxString> undoStack;
    wxVector<wxString> redoStack;

    // 复制/粘贴
    Component clipboard;
    bool hasClipboard = false;

    wxString filePath;                          // 当前电路对应的文件(空=还没保存过)
    bool dirty = false;                         // 有未保存的修改?
    std::function<void()> onSelection;          // 选中/内容变化时通知外面刷新属性表
};
