param(
    [int]$ProcId = 26712
)
$log = 'd:\UE_Work\FDRPG\tools\_out\build.log'

if (Get-Process -Id $ProcId -ErrorAction SilentlyContinue) {
    Write-Host "[BUILDING] PID $ProcId still running..."
    if (Test-Path $log) {
        $size = (Get-Item $log).Length
        Write-Host "build.log size: $size bytes"
        Write-Host "--- Last 20 lines ---"
        Get-Content $log -Tail 20 -Encoding UTF8
    }
} else {
    Write-Host "[DONE] PID $ProcId finished"
    if (Test-Path $log) {
        Write-Host "--- Last 100 lines ---"
        Get-Content $log -Tail 100 -Encoding UTF8
    }
}
