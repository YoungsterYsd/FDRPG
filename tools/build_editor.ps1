# =============================================================================
# build_editor.ps1
# -----------------------------------------------------------------------------
# 用途：重新生成 VS Project Files + 全量编译 RPGEditor (Win64 Development)
# 适用：源码大改后 / .Build.cs 改后 / .uproject 改后
# 输出：tools/_out/build.log
# 使用：powershell -NoProfile -ExecutionPolicy Bypass -File tools\build_editor.ps1
# =============================================================================

$ue = 'D:\Epic Games\UE_5.7'
$uproject = 'D:\UE_Work\FDRPG\RPG\RPGGame.uproject'
$ubt = Join-Path $ue 'Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe'
$genFiles = Join-Path $ue 'Engine\Binaries\Win64\UnrealEditor.exe'

$outDir = 'D:\UE_Work\FDRPG\tools\_out'
New-Item -ItemType Directory -Path $outDir -Force | Out-Null
$logFile = Join-Path $outDir 'build.log'

Write-Host '=== Regenerating VS project files ==='
& $genFiles -ProjectFiles -project="$uproject" -game -engine -progress 2>&1 | Select-Object -Last 10
Write-Host ''

Write-Host '=== Compiling RPGEditor Win64 Development ==='
& $ubt RPGEditor Win64 Development -project="$uproject" -progress -NoHotReload 2>&1 | Tee-Object -FilePath $logFile | Select-Object -Last 30
Write-Host ''
Write-Host ('Full log: ' + $logFile)
