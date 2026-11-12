@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0build.ps1" %*
set "build_result=%errorlevel%"
echo.
pause
exit /b %build_result%
