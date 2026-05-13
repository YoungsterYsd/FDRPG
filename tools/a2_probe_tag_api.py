import unreal

# 列出所有 GameplayTag 相关的 API
names = [n for n in dir(unreal) if "GameplayTag" in n and not n.startswith("_")]
unreal.log(f"[probe-tag] found {len(names)} GameplayTag-related names:")
for n in sorted(names):
    unreal.log(f"   {n}")

# 尝试几种构造方式
unreal.log("--- attempt 1: GameplayTagsLibrary.request_gameplay_tag ---")
lib = getattr(unreal, "GameplayTagsLibrary", None)
unreal.log(f"GameplayTagsLibrary = {lib}")
if lib:
    methods = [m for m in dir(lib) if not m.startswith("_")]
    unreal.log(f"  methods: {methods}")

unreal.log("--- attempt 2: BlueprintGameplayTagLibrary ---")
lib2 = getattr(unreal, "BlueprintGameplayTagLibrary", None)
unreal.log(f"BlueprintGameplayTagLibrary = {lib2}")
if lib2:
    methods = [m for m in dir(lib2) if not m.startswith("_") and "tag" in m.lower()]
    unreal.log(f"  tag-related methods: {methods}")

# 尝试直接用结构体的 set_editor_property 设置 tag_name
unreal.log("--- attempt 3: GameplayTag struct properties ---")
gt = unreal.GameplayTag()
unreal.log(f"GameplayTag instance = {gt}")
# 列出属性
for attr in ["tag_name", "TagName"]:
    try:
        v = gt.get_editor_property(attr)
        unreal.log(f"  get {attr} = {v}")
    except Exception as e:
        unreal.log(f"  get {attr} failed: {e}")
