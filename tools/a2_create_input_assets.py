# A2-1b · 批量创建 14 个 RPG InputAction 资产 + 1 个 IMC + 配置 InputData_Hero
# 用法（在 Editor Python Console 或 -ExecutePythonScript 跑）：
#   ExecutePythonScript "D:\UE_Work\FDRPG\tools\a2_create_input_assets.py"
#
# 设计说明：
#   - 仅做资产创建 + InputConfig 数组项写入（属性骨架）
#   - 键位绑定（IMC.Mappings）、Modifier、Triggers、PlayerMappableKeySettings 由人在 Editor 里一次性手动配置
#     原因：Python 对结构体数组（FEnhancedActionKeyMapping 等）的赋值容易序列化失败导致资产损坏
#   - 所有 14 个 IA 的 ValueType 默认 Bool（可后续按需改 Axis2D for IA_Move）
#
# 14 个 IA 清单（详见 14_后续日程与验收清单.md §2.1）：
#   Native (5):  IA_Move / IA_Pickup / IA_SwitchWeapon / IA_OpenMenu / IA_OpenInventory
#   Ability (9): IA_Attack / IA_QSkill / IA_WSkill / IA_ESkill / IA_RSkill
#                / IA_Ultimate / IA_Dodge / IA_Block / IA_HealItem

import unreal

# ---------- 配置 ----------
TARGET_FOLDER       = "/Game/Input/Actions/RPG"
IMC_FOLDER          = "/Game/Input/Mappings"
INPUT_CONFIG_PATH   = "/Game/Input/InputData_Hero"
IMC_NAME            = "IMC_RPG_Default"

# 14 个 InputAction 的元数据（Name / InputTag / 默认键位提示 / ValueType / 类别）
IA_DEFINITIONS = [
    # Native (5)
    ("IA_Move",            "InputTag.Move",                 "Arrow Keys / WASD",     "Axis2D",  "Native"),
    ("IA_Pickup",          "InputTag.Native.Pickup",        "F",                     "Bool",    "Native"),
    ("IA_SwitchWeapon",    "InputTag.Native.SwitchWeapon",  "Tab",                   "Bool",    "Native"),
    ("IA_OpenMenu",        "InputTag.Native.OpenMenu",      "Esc",                   "Bool",    "Native"),
    ("IA_OpenInventory",   "InputTag.Native.OpenInventory", "I",                     "Bool",    "Native"),
    # Ability (9)
    ("IA_Attack",          "InputTag.Ability.Attack",       "Mouse Left / A",        "Bool",    "Ability"),
    ("IA_QSkill",          "InputTag.Ability.Q",            "Q",                     "Bool",    "Ability"),
    ("IA_WSkill",          "InputTag.Ability.W",            "W",                     "Bool",    "Ability"),
    ("IA_ESkill",          "InputTag.Ability.E",            "E",                     "Bool",    "Ability"),
    ("IA_RSkill",          "InputTag.Ability.R",            "R",                     "Bool",    "Ability"),
    ("IA_Ultimate",        "InputTag.Ability.Ultimate",     "Space",                 "Bool",    "Ability"),
    ("IA_Dodge",           "InputTag.Ability.Dodge",        "Shift",                 "Bool",    "Ability"),
    ("IA_Block",           "InputTag.Ability.Block",        "Ctrl",                  "Bool",    "Ability"),
    ("IA_HealItem",        "InputTag.Ability.HealItem",     "H",                     "Bool",    "Ability"),
]

VALUE_TYPE_MAP = {
    "Bool":   unreal.InputActionValueType.BOOLEAN,
    "Axis1D": unreal.InputActionValueType.AXIS1D,
    "Axis2D": unreal.InputActionValueType.AXIS2D,
    "Axis3D": unreal.InputActionValueType.AXIS3D,
}

asset_tools  = unreal.AssetToolsHelpers.get_asset_tools()
editor_lib   = unreal.EditorAssetLibrary

# ---------- Step 1: 确保目录存在 ----------
for folder in [TARGET_FOLDER, IMC_FOLDER]:
    if not editor_lib.does_directory_exist(folder):
        editor_lib.make_directory(folder)
        unreal.log(f"[A2-1b] Created folder: {folder}")

# ---------- Step 2: 创建 14 个 InputAction ----------
created_ias = {}
for ia_name, tag_str, key_hint, value_type, category in IA_DEFINITIONS:
    asset_path = f"{TARGET_FOLDER}/{ia_name}"
    if editor_lib.does_asset_exist(asset_path):
        unreal.log_warning(f"[A2-1b] IA already exists, skipping: {asset_path}")
        ia_asset = editor_lib.load_asset(asset_path)
    else:
        ia_asset = asset_tools.create_asset(
            asset_name      = ia_name,
            package_path    = TARGET_FOLDER,
            asset_class     = unreal.InputAction,
            factory         = None,  # InputActionFactory 未暴露给 Python；传 None 让 UE 选默认
        )
        if ia_asset is None:
            unreal.log_error(f"[A2-1b] Failed to create {asset_path}")
            continue
        unreal.log(f"[A2-1b] Created IA: {asset_path}  | tag={tag_str} | key={key_hint} | type={value_type}")

    # 设置 ValueType
    ia_asset.set_editor_property("value_type", VALUE_TYPE_MAP[value_type])
    editor_lib.save_loaded_asset(ia_asset)
    created_ias[ia_name] = (ia_asset, tag_str, category)

# ---------- Step 3: 写入 InputData_Hero（URPGInputConfig）的 NativeInputActions / AbilityInputActions ----------
input_config = editor_lib.load_asset(INPUT_CONFIG_PATH)
if input_config is None:
    unreal.log_error(f"[A2-1b] InputData_Hero not found at {INPUT_CONFIG_PATH}, skip InputConfig wire-up")
else:
    # 先清空（脚本可重入）
    input_config.clear_all_mappings()
    # 通过 C++ BlueprintCallable API 添加映射（FName 形式 Tag，由 C++ 内部解析为 FGameplayTag）
    n_native, n_ability = 0, 0
    for ia_name, (ia_asset, tag_str, category) in created_ias.items():
        tag_name = unreal.Name(tag_str)
        if category == "Native":
            input_config.add_native_mapping(ia_asset, tag_name)
            n_native += 1
        else:
            input_config.add_ability_mapping(ia_asset, tag_name)
            n_ability += 1
    editor_lib.save_loaded_asset(input_config)
    unreal.log(f"[A2-1b] InputData_Hero updated: {n_native} native + {n_ability} ability mappings")

# ---------- Step 4: 创建 IMC_RPG_Default（仅骨架，Mappings 留空待人工填） ----------
imc_path = f"{IMC_FOLDER}/{IMC_NAME}"
if editor_lib.does_asset_exist(imc_path):
    unreal.log_warning(f"[A2-1b] IMC already exists, skipping: {imc_path}")
else:
    imc = asset_tools.create_asset(
        asset_name   = IMC_NAME,
        package_path = IMC_FOLDER,
        asset_class  = unreal.InputMappingContext,
        factory      = None,  # InputMappingContextFactory 未暴露给 Python；传 None 让 UE 选默认
    )
    if imc is None:
        unreal.log_error(f"[A2-1b] Failed to create IMC at {imc_path}")
    else:
        editor_lib.save_loaded_asset(imc)
        unreal.log(f"[A2-1b] Created IMC: {imc_path}  (Mappings 待人工配置)")

unreal.log("[A2-1b] DONE. Next: open Editor and bind keys in IMC_RPG_Default manually (or run a2_bind_imc.py)")
