# gEDA pcb 对接指南（任务 5 定标）

老师《任务5分析.pdf》（见 [课程资料/任务5参考-gEDA/](../课程资料/任务5参考-gEDA/)）给任务 5
定标的开源工具链是 **gEDA：gschem（原理图）→ gsch2pcb（网表）→ pcb（布局布线）**。
结论：自研原理图程序替代 gschem，必须输出两个文件给 pcb：

1. **`.net` 网表**——每行 `网络名<TAB>位号-引脚 位号-引脚 ...`（引脚从 1 编号）
2. **`.pcb` 初始布局**——每个元件一个 `Element(封装)` 块，焊盘已按位号放好

MyEDA 两个文件都能导出（2026-11-12 完成，17 项单元测试覆盖）。

## 一、怎么导出

**界面**：文件 → 导出 gEDA 网表(&G) / 导出 gEDA 初始布局(&P)。

**命令行**（批处理/比对用）：

```powershell
.\MyEDA\build\Debug\test_sim.exe --export-geda-net examples\subcircuits\nested_full_adder.eda MyEDA\build\full_adder.net
.\MyEDA\build\Debug\test_sim.exe --export-geda-pcb examples\subcircuits\nested_full_adder.eda MyEDA\build\full_adder.pcb
```

子电路工程会先自动展平再导出（嵌套全加器 → 10 个实际元件：3 开关 + 5 门 + 2 灯）。

## 二、在 gEDA pcb 里加载布线（老师 PDF 2.3 节的步骤）

环境：WSL Ubuntu 24.04 编译安装 gEDA（老师另有《WSL ubuntu24.04-gEDA 编译.pdf》）。

1. 运行 `pcb`，File→New 新建空白工程。
2. **File → Load layout data to buffer**，选我们导出的 `.pcb`，在画布点击放置
   （我们已按网格铺开，无需再 Disperse）。
3. **File → Load netlist**，选同名的 `.net`。
4. 把元件拖到合适位置，按 **O** 飞线（按网表显示逻辑连线）。
5. **Connects → Auto-route all rats** 自动布线。
6. **File → Export**：选 bom（出 `.bom` 元件表 + `.xy` 坐标）、选 gerber（出光刻文件）。

## 三、我们的格式约定（讲解/答辩用）

| 元件 | 封装 | 引脚映射 |
|---|---|---|
| 六种门、自定义元件、D 触发器 | DIP14 | 1,2=输入，3=输出，7=GND，14=Vcc |
| 开关、时钟源、指示灯 | SIP2 | 逻辑脚 → 焊盘 1 |

- **功率网络自动补**：原理图里不画电源，导出时按 DIP14 标准给每个芯片自动生成
  `Vcc`（脚 14）和 `GND`（脚 7）网络——老师样例的 `.net` 同样含这两个网络。
- **不做"同片多门 slot"**：每个门当一颗单门芯片（74AHC1G08/32/86，老师 PDF 明确
  列了这组标准件）。74HC86 四门芯片的 slot 语义是第二版扩展项。
- 网络名沿用老师样例的 `unnamed_netN`（顺序稳定，同一电路重复导出逐字节一致）。
- 代码在 `MyEDA/src/file/GedaExport.h`，几何模板逐字段取自老师样例
  `full_adder.pcb`（该文件在 pcb 1.99x 实际加载过，不要"修正"坐标尺度）。

## 四、验证状态（如实）

- 单元测试 17 项：网络条数/扇出连接/输出脚=3/功率网络/确定性/封装模板/括号平衡/层栈/网格位置。
- 与 KiCad 网表逐网络语义比对一致（同一全加器两种格式连接关系相同）。
- **pcb 实机加载验证待做**：本机未装 WSL。装好 WSL 并按老师教程编译 gEDA 后，
  走第二节的 6 步，把"飞线图、自动布线图、Gerber 导出"截图存
  `docs/images/screenshots/`。这是结题阶段计划 A2 项。
