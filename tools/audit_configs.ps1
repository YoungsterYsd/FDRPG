# =============================================================================
# audit_configs.ps1
# -----------------------------------------------------------------------------
# 用途：审计 RPG/Config 下所有 .ini 文件中残留的 Lyra 标识符，分三段输出：
#   1) /Script/LyraGame.* 的非 redirect 引用（说明类还指向 Lyra 模块）
#   2) Lyra* 标识符（TraceChannel / CollisionProfile / CVar / Section 基类等）
#   3) Lyra. 前缀的 GameplayTag（DefaultGameplayTags.ini / Tags 目录）
#
# 适用场景：
#   - Lyra → RPG 改造期 Config 残留排查（Day 3 用过一次，命中 80+ 处）
#   - 后续每次改 Config / 加新平台时跑一次回归
#
# 已知合法保留项（输出会包含但属于决策保留）：
#   - LyraExtTool/* 路径（DP-1 决策，保留 Lyra 编辑器扩展插件）
#   - Lyra.TestEncryption（注释行）
#
# 使用：
#   powershell -NoProfile -ExecutionPolicy Bypass -File tools\audit_configs.ps1
# =============================================================================

$configDir = 'D:\UE_Work\FDRPG\RPG\Config'
$files = Get-ChildItem -Path $configDir -Recurse -Include '*.ini'

Write-Host '=============================='
Write-Host '1. Non-redirect "/Script/LyraGame." references (need fix):'
Write-Host '=============================='
foreach ($f in $files) {
    $lines = Get-Content $f.FullName -Encoding UTF8
    for ($i = 0; $i -lt $lines.Count; $i++) {
        if ($lines[$i] -match '/Script/LyraGame\.' -and $lines[$i] -notmatch 'OldName="/Script/LyraGame') {
            Write-Host ('  ' + $f.Name + ':' + ($i+1) + ': ' + $lines[$i])
        }
    }
}

Write-Host ''
Write-Host '=============================='
Write-Host '2. Lyra* identifiers outside redirects (TraceChannel / CollisionProfile / CVar / Section base):'
Write-Host '=============================='
foreach ($f in $files) {
    $lines = Get-Content $f.FullName -Encoding UTF8
    for ($i = 0; $i -lt $lines.Count; $i++) {
        if ($lines[$i] -match 'Redirects=\(') { continue }
        if ($lines[$i] -match '^\s*[^;]' -and $lines[$i] -match '\bLyra[A-Z_a-z]') {
            Write-Host ('  ' + $f.Name + ':' + ($i+1) + ': ' + $lines[$i])
        }
    }
}

Write-Host ''
Write-Host '=============================='
Write-Host '3. "Lyra." GameplayTag prefix in Tag / DeviceProfile files:'
Write-Host '=============================='
$tagDirs = @($configDir, (Join-Path $configDir 'Tags'))
foreach ($td in $tagDirs) {
    if (-not (Test-Path $td)) { continue }
    $tagFiles = Get-ChildItem -Path $td -Filter '*.ini' -ErrorAction SilentlyContinue -Recurse
    foreach ($f in $tagFiles) {
        $lines = Get-Content $f.FullName -Encoding UTF8
        for ($i = 0; $i -lt $lines.Count; $i++) {
            if ($lines[$i] -match 'Lyra\.[A-Z]') {
                Write-Host ('  ' + $f.FullName + ':' + ($i+1) + ': ' + $lines[$i])
            }
        }
    }
}
