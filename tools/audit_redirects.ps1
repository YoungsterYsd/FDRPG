# =============================================================================
# audit_redirects.ps1
# -----------------------------------------------------------------------------
# 用途：审计 RPG/Source 中所有 RPG* 公开类，反推对应的 Lyra 原名，
#       并检查 RPG/Config/DefaultEngine.ini 中的 ClassRedirect 是否齐全。
#
# 适用场景：
#   1) 大批量改名后核对 CoreRedirect 覆盖率（如 Day 2 完成后用过一次，发现 37 条漏掉）
#   2) 新增 RPG 类后回归扫一次，确认 redirect 是否需要补
#   3) Phase A2/B 之后，每个里程碑结束跑一次留底
#
# 输出：
#   stdout：Total / Present / Missing 数量，附 Missing 完整 +ClassRedirects= 行
#   tools/_out/missing_redirects.ini：Missing 行单独成文件，方便复制粘贴回 ini
#
# 使用：
#   powershell -NoProfile -ExecutionPolicy Bypass -File tools\audit_redirects.ps1
# =============================================================================

$src = 'D:\UE_Work\FDRPG\RPG\Source\RPGGame'
$ini = Get-Content 'D:\UE_Work\FDRPG\RPG\Config\DefaultEngine.ini' -Encoding UTF8 -Raw

$pattern = '^\s*class\s+(?:RPGGAME_API\s+)?([AUIF])(RPG\w+)\s*(?::|$)'
$hits = Get-ChildItem -Path $src -Recurse -Include '*.h' | Select-String -Pattern $pattern

$allClasses = @{}
foreach ($h in $hits) {
    if ($h.Matches[0].Groups[2].Value) {
        $rpgName = $h.Matches[0].Groups[2].Value
        $lyraName = 'Lyra' + $rpgName.Substring(3)
        $allClasses[$rpgName] = $lyraName
    }
}

$missing = @()
$present = 0
foreach ($k in $allClasses.Keys | Sort-Object) {
    $lyra = $allClasses[$k]
    $needleOld = '/Script/LyraGame.' + $lyra
    $needleNew = '/Script/RPGGame.' + $k
    $pat = [regex]::Escape($needleOld) + '"'
    if ($ini -match $pat) {
        $present++
    } else {
        $missing += ('+ClassRedirects=(OldName="' + $needleOld + '",NewName="' + $needleNew + '")')
    }
}

Write-Host ('Total RPG classes found: ' + $allClasses.Count)
Write-Host ('Present in redirects:    ' + $present)
Write-Host ('Missing:                 ' + $missing.Count)

if ($missing.Count -gt 0) {
    Write-Host ''
    Write-Host '--- MISSING REDIRECTS ---'
    $missing | ForEach-Object { Write-Host $_ }

    $outDir = 'D:\UE_Work\FDRPG\tools\_out'
    New-Item -ItemType Directory -Path $outDir -Force | Out-Null
    $outFile = Join-Path $outDir 'missing_redirects.ini'
    $missing | Out-File -FilePath $outFile -Encoding UTF8
    Write-Host ''
    Write-Host ('Saved to: ' + $outFile)
}
