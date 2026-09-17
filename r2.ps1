Add-Type -AssemblyName System.Windows.Forms,System.Drawing
Add-Type @"
using System;
using System.Runtime.InteropServices;
public class W {
    [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
    [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr h, int cmd);
    [DllImport("user32.dll")] public static extern bool SetWindowPos(IntPtr h, IntPtr a, int x, int y, int cw, int ch, uint f);
    [DllImport("user32.dll")] public static extern IntPtr PostMessage(IntPtr h, uint m, IntPtr w, IntPtr l);
    [DllImport("user32.dll")] public static extern bool SetCursorPos(int x, int y);
    [DllImport("user32.dll")] public static extern void mouse_event(uint f, int x, int y, uint d, UIntPtr e);
}
"@
function RealClick($x, $y) {
    [W]::SetCursorPos($x, $y) | Out-Null
    Start-Sleep -m 350
    [W]::mouse_event(2, 0, 0, 0, [UIntPtr]::Zero)
    Start-Sleep -m 80
    [W]::mouse_event(4, 0, 0, 0, [UIntPtr]::Zero)
    Start-Sleep -m 900
}
$proc = Get-Process MyEDA -ErrorAction Stop
$h = $proc.MainWindowHandle
[W]::ShowWindow($h, 9) | Out-Null
[W]::SetWindowPos($h, [IntPtr]::Zero, 100, 100, 1200, 830, 0x0040) | Out-Null
[W]::SetForegroundWindow($h) | Out-Null
Start-Sleep -m 600
# 1. 导出:6004 → 对话框 → 真点保存按钮(系统对话框居中,按钮位置固定)
[W]::PostMessage($h, 0x0111, [IntPtr]6004, [IntPtr]0) | Out-Null
Start-Sleep -m 1500
RealClick 992 824
Write-Host "export-1 done"
