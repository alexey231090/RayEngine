@echo off
if exist "build\RaylibEngineApp.exe" (
    start "" "build\RaylibEngineApp.exe"
) else if exist "build\RaylibApp.exe" (
    start "" "build\RaylibApp.exe"
) else (
    echo [ERROR] Executable not found! Please build the project first with build.bat.
    pause
)
