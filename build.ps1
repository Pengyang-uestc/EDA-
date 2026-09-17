<#
MyEDA 一键编译脚本(给不熟悉命令行的组员用)

用法:
  双击 build.bat                    自动找 wxWidgets → 生成工程 → 编译 → 跑单元测试
  .\build.ps1 -Wx "D:\wxWidgets-3.2.2.1"    手动指定 wxWidgets 源码目录
  .\build.ps1 -Clean                先删掉 build 目录再编译(工程文件有问题时用)

说明:
  · 源码改动、甚至目录结构调整后,直接重新运行本脚本即可 —— CMake 会自动
    重新生成工程文件(它会检测到 CMakeLists.txt 变化),不需要手动删 build。
  · 万一真的出问题,再试 .\build.ps1 -Clean
#>
param(
    [string]$Wx = "",
    [switch]$Clean
)
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $MyInvocation.MyCommand.Path
$buildDir = Join-Path $root "MyEDA\build"

function Say($msg, $color = "Gray") { Write-Host $msg -ForegroundColor $color }

# ---------- 1. 找 wxWidgets ----------
if (-not $Wx) { $Wx = $env:WXWIN }
if (-not $Wx -or -not (Test-Path $Wx)) {
    $candidates = @(
        (Join-Path $root "..\基础平台搭建\基础平台搭建\基础工具软件-程序和例子\wxWidgets-3.2.2.1"),
        "D:\EDA工业软件\基础平台搭建\基础平台搭建\基础工具软件-程序和例子\wxWidgets-3.2.2.1",
        "C:\wxWidgets-3.2.2.1"
    )
    foreach ($c in $candidates) { if (Test-Path $c) { $Wx = $c; break } }
}
if (-not $Wx -or -not (Test-Path $Wx)) {
    Say "× 找不到 wxWidgets" "Red"
    Say "  请用参数指定它的位置(就是解压出来的 wxWidgets-3.2.2.1 文件夹):"
    Say '    .\build.ps1 -Wx "D:\你放wxWidgets的地方\wxWidgets-3.2.2.1"'
    exit 1
}
Say "√ 找到 wxWidgets: $Wx" "Green"

# ---------- 2. 需要时清掉旧 build ----------
if ($Clean -and (Test-Path $buildDir)) {
    Say "删除旧的 build 目录 ..."
    Remove-Item -Recurse -Force $buildDir
}

# ---------- 3. 生成工程 → 编译 → 测试 ----------
Say ""
Say "[1/3] 生成 Visual Studio 工程 ..."
& cmake -S (Join-Path $root "MyEDA") -B $buildDir -G "Visual Studio 17 2022" -A x64 `
        -DwxWidgets_ROOT_DIR="$Wx" -DwxWidgets_LIB_DIR="$Wx/lib/vc_x64_lib"
if ($LASTEXITCODE -ne 0) { Say "× CMake 配置失败,把上面的报错截图发群里" "Red"; exit 1 }

Say "[2/3] 编译程序 ..."
& cmake --build $buildDir --config Debug
if ($LASTEXITCODE -ne 0) {
    Say "× 编译失败。可以先试:  .\build.ps1 -Clean" "Red"
    exit 1
}

Say "[3/3] 跑单元测试(期望看到 ALL PASS) ..."
& cmake --build $buildDir --config Debug --target test_sim | Out-Null
& (Join-Path $buildDir "Debug\test_sim.exe")

Say ""
Say "全部完成!程序在这里:" "Green"
Say "  $buildDir\Debug\MyEDA.exe"
Say "  双击运行即可;也可以带一个电路文件打开:"
Say "  MyEDA.exe docs\样例\半加器\half_adder.eda"
