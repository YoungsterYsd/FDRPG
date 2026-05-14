# A3 启动前置校验
$ErrorActionPreference = 'Continue'

Write-Host "==== Editor process ====" -ForegroundColor Cyan
$editorProcs = Get-Process -ErrorAction SilentlyContinue | Where-Object {
    $_.ProcessName -match 'UnrealEditor|UE4Editor|UE5Editor|RPGEditor'
}
if ($editorProcs) {
    $editorProcs | Format-Table Id, ProcessName, StartTime -AutoSize
} else {
    Write-Host "[OK] No Editor running" -ForegroundColor Green
}

Write-Host ""
Write-Host "==== Source/RPG layout ====" -ForegroundColor Cyan
$src = 'd:\UE_Work\FDRPG\RPG\Source\RPGGame'
if (Test-Path $src) {
    Get-ChildItem $src -Directory | Select-Object Name | Format-Table -AutoSize
} else {
    Write-Host "[FAIL] Source/RPG not found at $src" -ForegroundColor Red
}

Write-Host ""
Write-Host "==== Existing AttributeSet files ====" -ForegroundColor Cyan
$attrs = Get-ChildItem -Path $src -Recurse -File -Include 'RPGAttributeSet*','RPGHealthSet*','RPGCombatSet*','RPGPrimaryAttributeSet*' -ErrorAction SilentlyContinue
if ($attrs) {
    $attrs | ForEach-Object { $_.FullName.Substring($src.Length + 1) } | Format-Table -AutoSize
} else {
    Write-Host "(none, will be created in A3-1/A3-2)" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "==== Existing DamageExecution / GE / Block GA ====" -ForegroundColor Cyan
$ge = Get-ChildItem -Path $src -Recurse -File -Include 'RPGDamageExecution*','RPGGameplayAbility_Block*','RPGCombatStateSubsystem*','RPGDamageNumberSubsystem*' -ErrorAction SilentlyContinue
if ($ge) {
    $ge | ForEach-Object { $_.FullName.Substring($src.Length + 1) } | Format-Table -AutoSize
} else {
    Write-Host "(none, will be created in A3-4~A3-8)" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "==== Lyra base class residual scan in Source/RPG ====" -ForegroundColor Cyan
$lyraRefs = Select-String -Path "$src\*\*.h","$src\*\*.cpp","$src\*\*\*.h","$src\*\*\*.cpp" -Pattern 'ULyraHealthSet|ULyraCombatSet|ULyraDamageExecution|ULyraAttributeSet' -ErrorAction SilentlyContinue
if ($lyraRefs) {
    $lyraRefs | Group-Object Path | Select-Object Count, Name | Format-Table -AutoSize
} else {
    Write-Host "[OK] No Lyra base class residual" -ForegroundColor Green
}

Write-Host ""
Write-Host "==== Build.cs deps for AbilitySystem / GameplayAbilities ====" -ForegroundColor Cyan
$buildCs = Get-ChildItem -Path $src -Recurse -File -Include 'RPG.Build.cs' -ErrorAction SilentlyContinue
if ($buildCs) {
    foreach ($f in $buildCs) {
        Write-Host "--- $($f.Name) ---"
        Select-String -Path $f.FullName -Pattern 'GameplayAbilities|GameplayTags|GameplayTasks|ModularGameplay|GameplayMessage|UMG|EnhancedInput' | ForEach-Object { $_.Line.Trim() }
    }
} else {
    Write-Host "(RPG.Build.cs not found)" -ForegroundColor Red
}
