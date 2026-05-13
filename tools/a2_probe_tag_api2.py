import unreal

lib = unreal.GameplayTagLibrary
unreal.log(f"GameplayTagLibrary = {lib}")
methods = [m for m in dir(lib) if not m.startswith("_")]
unreal.log(f"  {len(methods)} methods:")
for m in sorted(methods):
    unreal.log(f"   {m}")
