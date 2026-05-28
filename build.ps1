# ============================================================
#  Build script for H_Track_Car (2024 E-Design Competition Problem H)
#  Target: MSPM0G3507 (Cortex-M0+)
#  Framework: 铁头山羊 layered architecture
# ============================================================

$ErrorActionPreference = "Stop"

# ---- Paths ----
$ProjectRoot = "$PSScriptRoot"
$DebugDir    = "$ProjectRoot\Debug"
$CompilerDir = "E:\Study\DianSai\TI\ccs\tools\compiler\ti-cgt-armllvm_4.0.4.LTS"
$SDKRoot     = "E:\Study\DianSai\TI\mspm0_sdk_2_10_00_04"
$SDKSource   = "$SDKRoot\source"
$CC          = "$CompilerDir\bin\tiarmclang.exe"

# ---- Source files ----
$StartupFile = "$SDKSource\ti\devices\msp\m0p\startup_system_files\ticlang\startup_mspm0g350x_ticlang.c"
$SysConfigSource = "$DebugDir\ti_msp_dl_config.c"

$MyLibSources = @(
    "$ProjectRoot\my_lib\pid.c",
    "$ProjectRoot\my_lib\lpf.c",
    "$ProjectRoot\my_lib\qmath.c",
    "$ProjectRoot\my_lib\delay.c",
    "$ProjectRoot\my_lib\sys.c"
)

$UserSources = @(
    "$ProjectRoot\user\app_gyro.c",
    "$ProjectRoot\user\app_encoder.c",
    "$ProjectRoot\user\app_motor.c",
    "$ProjectRoot\user\app_sensor.c",
    "$ProjectRoot\user\app_control.c",
    "$ProjectRoot\user\app_bat.c",
    "$ProjectRoot\user\main.c"
)

# ---- Libraries ----
$DriverLib = "$SDKSource\ti\driverlib\lib\ticlang\m0p\mspm0g1x0x_g3x0x\driverlib.a"
$LibC      = "$CompilerDir\lib\libc.a"

# ---- Common compiler arguments ----
$CommonArgs = @(
    "-march=thumbv6m",
    "-mcpu=cortex-m0plus",
    "-mfloat-abi=soft",
    "-mlittle-endian",
    "-mthumb",
    "-O0",
    "-gdwarf-3"
)

$IncludeArgs = @(
    "-I$DebugDir",
    "-I$ProjectRoot",
    "-I$ProjectRoot\my_lib",
    "-I$ProjectRoot\user",
    "-I$SDKSource",
    "-I$SDKSource\third_party\CMSIS\Core\Include"
)

$DeviceResponseFile = "$DebugDir\device.opt"

function Invoke-Compile {
    param([string]$Source, [string]$Output)
    $args = @("-c") + $CommonArgs + @("@$DeviceResponseFile") + $IncludeArgs + @("-o", $Output, $Source)
    & $CC $args
    if ($LASTEXITCODE -ne 0) { throw "Compilation failed: $Source" }
}

# ---- Clean old objects ----
Write-Host "=== Cleaning old objects ===" -ForegroundColor Cyan
Remove-Item -Path "$DebugDir\*.o" -Force -ErrorAction SilentlyContinue

# ---- Compile ----
Write-Host "=== Compiling ===" -ForegroundColor Cyan

$ObjFiles = @()

# Compile startup file
$startupObj = "$DebugDir\startup_mspm0g350x_ticlang.o"
Write-Host "  Compiling startup_mspm0g350x_ticlang.c ..."
Invoke-Compile -Source $StartupFile -Output $startupObj
$ObjFiles += $startupObj

# Compile sysconfig file
$syscfgObj = "$DebugDir\ti_msp_dl_config.o"
Write-Host "  Compiling ti_msp_dl_config.c ..."
Invoke-Compile -Source $SysConfigSource -Output $syscfgObj
$ObjFiles += $syscfgObj

# Compile my_lib sources
foreach ($src in $MyLibSources) {
    $srcName = Split-Path $src -Leaf
    $objName = [System.IO.Path]::ChangeExtension($srcName, ".o")
    $objPath = "$DebugDir\$objName"
    Write-Host "  Compiling my_lib/$srcName ..."
    Invoke-Compile -Source $src -Output $objPath
    $ObjFiles += $objPath
}

# Compile user sources
foreach ($src in $UserSources) {
    $srcName = Split-Path $src -Leaf
    $objName = [System.IO.Path]::ChangeExtension($srcName, ".o")
    $objPath = "$DebugDir\$objName"
    Write-Host "  Compiling user/$srcName ..."
    Invoke-Compile -Source $src -Output $objPath
    $ObjFiles += $objPath
}

# ---- Link ----
Write-Host "=== Linking ===" -ForegroundColor Cyan

$OutFile = "$DebugDir\H_Track_Car.out"
$MapFile = "$DebugDir\H_Track_Car.map"

$LinkArgs = $CommonArgs + @(
    "@$DeviceResponseFile",
    "-Wl,-m$MapFile",
    "-Wl,-i$SDKSource",
    "-Wl,-i$ProjectRoot",
    "-Wl,-i$DebugDir\syscfg",
    "-Wl,-i$CompilerDir\lib",
    "-Wl,--diag_wrap=off",
    "-Wl,--display_error_number",
    "-Wl,--warn_sections",
    "-Wl,--rom_model",
    "-o", $OutFile
) + $ObjFiles + @(
    "-Wl,-l$DebugDir\device_linker.cmd",
    "-Wl,-l$DebugDir\device.cmd.genlibs",
    "-Wl,-l$LibC"
)

Write-Host "  Linking H_Track_Car.out ..."
& $CC $LinkArgs
if ($LASTEXITCODE -ne 0) { throw "Linking failed" }

Write-Host ""
Write-Host "=== Build succeeded! ===" -ForegroundColor Green
Write-Host "Output: $OutFile" -ForegroundColor Green

$outInfo = Get-Item $OutFile
Write-Host ("Size: {0:N0} bytes" -f $outInfo.Length)

# ---- Flash ----
$DSLite = "E:\Study\DianSai\TI\ccs\ccs_base\DebugServer\bin\DSLite.exe"
$CCXML  = "$ProjectRoot\MSPM0G3507.ccxml"

Write-Host ""
Write-Host "=== Flashing ===" -ForegroundColor Cyan
& $DSLite flash -c $CCXML -f $OutFile -u
if ($LASTEXITCODE -ne 0) { throw "Flash failed" }
Write-Host "=== Done! ===" -ForegroundColor Green
