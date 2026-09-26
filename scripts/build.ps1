# REngine Universal Build & Bootstrap Script
# Supports: System PATH, Project-Local .tools/, MSVC, Clang, GCC (w64devkit), Auto-Bootstrap
[CmdletBinding()]
param(
    [switch]$Clean,
    [switch]$ForceBootstrap,
    [string]$BuildType = "Release",
    [int]$Jobs = 2
)

$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path "$PSScriptRoot\..").Path
$ToolsDir = Join-Path $ProjectRoot ".tools"
$BuildDir = Join-Path $ProjectRoot "build"

Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "   REngine -- Universal AI-First Build and Bootstrap System  " -ForegroundColor Cyan
Write-Host "============================================================" -ForegroundColor Cyan

# 1. Clean build directory if requested
if ($Clean) {
    Write-Host "[REngine] Cleaning build directory..." -ForegroundColor Yellow
    if (Test-Path $BuildDir) {
        Remove-Item -Recurse -Force $BuildDir -ErrorAction SilentlyContinue
    }
}

# 2. Check stale CMakeCache.txt (Project moved or renamed)
$CachePath = Join-Path $BuildDir "CMakeCache.txt"
if (Test-Path $CachePath) {
    $currentNormalized = $ProjectRoot.Replace('\', '/')
    $cacheContent = Get-Content $CachePath -Raw -ErrorAction SilentlyContinue
    if ($cacheContent -and ($cacheContent -notmatch [regex]::Escape($currentNormalized))) {
        Write-Host "[REngine] Project moved or renamed! Automatically clearing stale CMake cache..." -ForegroundColor Yellow
        Remove-Item -Recurse -Force $CachePath -ErrorAction SilentlyContinue
        Remove-Item -Recurse -Force (Join-Path $BuildDir "CMakeFiles") -ErrorAction SilentlyContinue
    }
}

# Helper: Download and extract with fallback
function Install-PortablePackage {
    param(
        [string]$Name,
        [string]$Url,
        [string]$DestFolder,
        [string]$ExtractCheckFile
    )

    if (Test-Path $ExtractCheckFile) {
        return
    }

    Write-Host "[REngine] [Bootstrap] Missing $Name. Auto-downloading portable package..." -ForegroundColor Green
    if (-not (Test-Path $ToolsDir)) {
        New-Item -ItemType Directory -Path $ToolsDir -Force | Out-Null
    }

    $zipPath = Join-Path $ToolsDir "$Name.zip"
    $downloadSuccess = $false

    # Try curl.exe first (fast & reliable)
    if (Get-Command curl.exe -ErrorAction SilentlyContinue) {
        Write-Host "[REngine] [Bootstrap] Downloading $Name via curl..." -ForegroundColor Gray
        & curl.exe -sL "$Url" -o "$zipPath"
        if ($LASTEXITCODE -eq 0 -and (Test-Path $zipPath) -and ((Get-Item $zipPath).Length -gt 100000)) {
            $downloadSuccess = $true
        }
    }

    if (-not $downloadSuccess) {
        Write-Host "[REngine] [Bootstrap] Downloading $Name via Invoke-WebRequest..." -ForegroundColor Gray
        [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
        Invoke-WebRequest -Uri $Url -OutFile $zipPath -UseBasicParsing
        if ((Test-Path $zipPath) -and ((Get-Item $zipPath).Length -gt 100000)) {
            $downloadSuccess = $true
        }
    }

    if (-not $downloadSuccess) {
        throw "Failed to download $Name from $Url"
    }

    Write-Host "[REngine] [Bootstrap] Extracting $Name..." -ForegroundColor Gray
    if (-not (Test-Path $DestFolder)) {
        New-Item -ItemType Directory -Path $DestFolder -Force | Out-Null
    }

    # Use tar.exe if available (super fast) else Expand-Archive
    if (Get-Command tar.exe -ErrorAction SilentlyContinue) {
        & tar.exe -xf "$zipPath" -C "$DestFolder"
    } else {
        Expand-Archive -Path "$zipPath" -DestinationPath "$DestFolder" -Force
    }

    # Clean up archive
    Remove-Item -Force "$zipPath" -ErrorAction SilentlyContinue
    Write-Host "[REngine] [Bootstrap] $Name installed successfully!" -ForegroundColor Green
}

# 3. Toolchain Discovery & Resolution
$CmakeExe = $null
$NinjaExe = $null
$CompilerGcc = $null
$CompilerGxx = $null
$CompilerClang = $null
$CompilerClangxx = $null
$CompilerCl = $null

# Check system PATH first
$cmdCmake = Get-Command cmake.exe -ErrorAction SilentlyContinue
if ($cmdCmake) { $CmakeExe = $cmdCmake.Source }

$cmdNinja = Get-Command ninja.exe -ErrorAction SilentlyContinue
if ($cmdNinja) { $NinjaExe = $cmdNinja.Source }

$cmdGxx = Get-Command g++.exe -ErrorAction SilentlyContinue
if ($cmdGxx) {
    $CompilerGxx = $cmdGxx.Source
    $cmdGcc = Get-Command gcc.exe -ErrorAction SilentlyContinue
    if ($cmdGcc) { $CompilerGcc = $cmdGcc.Source }
}

$cmdClangxx = Get-Command clang++.exe -ErrorAction SilentlyContinue
if ($cmdClangxx) {
    $CompilerClangxx = $cmdClangxx.Source
    $cmdClang = Get-Command clang.exe -ErrorAction SilentlyContinue
    if ($cmdClang) { $CompilerClang = $cmdClang.Source }
}

$cmdCl = Get-Command cl.exe -ErrorAction SilentlyContinue
if ($cmdCl) { $CompilerCl = $cmdCl.Source }

# Check local .tools directory
$localCmakeCandidates = @(
    "$ToolsDir\cmake\bin\cmake.exe",
    "$ToolsDir\bin\cmake.exe"
)
foreach ($c in $localCmakeCandidates) {
    if (-not $CmakeExe -and (Test-Path $c)) { $CmakeExe = (Resolve-Path $c).Path }
}
if (-not $CmakeExe -and (Test-Path "$ToolsDir\cmake")) {
    $found = Get-ChildItem -Path "$ToolsDir\cmake" -Filter "cmake.exe" -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($found) { $CmakeExe = $found.FullName }
}

$localNinjaCandidates = @(
    "$ToolsDir\ninja\ninja.exe",
    "$ToolsDir\bin\ninja.exe"
)
foreach ($n in $localNinjaCandidates) {
    if (-not $NinjaExe -and (Test-Path $n)) { $NinjaExe = (Resolve-Path $n).Path }
}

$localW64Candidates = @(
    "$ToolsDir\w64devkit\bin\g++.exe",
    "$ToolsDir\bin\g++.exe"
)
foreach ($w in $localW64Candidates) {
    if (-not $CompilerGxx -and (Test-Path $w)) {
        $CompilerGxx = (Resolve-Path $w).Path
        $CompilerGcc = Join-Path (Split-Path $CompilerGxx) "gcc.exe"
    }
}

# Check standard program directories if still missing
if (-not $CmakeExe) {
    $stdCmake = @(
        "C:\Program Files\CMake\bin\cmake.exe",
        "C:\Program Files (x86)\CMake\bin\cmake.exe"
    )
    foreach ($sc in $stdCmake) {
        if (Test-Path $sc) { $CmakeExe = $sc; break }
    }
}

if (-not $CompilerClangxx) {
    $stdLlvm = "C:\Program Files\LLVM\bin\clang++.exe"
    if (Test-Path $stdLlvm) {
        $CompilerClangxx = $stdLlvm
        $CompilerClang = "C:\Program Files\LLVM\bin\clang.exe"
    }
}

# 4. Auto-Bootstrap if components are missing
# Need: CMake, Ninja, and at least one C++ compiler (GCC, Clang, or MSVC)
if (-not $CmakeExe -or $ForceBootstrap) {
    $cmakeDest = Join-Path $ToolsDir "cmake"
    $cmakeZipCheck = Join-Path $cmakeDest "bin\cmake.exe"
    Install-PortablePackage -Name "cmake" -Url "https://github.com/Kitware/CMake/releases/download/v3.30.5/cmake-3.30.5-windows-x86_64.zip" -DestFolder $ToolsDir -ExtractCheckFile $cmakeZipCheck
    # Normalize folder name if it extracted as cmake-3.30.5-windows-x86_64
    $extractedFolder = Join-Path $ToolsDir "cmake-3.30.5-windows-x86_64"
    if (Test-Path $extractedFolder) {
        if (Test-Path $cmakeDest) { Remove-Item -Recurse -Force $cmakeDest }
        Rename-Item -Path $extractedFolder -NewName "cmake"
    }
    $CmakeExe = Join-Path $ToolsDir "cmake\bin\cmake.exe"
}

if (-not $NinjaExe -or $ForceBootstrap) {
    $ninjaDest = Join-Path $ToolsDir "bin"
    $ninjaCheck = Join-Path $ninjaDest "ninja.exe"
    Install-PortablePackage -Name "ninja" -Url "https://github.com/ninja-build/ninja/releases/download/v1.12.1/ninja-win.zip" -DestFolder $ninjaDest -ExtractCheckFile $ninjaCheck
    $NinjaExe = $ninjaCheck
}

$hasCompiler = ($CompilerGxx -ne $null) -or ($CompilerClangxx -ne $null) -or ($CompilerCl -ne $null)
if (-not $hasCompiler -or $ForceBootstrap) {
    $w64Dest = Join-Path $ToolsDir "w64devkit"
    $w64Check = Join-Path $w64Dest "bin\g++.exe"
    # w64devkit zip contains a root folder "w64devkit"
    Install-PortablePackage -Name "w64devkit" -Url "https://github.com/skeeto/w64devkit/releases/download/v1.23.0/w64devkit-1.23.0.zip" -DestFolder $ToolsDir -ExtractCheckFile $w64Check
    $CompilerGxx = Join-Path $ToolsDir "w64devkit\bin\g++.exe"
    $CompilerGcc = Join-Path $ToolsDir "w64devkit\bin\gcc.exe"
}

# 5. Populate PATH with all toolchain locations for child processes
$toolDirs = @()
if ($CmakeExe) { $toolDirs += (Split-Path $CmakeExe) }
if ($NinjaExe) { $toolDirs += (Split-Path $NinjaExe) }
if ($CompilerGxx) { $toolDirs += (Split-Path $CompilerGxx) }
if ($CompilerClangxx) { $toolDirs += (Split-Path $CompilerClangxx) }
$uniqueToolDirs = $toolDirs | Select-Object -Unique
$env:PATH = ($uniqueToolDirs -join ";") + ";" + $env:PATH

# Print active toolchain
Write-Host "[REngine] Active Toolchain Configuration:" -ForegroundColor Green
Write-Host "  - CMake:    $CmakeExe" -ForegroundColor Gray
Write-Host "  - Ninja:    $NinjaExe" -ForegroundColor Gray
if ($CompilerGxx) {
    Write-Host "  - CXX (GCC): $CompilerGxx" -ForegroundColor Gray
} elseif ($CompilerClangxx) {
    Write-Host "  - CXX (Clang): $CompilerClangxx" -ForegroundColor Gray
} elseif ($CompilerCl) {
    Write-Host "  - CXX (MSVC): $CompilerCl" -ForegroundColor Gray
}

# 6. Configure CMake
Write-Host "[REngine] Configuring CMake project..." -ForegroundColor Cyan

$cmakeArgs = @("-B", "build", "-DCMAKE_BUILD_TYPE=$BuildType")

if ($NinjaExe) {
    $cmakeArgs += @("-G", "Ninja")
    $cmakeArgs += "-DCMAKE_MAKE_PROGRAM=$NinjaExe"
}

# Prefer GCC / Clang if specified
if ($CompilerGxx -and $CompilerGcc) {
    $cNormalized = $CompilerGcc.Replace('\', '/')
    $cxxNormalized = $CompilerGxx.Replace('\', '/')
    $cmakeArgs += "-DCMAKE_C_COMPILER=$cNormalized"
    $cmakeArgs += "-DCMAKE_CXX_COMPILER=$cxxNormalized"
} elseif ($CompilerClangxx -and $CompilerClang) {
    $cNormalized = $CompilerClang.Replace('\', '/')
    $cxxNormalized = $CompilerClangxx.Replace('\', '/')
    $cmakeArgs += "-DCMAKE_C_COMPILER=$cNormalized"
    $cmakeArgs += "-DCMAKE_CXX_COMPILER=$cxxNormalized"
}

& "$CmakeExe" $cmakeArgs
if ($LASTEXITCODE -ne 0) {
    Write-Host "[REngine] [ERROR] CMake configuration failed!" -ForegroundColor Red
    exit $LASTEXITCODE
}

# 7. Build Project with Windows Defender Retry Handling
Write-Host "[REngine] Compiling ($BuildType, jobs: $Jobs)..." -ForegroundColor Cyan
& "$CmakeExe" --build build --config $BuildType -j $Jobs

if ($LASTEXITCODE -ne 0) {
    $retries = 6
    while ($LASTEXITCODE -ne 0 -and $retries -gt 0) {
        Write-Host "[REngine] Retrying compilation (antivirus lock recovery, $retries attempts remaining)..." -ForegroundColor Yellow
        Start-Sleep -Seconds 2
        & "$CmakeExe" --build build --config $BuildType -j 1
        $retries--
    }
}

if ($LASTEXITCODE -eq 0) {
    $appExe = Join-Path $BuildDir "RaylibEngineApp.exe"
    Write-Host "============================================================" -ForegroundColor Green
    Write-Host " [REngine] Build succeeded!" -ForegroundColor Green
    Write-Host " Output executable: $appExe" -ForegroundColor Green
    Write-Host " Run testing with:  .\build\RaylibEngineApp.exe --test-frames 60" -ForegroundColor Green
    Write-Host "============================================================" -ForegroundColor Green
} else {
    Write-Host "[REngine] [ERROR] Build failed after retries." -ForegroundColor Red
    exit $LASTEXITCODE
}
