# MyEDA 数字电路编辑与仿真

C++17 / wxWidgets / CMake 桌面项目。支持网格编辑、组合与顶层时序仿真、层次子电路、工程存档及 KiCad 交换。

## 运行

直接打开 `release/MyEDA/MyEDA.exe`；`res/` 必须与程序同目录。此目录为本机发布产物，不提交到 Git。旧 BAT/PowerShell 快捷入口已移除。

## 开发与安装

在配置好 Visual Studio C++、CMake 和 wxWidgets 后，从仓库根目录执行：

```powershell
cmake -S MyEDA -B MyEDA/build -DwxWidgets_ROOT_DIR="你的 wxWidgets 路径"
cmake --build MyEDA/build --config Release --target MyEDA test_sim test_modules test_canvas
cmake --install MyEDA/build --config Release --prefix release/MyEDA
```

运行对应配置下的 test_sim、test_modules、test_canvas；画布测试需要可用桌面。更新代码使用 Git，随后增量构建与安装。

## 目录

- `MyEDA/src/`：core 数据与网表模型、editor 画布、ui 界面、sim 仿真、file 交换、res 资源。
- `MyEDA/test/`：逻辑、文件、子电路及画布回归。
- `docs/`：操作、开发协作和课程资料。
- `examples/`：半加器、全加器和层次封装示例。
- `tools/`：仍在使用的文档生成工具。
- `release/`：本机安装目录；`MyEDA/build/`：构建缓存。
- `dist/`：先前展示版本，暂保留以免影响已有演示材料。

## 网表

`core/Netlist.h` 提供 ElectricalNet 和 CircuitNetlist：网络编号、引脚、驱动端、接收端、未连接引脚及诊断。KiCad 导出拒绝无效引用和多驱动网络。无驱动网络保留警告，供参考网表检查；不能把警告理解为可仿真电路已经通过校验。导线交叉不自动连接，当前仍按端点定义电气连接。

本机发布包和历史课程报告不随源码自动更新。历史文档中的旧脚本命令已失效，以本页 CMake 操作为准。

## 团队更新

已合并原理图位号重排、子电路元件管理，以及 docs/课程资料/中期总结汇报.md 和对应 PPT。构建与运行按上方 CMake 流程执行。
