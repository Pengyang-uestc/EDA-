# 任务5 实机验证归档（2026-11-26 真实操作复核）

本目录是**用真实操作（GUI 菜单点击 + 真实 PCB 工具）端到端跑通老师任务5 链路**的原始证据，
不是单元测试的模拟。目的：证明 MyEDA 导出的 gEDA `.net`/`.pcb` 能被真实 pcb 软件
加载并**自动布线布通**。

## 一、操作步骤（全部真实点击/运行）

1. **启动 MyEDA**，打开 `examples/half_adder/half_adder.eda`（状态栏确认已加载）。
2. **文件(F) → 导出 gEDA 网表(G)...** → 保存对话框（类型自动选 `gEDA 网表 (*.net)`）→ 保存
   → 产出 `MyEDA菜单导出.net`。
3. **文件(F) → 导出 gEDA 初始布局(P)...** → 保存对话框（类型自动选 `gEDA PCB (*.pcb)`）→ 保存
   → 产出 `MyEDA菜单导出.pcb`。
4. 把这两个文件喂给 **pcb-rnd 3.1.4（WSL Ubuntu-24.04，真实工具）**，批处理执行：
   `LoadFrom(Netlist) → AddRats(AllRats, manhattan) → AutoRoute(AllRats) → SaveTo → Export(gerber)`。

## 二、实测结果

pcb-rnd 日志原文见 `pcb-rnd-run.log`，关键行：

```
I: Importing PCB netlist live.net
I: 8 of 8 nets successfully routed.
I: Total added wire length = 17.58800 in, 0 vias added
```

- **8 条网络全部自动布通**（4 信号 + GND/Vcc 拆分后的飞线）
- **0 短路**（SHORT 警告计数 = 0）
- 27 段走线、0 过孔、**13 个 Gerber 文件**全层栈导出
- 布线结果板见 `pcb-rnd-routed.pcb`

## 三、文件清单

| 文件 | 来源 | 说明 |
|---|---|---|
| `MyEDA菜单导出.net` | MyEDA 菜单真实导出 | 半加器网表，6 网络（4 信号 + GND + Vcc），DIP14 自动补电源脚 |
| `MyEDA菜单导出.pcb` | MyEDA 菜单真实导出 | 6 元件初始布局（2 SW + XOR/AND 两颗 DIP14 + 2 LED），坐标老师样例量级、网格铺开 |
| `pcb-rnd-run.log` | pcb-rnd 实测日志 | 加载网表 → 自动布线 8/8 全通 → 导出 Gerber 的完整输出 |
| `pcb-rnd-routed.pcb` | pcb-rnd 布线结果 | 自动布线后的板子（含 27 段走线） |

## 四、口径

- 本验证证明：**导出格式正确 + 能被真实 pcb 工具加载并自动布线 + Gerber 可导出**。
- 不等于：已送厂制板 / 已做物理板验证（答辩口径守住"Gerber 已导出 ≠ 已制板"）。
- 可复现脚本（非 GUI，用仓库样例文件）：`tools/verify_geda_pcb-rnd.sh`。
