@echo off
setlocal
cd /d "%~dp0"
taskkill /F /IM RaylibEngineApp.exe >nul 2>&1
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0scripts\build.ps1" %*
exit /b %ERRORLEVEL%
