@echo off
powershell -NoProfile -ExecutionPolicy Bypass -Command ^
    "$env:PATH = 'E:\Programm\w64devkit\bin;E:\Programm\Python\Scripts;E:\Programm\Git\cmd;' + $env:PATH; " ^
    "$cachePath = 'build/CMakeCache.txt'; " ^
    "if (Test-Path $cachePath) { " ^
    "    $currentDir = (Get-Location).Path.Replace('\', '/'); " ^
    "    $content = Get-Content $cachePath -Raw; " ^
    "    if ($content -notmatch [regex]::Escape($currentDir)) { " ^
    "        Write-Host '[REngine] Project moved or renamed! Automatically clearing stale CMake cache...' -ForegroundColor Yellow; " ^
    "        Remove-Item -Recurse -Force 'build/CMakeCache.txt', 'build/CMakeFiles' -ErrorAction SilentlyContinue; " ^
    "    } " ^
    "} " ^
    "Write-Host '[REngine] Configuring CMake with Ninja...' -ForegroundColor Cyan; " ^
    "cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=E:/Programm/w64devkit/bin/gcc.exe -DCMAKE_CXX_COMPILER=E:/Programm/w64devkit/bin/g++.exe; " ^
    "if ($LASTEXITCODE -eq 0) { " ^
    "    Write-Host '[REngine] Building Release (-j 2)...' -ForegroundColor Cyan; " ^
    "    cmake --build build --config Release -j 2; " ^
    "    $retries = 6; " ^
    "    while ($LASTEXITCODE -ne 0 -and $retries -gt 0) { " ^
    "        Write-Host \"[REngine] Retrying build (Windows Defender lock detected, $retries attempts remaining)...\" -ForegroundColor Yellow; " ^
    "        Start-Sleep -Seconds 2; " ^
    "        cmake --build build --config Release -j 1; " ^
    "        $retries--; " ^
    "    } " ^
    "    if ($LASTEXITCODE -eq 0) { " ^
    "        Write-Host '[REngine] Build succeeded! Executable: build\RaylibEngineApp.exe' -ForegroundColor Green; " ^
    "    } " ^
    "}"
