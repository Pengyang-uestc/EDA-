Add-Type -AssemblyName System.Windows.Forms,System.Drawing
Add-Type @"
using System;
using System.Text;
using System.Runtime.InteropServices;
public class W {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
    [DllImport("user32.dll")] public static extern int GetClassName(IntPtr h, StringBuilder s, int n);
    [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
    [DllImport("user32.dll")] public static extern void mouse_event(uint f, int x, int y, uint d, UIntPtr e);
    [DllImport("user32.dll")] public static extern bool SetCursorPos(int x, int y);
    [DllImport("user32.dll")] public static extern IntPtr PostMessage(IntPtr h, uint m, IntPtr w, IntPtr l);
    public struct RECT { public int L; public int T; public int R; public int B; }
}
"@
function FindDialog($p) {
    $found = [IntPtr]::Zero
    $cb = [W+EnumProc]{ param($hw, $l)
        $pid2 = 0
        [W]::GetWindowThreadProcessId($hw, [ref]$pid2) | Out-Null
        if ($pid2 -eq $script:p -and [W]::IsWindowVisible($hw)) {
            $sb = New-Object System.Text.StringBuilder 256
            [W]::GetClassName($hw, $sb, 256) | Out-Null
            if ($sb.ToString() -eq "#32770") { $script:found = $hw; return $false }
        }
        return $true
    }
    [W]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $found
}
function RealClick($x, $y) {
    [W]::SetCursorPos($x, $y) | Out-Null
    Start-Sleep -m 350
    [W]::mouse_event(2, 0, 0, 0, [UIntPtr]::Zero)
    Start-Sleep -m 80
    [W]::mouse_event(4, 0, 0, 0, [UIntPtr]::Zero)
    Start-Sleep -m 1200
}
$proc = Get-Process MyEDA -ErrorAction Stop
$h = $proc.MainWindowHandle
$pid2 = $proc.Id
[W]::SetForegroundWindow($h) | Out-Null
Start-Sleep -m 400

# 参数1:WM_COMMAND id;动作=弹出对话框→等它出现→激活→按比例点确认按钮
[W]::PostMessage($h, 0x0111, [IntPtr]$args[0], [IntPtr]0) | Out-Null
$dlg = [IntPtr]::Zero
for ($i = 0; $i -lt 20; $i++) {
    Start-Sleep -m 250
    $dlg = FindDialog $pid2
    if ($dlg -ne [IntPtr]::Zero) { break }
}
if ($dlg -eq [IntPtr]::Zero) { Write-Host "no dialog"; exit 1 }
[W]::SetForegroundWindow($dlg) | Out-Null
Start-Sleep -m 500
$r = New-Object W+RECT
[W]::GetWindowRect($dlg, [ref]$r) | Out-Null
$w = $r.R - $r.L; $ht = $r.B - $r.T
$bx = [int]($r.L + $w * 0.82)
$by = [int]($r.T + $ht * 0.945)
Write-Host "dialog L=$($r.L) T=$($r.T) W=$w H=$ht -> click $bx,$by"
RealClick $bx $by
Write-Host "clicked"
