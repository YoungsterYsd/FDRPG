# =============================================================================
# kill_editor.ps1
# -----------------------------------------------------------------------------
# 用途：一键关闭所有 UnrealEditor 进程。
# 适用：编译前 / MCP 调试切换 / Editor 卡死时强杀
# 使用：powershell -NoProfile -ExecutionPolicy Bypass -File tools\kill_editor.ps1
# =============================================================================

$procs = Get-Process -Name UnrealEditor -ErrorAction SilentlyContinue
foreach ($p in $procs) {
    Write-Host ('Stopping PID ' + $p.Id)
    Stop-Process -Id $p.Id -Force
}
Start-Sleep -Seconds 3
$remain = (Get-Process -Name UnrealEditor -ErrorAction SilentlyContinue | Measure-Object).Count
Write-Host ('Remaining UnrealEditor processes: ' + $remain)
