# Animation / Audio / Niagara Package (ue_api_animation_audio_vfx)

`ue_api_animation_audio_vfx` wraps the reflected API of animation instances and montages, skeletal meshes, audio components and Niagara. It is generated from `Bindings/Packages/AnimationAudioVFX.profile.json` (UE modules Engine, AnimGraphRuntime, AudioMixer, Niagara). The package is not installed by default: publish it first.

```powershell
.\Scripts\GenerateApiPackage.ps1 -Profile .\Bindings\Packages\AnimationAudioVFX.profile.json -SkipBuild -Publish
```

```das
options gen2
require ue_api_animation_audio_vfx

[export]
def main(input:int):int {
    let angle=ue_anim_math_CalculateDirection(ue_vector(0.0lf,100.0lf,0.0lf),ue_rotator(0.0lf,0.0lf,0.0lf))
    return int(angle)          // 90: moving right relative to forward
}
```

## Contents

| Prefix | Class or library |
| --- | --- |
| `ue_anim_math_` | `KismetAnimationLibrary` |
| `ue_audio_mixer_` | `AudioMixerBlueprintLibrary` |
| `ue_vfx_` | `NiagaraFunctionLibrary` |
| `ue_vfx_array_` | `NiagaraDataInterfaceArrayFunctionLibrary` |
| `ue_anim_instance_` | `AnimInstance` |
| `ue_anim_montage_` | `AnimMontage` |
| `ue_anim_sequence_` | `AnimSequence` |
| `ue_skeletal_mesh_` | `SkeletalMeshComponent` |
| `ue_skinned_mesh_` | `SkinnedMeshComponent` |
| `ue_audio_component_` | `AudioComponent` |
| `ue_sound_` | `SoundBase` (no callable functions in this engine version) |
| `ue_vfx_component_` | `NiagaraComponent` |
| `ue_vfx_parameters_` | `NiagaraParameterCollectionInstance` |

About 510 wrappers are generated. Each `include: ["*"]` selects the class's own reflected API; inherited copies are not repeated. Returned assets (animation sequences, Niagara systems, sounds) remain typed references even when their class has no callable functions of its own. Vector/Rotator/Transform use canonical math; all arguments are explicit. Some wrappers keep construction guards.

## Managed playback and callbacks

Generated wrappers are plain calls. Montage playback with notify/completion callbacks (`ue_montage_play`) and world-space sound/Niagara effects with a finish handler (`ue_effect_at`, `ue_effect_play`, `ue_sound_play`, `ue_niagara_spawn`), together with their lifetime handling, are provided by the managed `ue_animation` and `ue_effects` modules; see [Animation and effects](../Guide/Gameplay/AnimationAndEffects.md). Generating the reflected API adds no lifecycle guarantee for other asynchronous operations.

## Not included

| Function | Reason |
| --- | --- |
| `KismetAnimationLibrary.K2_CalculateVelocityFromSockets` | Opaque/empty struct needs an adapter |
| `AudioMixerBlueprintLibrary.GetAvailableAudioOutputDevices`, `GetCurrentAudioOutputDeviceName`, `PrimeSoundForPlayback`, `SwapAudioOutputDevice` | Async device/prime callbacks need a managed contract |
| `AudioComponent.AddModulationRouting`, `GetModulators`, `RemoveModulationRouting`, `SetModulationRouting` | Sets of objects are not supported by the generic hash-container ABI |

Export also rejects internal/deprecated functions, soft or native-reference structs, some construction/constraint contracts and instance `WorldContext` functions. `AnimInstance` override events (Blueprint events) are not callable functions, so their absence does not mean `AnimInstance` is missing. Full list: `Bindings/Packages/ue_api_animation_audio_vfx.summary.json`.

Try it: `.\Scripts\TestAnimationAudioVFXPackage.ps1 -SkipBuild` generates the package, compiles it and runs the `CalculateDirection` probe.

See also: [API packages](README.md) · [Animation and effects](../Guide/Gameplay/AnimationAndEffects.md) · [Pawn / Character / AI / Navigation](PawnCharacterAINavigation.md)
