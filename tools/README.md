# tools/ · FDRPG 项目运维与诊断脚本目录

> **维护者**：implementer
> **归属**：FDRPG 工程长期运维脚本，**入库**（与 `day*_*` 一次性改造脚本不同）
> **入口**：所有脚本都从工作区根 `D:\UE_Work\FDRPG\` 调用，路径写死无需参数
> **输出位置**：所有脚本输出统一到 `tools/_out/`（已 gitignore）

---

## 1. 脚本目录索引

| 脚本 | 类别 | 用途 | 何时跑 |
|---|---|---|---|
| [`audit_redirects.ps1`](#audit_redirectsps1) | 🔍 审计 | 检查 `RPG/Source` 中的 RPG 类与 `DefaultEngine.ini` 中 ClassRedirect 是否齐全 | 大批量改名后 / 每个 Phase 末 |
| [`audit_configs.ps1`](#audit_configsps1) | 🔍 审计 | 扫 `RPG/Config/*.ini` 中残留的 `Lyra*` 标识符（TraceChannel / CollisionProfile / CVar / Tag） | 每次改 Config / 加平台后 |
| [`audit_source_tags.ps1`](#audit_source_tagsps1) | 🔍 审计 | 列出 `RPG/Source` 中所有 Native GameplayTag 与 CVar 字符串，检查 Lyra. 残留 | 每次新增 Tag/CVar 后 |
| [`build_editor.ps1`](#build_editorps1) | 🛠 构建 | 一键 GenerateProjectFiles + UBT 编译 RPGEditor (Win64 Development) | 改 Build.cs / .uproject / 大量源码后 |
| [`headless_boot.ps1`](#headless_bootps1) | 🛠 验证 | 用 NullCommandlet 模式启动 Editor 完整冒烟，扫日志判定 0 Fatal/Critical | 每次大改后回归 |
| [`kill_editor.ps1`](#kill_editorps1) | 🛠 工具 | 一键关闭所有 UnrealEditor 进程 | 编译/MCP 调试切换前 |

---

## 2. 工作流速查

### 「改完源码 / .Build.cs / .uproject」标准回归流程
```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools\kill_editor.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File tools\build_editor.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File tools\headless_boot.ps1
```

### 「大批量改名 / 改 Config」回归流程
```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools\audit_redirects.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File tools\audit_configs.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File tools\audit_source_tags.ps1
```

### 「Phase 末里程碑」全量留底
```powershell
# 跑齐上面 6 个，把 _out/ 下产物归档到 策划案/ 报告附录
```

---

## 3. 脚本详细说明

### audit_redirects.ps1

**输入**：扫 `RPG/Source/RPGGame/**/*.h`，正则 `class (RPGGAME_API)? [AUIF](RPG\w+) :` 提取所有 RPG 公开类。

**逻辑**：每个类反推对应的 Lyra 原名（`RPGCharacter` → `LyraCharacter`），在 `DefaultEngine.ini` 里查是否有对应的 `+ClassRedirects=(OldName="/Script/LyraGame.LyraXxx",NewName="/Script/RPGGame.RPGXxx")`。

**输出**：
- stdout：`Total: 173 / Present: 173 / Missing: 0`
- 缺失项追加到 `tools/_out/missing_redirects.ini`（可直接复制粘贴回 ini）

**历史命中**：Day 3 用过一次，发现 37 条 redirect 漏掉（短名基类被同名前缀派生类抢匹配）

---

### audit_configs.ps1

**输出 3 段**：
1. `/Script/LyraGame.*` 非 redirect 引用（说明类 ini 还指向 Lyra 模块）
2. `Lyra*` 标识符（Section 基类 / TraceChannel / CollisionProfile / 路径过滤）
3. `Lyra.` 前缀的 GameplayTag（DefaultGameplayTags.ini / DefaultDeviceProfiles.ini）

**已知合法保留**（输出会包含但属于决策保留）：
- `LyraExtTool/*` 路径（DP-1 决策）
- `Lyra.TestEncryption` 注释行

---

### audit_source_tags.ps1

**输出**：
- 所有 `UE_DEFINE_GAMEPLAY_TAG[_STATIC](_, "X.Y.Z")` 字符串去重列表
- 所有 `"X.Y.Z"` 形式的 CVar 字符串去重列表
- 末尾自动检查 `Lyra./lyra.` 前缀残留（应为 0）

**历史命中**：Day 3 用过一次，发现 7 个 Lyra. Tag + 31 个 Lyra./lyra. CVar 待改名

---

### build_editor.ps1

**两步**：
1. `UnrealEditor.exe -ProjectFiles -project=RPGGame.uproject -game -engine -progress` 重新生成 `.sln`
2. `UnrealBuildTool.exe RPGEditor Win64 Development -project=RPGGame.uproject -progress -NoHotReload` 全量编译

**输出**：`tools/_out/build.log`（完整 UBT 日志）

---

### headless_boot.ps1

**逻辑**：
1. 用 `UnrealEditor-Cmd.exe -run=NullCommandlet -nullrhi -unattended -nopause -log=...` 启动
2. NullCommandlet 在 5.7 已移除，引擎完整初始化后会以 exit code 1 退出（**这是预期行为**）
3. 扫日志关键词：Fatal / Critical / Class load failure / CoreRedirect failure / Asset load error

**期望输出**：`>>> NO CRITICAL ISSUES FOUND. <<<`

**输出**：`tools/_out/headless_boot.log`

---

### kill_editor.ps1

无参数，遍历 `Get-Process -Name UnrealEditor` 全部 `Stop-Process -Force`，再 sleep 3 秒确认无残留。

---

## 4. 维护规则

- ✅ 添加新工具脚本时，**必须**在本 README 的索引表里同步登记，并写"何时跑"
- ✅ 一次性改造脚本（如 Day1~4 的步骤脚本）**不要**放进 tools/，应该写在工作区根目录用 `day*_*` 命名（`.gitignore` 已忽略）
- ✅ 输出统一到 `tools/_out/`，该目录会在脚本首次运行时自动创建，已加入 `.gitignore`
- ❌ 不要在脚本里 hard-code 个人路径（除了 UE 引擎路径 `D:\Epic Games\UE_5.7\` 这种项目级常量）

## 5. 后续可能新增的工具（占位）

- `audit_assets_orphans.ps1`：扫 Content 下没有引用者的孤儿资产
- `audit_log_categories.ps1`：列出所有 LogXxx 类别声明，检查是否符合 LogRPG* 命名
- `pak_validate.ps1`：打包后 .pak 内容审计
- `package_steam.ps1` / `package_ios.ps1`：Steam / iOS 打包流水线脚本

---

## 变更日志

| 版本 | 日期 | 说明 |
|---|---|---|
| v1.0 | 2026-05-12 | 从 Day 1~4 改造脚本中提炼出 6 个有复用价值的工具，一次性清理工作区根目录 67 个临时文件 |
