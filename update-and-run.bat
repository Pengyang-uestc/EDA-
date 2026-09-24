@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0update-and-run.ps1" %*
if errorlevel 1 (
    echo.
    pause
    exit /b 1
)
exit /b 0
