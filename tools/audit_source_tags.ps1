# =============================================================================
# audit_source_tags.ps1
# -----------------------------------------------------------------------------
# Purpose: Scan RPG/Source/**/*.h *.cpp and enumerate:
#   - Native GameplayTag definitions (UE_DEFINE_GAMEPLAY_TAG / _STATIC)
#   - CVar / ConsoleVariable strings ("xxx.yyy")
#   List them alphabetically so we can periodically check for naming-rule drift
#   or leftover Lyra. prefixes.
#
# When to run:
#   - Day 3 found 7 Lyra.* Tags + 31 Lyra./lyra.* CVars to rename.
#   - Run once at the end of every Phase to ensure naming stays clean.
#
# Usage:
#   powershell -NoProfile -ExecutionPolicy Bypass -File tools\audit_source_tags.ps1
# =============================================================================

$src = 'D:\UE_Work\FDRPG\RPG\Source'
$files = Get-ChildItem -Path $src -Recurse -Include '*.h','*.cpp'

$tags = @{}
$cvars = @{}
foreach ($f in $files) {
    $text = [System.IO.File]::ReadAllText($f.FullName, [System.Text.Encoding]::UTF8)
    $tagMatches = [regex]::Matches($text, 'UE_DEFINE_GAMEPLAY_TAG[A-Z_]*\s*\(\s*\w+\s*,\s*"([^"]+)"')
    foreach ($m in $tagMatches) {
        $t = $m.Groups[1].Value
        if (-not $tags.ContainsKey($t)) { $tags[$t] = @() }
        $tags[$t] += $f.Name
    }
    $cvarMatches = [regex]::Matches($text, '"([A-Za-z][\w]*\.[A-Za-z][\w\.]*)"')
    foreach ($m in $cvarMatches) {
        $c = $m.Groups[1].Value
        if ($tags.ContainsKey($c)) { continue }
        if (-not $cvars.ContainsKey($c)) { $cvars[$c] = @() }
        $cvars[$c] += $f.Name
    }
}

Write-Host ('=== GameplayTag (Native Define) total: ' + $tags.Count + ' ===')
$tags.Keys | Sort-Object | ForEach-Object { Write-Host ('  ' + $_) }

Write-Host ''
Write-Host ('=== CVar / ConsoleVariable strings total: ' + $cvars.Count + ' ===')
$cvars.Keys | Sort-Object | ForEach-Object { Write-Host ('  ' + $_) }

Write-Host ''
Write-Host 'Hint: check Lyra./lyra. prefix residue (expected none)'
$lyraResidues = @()
foreach ($k in $tags.Keys + $cvars.Keys) {
    if ($k -match '^(Lyra|lyra)\.') { $lyraResidues += $k }
}
if ($lyraResidues.Count -gt 0) {
    Write-Host ('  [WARN] Lyra residue count: ' + $lyraResidues.Count)
    $lyraResidues | ForEach-Object { Write-Host ('    ' + $_) }
} else {
    Write-Host '  [OK] No Lyra./lyra. residue'
}
