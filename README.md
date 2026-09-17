# MyEDA — 电路原理图编辑器

> 「工业软件创新训练 I」课程项目 · 小组四人合作
> 用 **C++17 + wxWidgets 3.2** 实现的数字电路原理图编辑工具：
> 画电路 → 逻辑仿真 → 导出网表 → 在 KiCad 里生成 PCB。

---

## 一、功能(对应课程六个任务)

| 任务 | 内容 | 我们的实现 |
|---|---|---|
| 1 硬件电路基础 | 逻辑代数与组合逻辑 | 学习笔记见 `docs/学习笔记-数字电路基础/` |
| 2 用户界面模块 | 菜单/工具栏/元件库树/绘图区/属性表 | 主窗口三栏布局,属性表随选中实时刷新 |
| 3 元件库模块 | 预定义元件 + **用户自定义元件** | 与/或/非/异或/与非/或非门 + 开关 + 指示灯;**用户用真值表自定义元件** |
| 4 绘图与编辑模块 | 放置/移动/连线等 | 放置、整组拖动、引脚连线、框选多选、删除、复制粘贴、撤销重做 |
| 5 文件功能模块 | 网表结构 + 导入导出,能被 PCB 软件加载 | `.eda` 工程存档;KiCad 网表导出/导入;**网表在 KiCad 10 里导入 0 错误并生成 PCB** |
| 6 电路仿真模块 | 动态信号传播与逻辑仿真 | 迭代传播算法;点开关实时重算,导线按电位着色,LED 亮灭 |

**验收案例**:半加器(两开关 + 异或门 + 与门 + 两灯),四种输入组合与真值表一致。

---

## 二、目录结构(按模块划分,对应小组分工)

```
MyEDA/
├── CMakeLists.txt            构建脚本
├── src/
│   ├── core/     Component.h              数据模型:元件/连线/引脚坐标
│   ├── ui/       MyApp.cpp                程序入口
│   │             MyFrame.h/.cpp           主窗口:菜单/工具栏/元件库树/绘图区/属性表
│   │             CustomDialog.h           自定义元件对话框(真值表定义 + 管理)
│   ├── editor/   DrawingCanvas.h/.cpp     绘图区:绘制元件符号 + 鼠标交互(放置/拖动/连线/框选/撤销)
│   ├── file/     CircuitFile.h            .eda 工程存档(JSON)读写
│   │             NetlistExport.h          网表导出/导入(KiCad S 表达式)
│   │             KicadSchExport.h         导出 .kicad_sch 原理图
│   ├── sim/      Simulation.h             仿真引擎(纯函数,不碰界面)
│   └── res/                               工具栏图标(10 个 PNG)
├── test/         test_sim.cpp             单元测试(45 项)+ 命令行工具
└── docs/                                  教程、分工手册、样例、学习笔记
```

| 目录 | 负责人 | 说明 |
|---|---|---|
| `src/core/` | 全组共用(B 维护) | 数据模型是共同语言,先冻结再开发 |
| `src/ui/` | A | 界面与对话框 |
| `src/editor/` | B | 绘图与编辑交互 |
| `src/file/` | C | 存档、网表、KiCad 对接 |
| `src/sim/` + `test/` | D | 仿真引擎与测试 |

> 分工细节、逐人讲稿、答辩 20 问见 **[docs/分工与答辩手册.md](docs/分工与答辩手册.md)**

---

## 三、编译与运行

前置:Visual Studio 2022(或 BuildTools)+ CMake ≥ 3.20 + 已编译好的 wxWidgets 3.2(静态库)。

```bash
# 1) 生成工程(wxWidgets 路径按本机实际情况改)
cmake -S MyEDA -B MyEDA/build -G "Visual Studio 17 2022" -A x64 \
      -DwxWidgets_ROOT_DIR="<wxWidgets 源码目录>" \
      -DwxWidgets_LIB_DIR="<wxWidgets 源码目录>/lib/vc_x64_lib"

# 2) 编译
cmake --build MyEDA/build --config Debug

# 3) 运行(可直接带文件路径打开电路)
MyEDA\build\Debug\MyEDA.exe
MyEDA\build\Debug\MyEDA.exe docs\样例\半加器\half_adder.eda
```

> **注意**:源代码已经按模块分目录(`core/ ui/ editor/ file/ sim/`),
> 如果你之前生成过 `MyEDA/build`,拉取这次改动后请**删掉 build 目录重新执行上面的第 1 步**
> (源码路径变了,旧工程文件会指向不存在的文件)。

## 四、测试与命令行工具

```bash
# 单元测试(不开窗口):45 项检查,覆盖真值表/存档往返/网表往返/KiCad 网表互操作
cmake --build MyEDA/build --config Debug --target test_sim
MyEDA\build\Debug\test_sim.exe

# 命令行工具(同一程序的两个模式)
test_sim.exe --export-netlist  电路.eda 输出.net          # 导出 KiCad 网表
test_sim.exe --export-kicad-sch 电路.eda 输出.kicad_sch   # 导出 KiCad 原理图
test_sim.exe --parse-netlist   任意.net                   # 解析网表并打印摘要
```

## 五、文档与样例

| 位置 | 内容 |
|---|---|
| `docs/分工与答辩手册.md` | 四人分工、逐人讲稿、8 步现场演示脚本、老师可能问的 20 问 + 答案、现场应急 |
| `docs/实现讲解-新手版.md` | 面向零基础的技术讲解:数据模型、分层、各功能"为什么这么写"、踩坑记录 |
| `docs/学习笔记-数字电路基础/` | 任务 1 的学习资料(逻辑代数、组合逻辑) |
| `docs/样例/半加器/` | 同一个半加器的四件套:`.eda`(工程)、`.net`(网表)、`.kicad_sch`(原理图)、`.kicad_pcb`(KiCad 生成的 PCB) |
| `docs/样例/*.png` | 关键验证截图(KiCad 导入 0 错误、生成的 PCB、演示参考图) |

## 六、与 KiCad 的对接(已实测)

1. 在我们程序里:文件 → 导出网表 / 导出 KiCad 原理图
2. 在 KiCad 的 PCB 编辑器里:文件 → 导入 → 网表 → 选择 `.net`
   (或直接打开导出的 `.kicad_sch` 后按 F8「从原理图更新 PCB」)
3. 实测结果:**错误 0**,生成 PCB(6 个封装、40 个焊盘、4 个网络)
   - 网表里的网络名就是我们生成的,例如 `Net-(U1-Pad1)`
   - 元件默认封装:逻辑门 `Package_DIP:DIP-14_W7.62mm`、开关 `Button_Switch_THT:SW_PUSH_6mm`、灯 `LED_THT:LED_D5.0mm`
