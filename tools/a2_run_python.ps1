# A2 · 运行 Python 脚本到当前项目（commandlet 模式，无 GUI）
# 用法：
#   powershell -File tools\a2_run_python.ps1 -Script "tools\a2_probe_input_api.py"
#
# 注意：commandlet 模式下 AssetTools 完全可用，但部分依赖 GUI 的 API（如 PIE）不可用
# 输出会写到 tools\_out\a2_python_<scriptname>.log

param(
    [Parameter(Mandatory=$true)]
    [string]$Script
)

$ErrorActionPreference = 'Stop'
$ProjectFile  = 'D:\UE_Work\FDRPG\RPG\RPGGame.uproject'
$EditorExe    = 'D:\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
$OutDir       = 'D:\UE_Work\FDRPG\tools\_out'
New-Item -ItemType Directory -Path $OutDir -Force | Out-Null

$ScriptFull = (Resolve-Path $Script).Path
$ScriptName = [System.IO.Path]::GetFileNameWithoutExtension($ScriptFull)
$LogFile    = Join-Path $OutDir "a2_python_$ScriptName.log"

Write-Host "=== Running Python in UE commandlet ==="
Write-Host "Script : $ScriptFull"
Write-Host "Log    : $LogFile"

$args = @(
    "`"$ProjectFile`"",
    "-run=pythonscript",
    "-script=`"$ScriptFull`"",
    "-stdout",
    "-FullStdOutLogOutput",
    "-NoSplash",
    "-Unattended"
)

$proc = Start-Process -FilePath $EditorExe -ArgumentList $args -NoNewWindow -PassThru -Wait `
    -RedirectStandardOutput $LogFile -RedirectStandardError "$LogFile.err"

Write-Host "Exit code: $($proc.ExitCode)"
if (Test-Path $LogFile) {
    Write-Host '--- last 80 lines of log ---'
    Get-Content $LogFile -Tail 80
}
if ((Test-Path "$LogFile.err") -and (Get-Item "$LogFile.err").Length -gt 0) {
    Write-Host '--- stderr ---'
    Get-Content "$LogFile.err"
}
