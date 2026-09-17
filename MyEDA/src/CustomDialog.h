#pragma once
#include <wx/wx.h>

// ================================================================
// 自定义元件对话框:用户填一个名字,再勾 4 个复选框定义真值表。
// 真值表 = 这个元件"每种输入组合下输出什么"的完整描述,所以
// 2 输入元件只需要 4 个勾选框,就能定义任意 2 输入 1 输出的逻辑!
// 位序约定:第 i 位对应输入组合 (A,B),i = A*2 + B
//   勾选框0 = A=0,B=0   勾选框1 = A=0,B=1
//   勾选框2 = A=1,B=0   勾选框3 = A=1,B=1
// ================================================================
class CustomGateDialog : public wxDialog {
public:
    CustomGateDialog(wxWindow* parent, const wxString& defName = "MYGATE", int defTruth = 0)
        : wxDialog(parent, wxID_ANY, "自定义元件", wxDefaultPosition, wxSize(380, 340))
    {
        wxBoxSizer* top = new wxBoxSizer(wxVERTICAL);

        // ---- 名字 ----
        wxFlexGridSizer* nameRow = new wxFlexGridSizer(2, 8, 8);
        nameRow->Add(new wxStaticText(this, wxID_ANY, "元件名称:"), 0, wxALIGN_CENTER_VERTICAL);
        nameField = new wxTextCtrl(this, wxID_ANY, defName);
        nameRow->Add(nameField, 1, wxEXPAND);
        top->Add(nameRow, 0, wxALL | wxEXPAND, 12);

        // ---- 真值表 ----
        top->Add(new wxStaticText(this, wxID_ANY, "真值表(勾选 = 该输入组合下输出为 1):"),
                 0, wxLEFT | wxRIGHT, 12);
        top->Add(new wxStaticText(this, wxID_ANY, "输入引脚:上=A,下=B;输出引脚在右侧"),
                 0, wxLEFT | wxRIGHT | wxTOP, 12);

        wxFlexGridSizer* grid = new wxFlexGridSizer(4, 12, 6);
        const char* labels[4] = { "A=0, B=0", "A=0, B=1", "A=1, B=0", "A=1, B=1" };
        for (int i = 0; i < 4; i++) {
            grid->Add(new wxStaticText(this, wxID_ANY, labels[i]), 0, wxALIGN_CENTER_VERTICAL);
            outs[i] = new wxCheckBox(this, wxID_ANY, "输出 1");
            outs[i]->SetValue((defTruth >> i) & 1);
            grid->Add(outs[i], 0);
        }
        top->Add(grid, 0, wxALL, 12);

        // 按钮行:不用 CreateStdDialogButtonSizer(它的文字跟随系统语言,
        // 这里固定成中文,演示时更统一)
        wxBoxSizer* btnRow = new wxBoxSizer(wxHORIZONTAL);
        wxButton* okBtn = new wxButton(this, wxID_OK, "确定");
        okBtn->SetDefault();                       // 回车 = 确定
        btnRow->Add(okBtn, 0, wxRIGHT, 8);
        btnRow->Add(new wxButton(this, wxID_CANCEL, "取消"), 0);
        top->Add(btnRow, 0, wxALL | wxALIGN_RIGHT, 12);

        SetSizer(top);
        Center();
    }

    wxString GetGateName() const { return nameField->GetValue(); }

    // 把 4 个勾选框打包成一个整数(位 i = 输入组合 i 的输出)
    int GetTruth() const {
        int t = 0;
        for (int i = 0; i < 4; i++)
            if (outs[i]->IsChecked()) t |= (1 << i);
        return t;
    }

private:
    wxTextCtrl* nameField;
    wxCheckBox* outs[4];
};
