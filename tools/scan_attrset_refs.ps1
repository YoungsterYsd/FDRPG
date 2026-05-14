# 扫描 .uasset 二进制中是否引用 RPGHealthSet / RPGCombatSet 字段名
$root = 'd:\UE_Work\FDRPG\RPG\Content'
$patterns = @('RPGHealthSet', 'RPGCombatSet')
$results = @{}
foreach ($p in $patterns) { $results[$p] = @() }

$files = Get-ChildItem $root -Recurse -Filter *.uasset -ErrorAction SilentlyContinue
$total = $files.Count
$idx = 0
foreach ($f in $files) {
    $idx++
    if ($idx % 500 -eq 0) { Write-Host "Scanning $idx / $total..." }
    try {
        $bytes = [System.IO.File]::ReadAllBytes($f.FullName)
        $text = [System.Text.Encoding]::UTF8.GetString($bytes)
        foreach ($p in $patterns) {
            if ($text -match $p) {
                $results[$p] += $f.FullName.Substring($root.Length + 1)
            }
        }
    } catch {}
}

Write-Host ""
foreach ($p in $patterns) {
    $list = $results[$p]
    Write-Host "==== $p : $($list.Count) uasset reference(s) ====" -ForegroundColor Cyan
    $list | Select-Object -First 30 | ForEach-Object { Write-Host "  $_" }
    if ($list.Count -gt 30) { Write-Host "  ... and $($list.Count - 30) more" }
    Write-Host ""
}
