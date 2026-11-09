# MyEDA · 数字电路编辑与仿真

MyEDA 是四人 EDA 课程综合设计项目，使用 C++17、wxWidgets 和 CMake 开发。你可以在 Windows 桌面上绘制数字电路、观察信号传播，将常用电路封装为可复用元件，再把原理图或网表交给 KiCad，继续完成 PCB 布局和布线。

## 能做什么

| 功能 | 操作与用途 |
| --- | --- |
| 原理图编辑 | 放置逻辑门、开关和指示灯，使用网格吸附和多拐点直角连线。支持选择、复制粘贴、撤销重做和缩放平移。 |
| 逻辑仿真 | 切换输入开关，通过导线颜色和指示灯查看输出。顶层电路支持时钟源和上升沿触发的 D 触发器。 |
| 元件复用 | 用真值表定义双输入逻辑门，或将完整组合电路封装成带命名端口的子电路。支持嵌套、查看内部、管理元件库和跨工程导入导出。 |
| 工程管理 | 保存和打开工程，支持按画布位置重新排列元件位号。 |
| KiCad 交换 | 导入和导出网表，导出 KiCad 原理图。网表记录驱动端、接收端和未连接引脚，导出时检查无效引用和多驱动。 |

子电路封装适用于组合逻辑，时钟源和 D 触发器可放在顶层使用。电气连接由导线端点决定，交叉线段需要显式连接。KiCad 交换文件用于后续设计，制造前需在 KiCad 中确认实际器件与封装、完成布局布线和设计规则检查。

## 运行与构建

已有本机安装目录时，直接双击 `release/MyEDA/MyEDA.exe`。程序和同级 `res/` 目录应一起保留。首次克隆仓库后，在 Windows 上准备以下环境并执行构建安装流程：

- Visual Studio 2022，安装「使用 C++ 的桌面开发」工作负载。
- CMake 3.20 或以上。
- 已编译的 wxWidgets 3.2 x64 库，包含 `core` 和 `base`。开发环境使用过 wxWidgets 3.2.2.1。

从仓库根目录运行以下 PowerShell 命令，将 wxWidgets 路径替换为本机的实际位置：

```powershell
cmake -S MyEDA -B MyEDA/build -G "Visual Studio 17 2022" -A x64 "-DwxWidgets_ROOT_DIR=D:/libs/wxWidgets-3.2.2.1"
cmake --build MyEDA/build --config Release --target MyEDA
cmake --install MyEDA/build --config Release --prefix release/MyEDA
```

安装成功后，程序和资源位于 `release/MyEDA/`。更新源码后，再执行构建和安装两条命令即可刷新本机程序。

## 用半加器体验完整流程

1. 启动 MyEDA，通过「文件 → 打开」载入半加器工程。 [half_adder.eda](examples/half_adder/half_adder.eda)
2. 按 **F5** 开始仿真。分别切换两个输入开关，观察和位与进位。两个输入均为 1 时，和位为 0、进位为 1。
3. 按 **F6** 停止仿真。选中包含开关、逻辑门、连线和指示灯的完整电路，通过「工程 → 将选中电路封装为元件」生成可复用的半加器。 [子电路封装指南](docs/使用指南/子电路封装指南.md)
4. 打开由两个半加器组成的全加器，或者打开嵌套示例，选中元件后查看内部结构。 [全加器](examples/subcircuits/full_adder.eda) · [嵌套全加器](examples/subcircuits/nested_full_adder.eda)
5. 在「文件」菜单中导出网表或 KiCad 原理图，查看器件及引脚之间的连接。 [网表模型说明](docs/开发协作/网表模型.md)

| 文件 | 用途 |
| --- | --- |
| `.eda` | MyEDA 电路工程，通过「文件 → 打开」载入。 |
| `.edasub` | 可复用的子电路元件，通过「工程 → 导入子电路元件」加入元件库。 |
| `.net` | KiCad 网表，描述元件和引脚连接关系。 |
| `.kicad_sch` | KiCad 原理图，用于在 KiCad 中继续编辑。 |

## 源码结构

```text
EDA-repo/
├── MyEDA/
│   ├── CMakeLists.txt       # 构建与安装配置
│   ├── src/
│   │   ├── ui/              # 主窗口、菜单与元件对话框
│   │   ├── editor/          # 画布、连线与编辑交互
│   │   ├── core/            # 元件、路由、子电路与电气网表
│   │   ├── sim/             # 组合逻辑与时序仿真
│   │   ├── file/            # 工程存档与 KiCad 交换
│   │   └── res/             # 图标等界面资源
│   ├── test/                # 仿真、文件、子电路与画布测试
│   └── build/               # 本机构建目录
├── examples/                # 示例电路与参考网表
├── docs/                    # 操作、开发与课程资料
├── tools/                   # 文档生成工具
└── release/                 # 本机程序安装目录
```

## 回归测试

```powershell
cmake --build MyEDA/build --config Release --target test_sim test_modules test_canvas
.\MyEDA\build\Release\test_sim.exe
.\MyEDA\build\Release\test_modules.exe
.\MyEDA\build\Release\test_canvas.exe
```

`test_sim` 覆盖逻辑、文件读写与网表，`test_modules` 覆盖层次封装和时序兼容，`test_canvas` 覆盖画布交互。运行画布测试时需要可用的 Windows 桌面。

## 项目资料

- 软件操作、开发协作和课程资料。 [文档索引](docs/README.md)
- 项目进度、团队分工和演示材料。 [中期汇报](docs/课程资料/中期总结汇报.md) · [汇报 PPT](docs/课程资料/中期总结汇报.pptx) · [分工与贡献说明](docs/课程资料/四人分工与贡献说明.md)
