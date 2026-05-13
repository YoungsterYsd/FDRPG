import unreal

# 方式 A: 用 RPGInputAction 的 set_editor_property 直接传字符串（让 UE 自动转）
struct = unreal.RPGInputAction()
try:
    struct.set_editor_property("input_tag", "InputTag.Move")
    unreal.log(f"[A] OK: set tag from str. now tag = {struct.get_editor_property('input_tag')}")
except Exception as e:
    unreal.log(f"[A] failed: {e}")

# 方式 B: 用 Name 类型构造
try:
    n = unreal.Name("InputTag.Move")
    struct.set_editor_property("input_tag", n)
    unreal.log(f"[B] OK: set tag from Name. tag = {struct.get_editor_property('input_tag')}")
except Exception as e:
    unreal.log(f"[B] failed: {e}")

# 方式 C: 创建 dict / 通过 Tag manager
mgr = unreal.GameplayTagsManager.get()
unreal.log(f"[C] GameplayTagsManager = {mgr}")
if mgr:
    methods = [m for m in dir(mgr) if not m.startswith("_")]
    unreal.log(f"  methods: {methods[:30]}")
