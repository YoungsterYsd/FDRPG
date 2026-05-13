# 扫描 RPG/Content 下名称含 Lyra 或 L_ 前缀的资产
$root = 'd:\UE_Work\FDRPG\RPG\Content'
$files = Get-ChildItem -Path $root -Recurse -File -ErrorAction SilentlyContinue |
    Where-Object { $_.Name -match 'Lyra|^L_|^B_Lyra|^M_Lyra|^SK_Lyra|^SM_Lyra' }

Write-Host "==== Total Lyra-named assets: $($files.Count) ===="
Write-Host ""
Write-Host "==== By extension ===="
$files | Group-Object Extension | Sort-Object Count -Descending | Format-Table -AutoSize

Write-Host "==== By top-level folder ===="
$files | ForEach-Object {
    $rel = $_.FullName.Substring($root.Length + 1)
    $top = ($rel -split '[\\/]')[0]
    [PSCustomObject]@{ Folder = $top; Name = $_.Name }
} | Group-Object Folder | Sort-Object Count -Descending | Select-Object Count, Name | Format-Table -AutoSize
