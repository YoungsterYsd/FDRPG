# 探测 EnhancedInput 在 Python 里可用的工厂类与 RPGInputAction 结构体
import unreal

candidates = [
    "InputAction", "InputMappingContext",
    "InputActionFactory", "InputMappingContextFactory",
    "RPGInputAction", "RPGInputConfig", "GameplayTag",
]
for n in candidates:
    cls = getattr(unreal, n, None)
    unreal.log(f"[probe] {n} = {cls}")

# 尝试找到 InputAction 的 default factory
asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
unreal.log(f"[probe] asset_tools = {asset_tools}")

# 列出 unreal 模块里所有名字含 Input 的
names = [n for n in dir(unreal) if "Input" in n and not n.startswith("_")]
unreal.log(f"[probe] {len(names)} Input-related names:")
for n in sorted(names)[:60]:
    unreal.log(f"   {n}")
