# MyEDA — 数字电路原理图编辑器

“工业软件创新训练 I”四人课程项目，使用 C++17、wxWidgets 3.2 和 CMake。

在画布上放置逻辑门、连接引脚，点击开关验证 0/1 逻辑，保存工程或导出文件到 KiCad。

## 快速开始（Windows）

先安装 Visual Studio 2022 的“使用 C++ 的桌面开发”（含 CMake 工具），准备 wxWidgets 3.2 的 x64 静态库。首次配置见[新人上手指南](docs/新人上手指南.md)。

| 你想做什么 | 仓库根目录中的入口 |
|---|---|
| 首次编译，或验证自己修改的代码 | 双击 `build.bat`，编译和测试通过后打开 `MyEDA/build/Debug/MyEDA.exe` |
| 更新当前分支并运行 | 先关闭 MyEDA，再双击 `update-and-run.bat` |
| 不更新，只运行已有程序 | 双击 `MyEDA/build/Debug/MyEDA.exe` |

更新入口要求工作区干净且当前分支有远程跟踪分支；有本地修改、拉取失败、编译失败或测试失败时停止。它不会自动提交、暂存或覆盖改动。Git 只更新源码，程序仍需编译；入口会自动完成增量编译。

打开示例：程序中选择“文件 → 打开”，选择 `docs/样例/半加器/half_adder.eda`，按 F5 后点击开关。

## 功能与范围

- 6 种逻辑门、开关、指示灯，共 8 种内置元件；支持用真值表定义双输入、单输出元件。
- 放置、移动、连线、框选、复制粘贴、删除、撤销和重做。
- 0/1 组合逻辑仿真，导线按逻辑值着色，指示灯显示输出。
- `.eda` 工程存档；KiCad 网表导入/导出；KiCad 原理图导出。

当前没有时钟/触发器、模拟电压电流或器件延迟仿真。KiCad 样例展示封装和网络导入，尚未完成 PCB 布线；默认封装用于对接演示，不能直接视为可制造电路。详细限制见[操作手册](docs/功能操作手册.md)。

## 文档导航

| 目的 | 文档 |
|---|---|
| 安装、运行、排查启动错误 | [新人上手指南](docs/新人上手指南.md) |
| 四人分支、提交、更新和合并 | [四人协作指南](docs/四人协作指南.md) |
| 菜单、鼠标操作、快捷键与文件格式 | [功能操作手册](docs/功能操作手册.md) |
| 理解代码和仿真原理 | [实现讲解](docs/实现讲解-新手版.md) |
| 分工和答辩演示 | [分工与答辩手册](docs/分工与答辩手册.md) |
| 组织课程报告 | [课程报告写作指南](docs/课程报告写作指南.md) |
| 数字电路基础 | [学习笔记](docs/学习笔记-数字电路基础/) |
| 一页连续阅读 | [项目完整教程](项目完整教程.md)（自动生成，请勿直接编辑） |

## 目录

```text
仓库根目录/
├── README.md
├── build.bat / build.ps1                 本地编译与测试
├── update-and-run.bat / update-and-run.ps1  更新、编译、测试、启动
├── MyEDA/
│   ├── CMakeLists.txt
│   ├── src/
│   │   ├── core/       元件、连线数据模型
│   │   ├── ui/         主窗口与对话框
│   │   ├── editor/     绘图和编辑
│   │   ├── file/       存档和 KiCad 对接
│   │   ├── sim/        仿真
│   │   └── res/        图标
│   └── test/           测试及命令行工具
├── docs/               分篇教程、学习笔记、演示图片和样例
├── tools/              文档生成和检查工具
└── 项目完整教程.md       由分篇教程生成
```

## 测试与文档维护

`build.bat` 会编译并运行测试；必须看到测试通过且脚本成功结束。本次整理时已有测试包含 45 项检查，后续以实际输出为准。

以下命令在仓库根目录的 **PowerShell** 中执行。三种命令行工具模式：

```powershell
.\MyEDA\build\Debug\test_sim.exe --export-netlist .\docs\样例\半加器\half_adder.eda .\MyEDA\build\half_adder.net
.\MyEDA\build\Debug\test_sim.exe --export-kicad-sch .\docs\样例\半加器\half_adder.eda .\MyEDA\build\half_adder.kicad_sch
.\MyEDA\build\Debug\test_sim.exe --parse-netlist .\docs\样例\半加器\half_adder.net
```

修改 `docs/` 中源文档后，用 Python 3 更新并检查合集：

```powershell
python .\tools\merge_docs.py
python .\tools\merge_docs.py --check
```

Python 只用于维护文档，不是运行 MyEDA 的前置条件。演示图片和速查卡不会随文本自动更新，修改对应功能后需人工复核。

维护构建脚本时，可在成功编译一次后运行 `python .\tools\test_build_failure.py`：它在临时目录模拟测试编译失败，确认脚本不会运行遗留的旧测试。
