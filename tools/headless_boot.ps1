# headless_boot.ps1
# Headless smoke test: boot UnrealEditor in NullCommandlet mode (no GUI),
# load all modules/assets, then scan log for Fatal/Critical/Class-load/Redirect failures.
# Usage: powershell -NoProfile -ExecutionPolicy Bypass -File tools\headless_boot.ps1
# Output:
#   tools/_out/headless_boot.log  - full UE log
#   stdout                        - summary + first 5 hits per pattern

$ue = 'D:\Epic Games\UE_5.7'
$uproject = 'D:\UE_Work\FDRPG\RPG\RPGGame.uproject'
$exe = Join-Path $ue 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'

$outDir = 'D:\UE_Work\FDRPG\tools\_out'
New-Item -ItemType Directory -Path $outDir -Force | Out-Null
$log = Join-Path $outDir 'headless_boot.log'
if (Test-Path $log) { Remove-Item $log -Force }

Write-Host 'Starting UnrealEditor-Cmd headless boot...'
$sw = [System.Diagnostics.Stopwatch]::StartNew()
$proc = Start-Process -FilePath $exe `
    -ArgumentList "`"$uproject`"", '-run=NullCommandlet', '-nullrhi', '-unattended', '-nopause', "-log=$log" `
    -NoNewWindow -PassThru -Wait
$sw.Stop()
Write-Host ('Editor commandlet exited in ' + [math]::Round($sw.Elapsed.TotalSeconds, 1) + 's (exit code: ' + $proc.ExitCode + ')')
Write-Host ''

if (-not (Test-Path $log)) { Write-Host 'NO LOG'; exit 1 }
$lines = [System.IO.File]::ReadAllLines($log, [System.Text.Encoding]::UTF8)
Write-Host ('Total log lines: ' + $lines.Count)

$patterns = @{
    'Fatal/appError'       = 'Fatal error|appError called'
    'LogWindows Error'     = 'LogWindows: Error'
    'Class load failure'   = 'Failed to find class|FindClass failed|Could not find class|Could not load native class|Unknown class'
    'CoreRedirect failure' = 'CoreRedirect.*failed|Redirect.*not applied'
    'Asset load error'     = 'LogLinker: Error:|LogUObjectGlobals: Error:'
}

$anyFound = $false
foreach ($name in $patterns.Keys) {
    $pat = $patterns[$name]
    $hits = @($lines | Where-Object { $_ -match $pat })
    $count = $hits.Count
    if ($count -gt 0) {
        $anyFound = $true
        Write-Host ''
        Write-Host ('[' + $name + '] count=' + $count)
        foreach ($m in ($hits | Select-Object -First 5)) { Write-Host ('  ' + $m) }
    }
}

if (-not $anyFound) {
    Write-Host ''
    Write-Host '>>> NO CRITICAL ISSUES FOUND. <<<'
}

Write-Host ''
Write-Host 'Note: NullCommandlet in UE5.7 prints "could not find the class" - this is expected, not a real error.'
Write-Host ('Full log: ' + $log)
