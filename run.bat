@echo off
if exist "build\RaylibEngineApp.exe" (
    start "" "build\RaylibEngineApp.exe" %*
) else (
    echo [ERROR] Executable not found! Please build the project first with build.bat.
    pause
)
