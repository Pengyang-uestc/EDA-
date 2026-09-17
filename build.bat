@echo off
rem ================================================================
rem  MyEDA 一键编译(双击本文件即可)
rem  等价于执行同目录的 build.ps1,详见那个文件里的说明
rem ================================================================
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0build.ps1" %*
echo.
pause
