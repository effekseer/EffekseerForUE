"""Compile standard Effekseer materials in an Unreal Editor commandlet.

Example (UE 4.27; use UnrealEditor-Cmd.exe for UE 5):
UE4Editor-Cmd.exe <project.uproject> -run=pythonscript
  -script=<repo>/scripts/validate_ue_materials.py -AllowCommandletRendering
  -EnablePlugins=PythonScriptPlugin,EditorScriptingUtilities -unattended -nop4

Checks every base material and refreshes its instances, without saving assets.
Also inspect the commandlet log for shader errors: instance shader compilation
can finish asynchronously after the Python script returns.
"""
from pathlib import Path

import unreal


def validate():
    root = Path(unreal.Paths.project_plugins_dir()) / 'Effekseer/Content/Materials'
    files = sorted(root.rglob('*.uasset'))
    if not files:
        raise RuntimeError('No Effekseer material assets found at {}'.format(root))

    materials = 0
    instances = 0
    failures = []
    for file in files:
        path = '/Effekseer/Materials/' + file.relative_to(root).with_suffix('').as_posix()
        asset = unreal.load_asset(path)
        if asset is None:
            failures.append(path + ': could not load asset')
        elif isinstance(asset, unreal.Material):
            unreal.MaterialEditingLibrary.recompile_material(asset)
            stats = unreal.MaterialEditingLibrary.get_statistics(asset)
            # A failed material translation has no valid sampler count.
            if stats.num_samplers < 0:
                failures.append(path + ': material translation failed')
            materials += 1
            unreal.log('EFFEKSEER_MATERIAL {} {}'.format(path, stats))
        elif isinstance(asset, unreal.MaterialInstanceConstant):
            unreal.MaterialEditingLibrary.update_material_instance(asset)
            instances += 1
            unreal.log('EFFEKSEER_MATERIAL_INSTANCE {}'.format(path))

    if failures:
        raise RuntimeError('\n'.join(failures))
    unreal.log('EFFEKSEER_MATERIAL_VALIDATION_COMPLETE: {} materials, {} instances'.format(
        materials, instances))


validate()
