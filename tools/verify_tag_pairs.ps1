# 验证 RPGGameplayTags.h/.cpp 的 EXTERN/DEFINE 数量是否一致
$hPath = 'd:\UE_Work\FDRPG\RPG\Source\RPGGame\RPGGameplayTags.h'
$cppPath = 'd:\UE_Work\FDRPG\RPG\Source\RPGGame\RPGGameplayTags.cpp'

$hExterns = @(Select-String -Path $hPath -Pattern 'UE_DECLARE_GAMEPLAY_TAG_EXTERN')
$cppDefines = @(Select-String -Path $cppPath -Pattern 'UE_DEFINE_GAMEPLAY_TAG_COMMENT')

Write-Host "Header (.h) EXTERN count : $($hExterns.Count)"
Write-Host "Cpp    (.cpp) DEFINE count: $($cppDefines.Count)"

if ($hExterns.Count -eq $cppDefines.Count) {
    Write-Host "[OK] Pair counts match" -ForegroundColor Green
} else {
    Write-Host "[FAIL] Pair counts mismatch" -ForegroundColor Red
}

# 提取每个 Tag 标识符并交叉对比
$hNames = $hExterns | ForEach-Object {
    if ($_ -match 'UE_DECLARE_GAMEPLAY_TAG_EXTERN\((\w+)\)') { $matches[1] }
}
$cppNames = $cppDefines | ForEach-Object {
    if ($_ -match 'UE_DEFINE_GAMEPLAY_TAG_COMMENT\((\w+)\s*,') { $matches[1] }
}

$onlyInH = Compare-Object $hNames $cppNames -PassThru | Where-Object { $_ }
if (-not $onlyInH) {
    Write-Host "[OK] All EXTERN/DEFINE identifiers paired" -ForegroundColor Green
} else {
    Write-Host "[FAIL] Mismatched identifiers:" -ForegroundColor Red
    $onlyInH | ForEach-Object { Write-Host "  $_" }
}
