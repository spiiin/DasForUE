# Animation and Effects

Two managed modules cover the most common presentation tasks: `ue_animation` plays Anim Montages and delivers typed montage/notify events to a script method, and `ue_effects` plays sounds and Niagara systems at a world location with an optional completion callback. Both clean up after themselves on Destroy, EndPlay and hot reload.

For the full generated Animation/Audio/Niagara function surface (for example `CalculateDirection`), see [Animation, Audio and Niagara API](../../Api/AnimationAudioNiagara.md).

## Montages

Example: `Plugins/DasForUE/Content/Das/Scripts/dsl_montage_actor.das`.

```das
options gen2
require ue_animation

[ue_class(Blueprintable)]
class MontageActor : Actor {
    settings=ue_properties(EditAnywhere,BlueprintReadWrite) { var Montage:AnimMontageRef }
    @ue_component
    @RootComponent
    Mesh:SkeletalMeshComponentRef

    [ue_function(BlueprintCallable)]
    def Play():float { return ue_montage_play("action",self.Mesh,self.Montage,"OnAnimation") }

    [ue_function(BlueprintCallable)]
    def OnAnimation(event:MontageEvent):void {
        if(event.Kind==MontageEventKind.NotifyBegin) {
            // event.NotifyName is the FName of the Montage Notify / Notify Window.
        }
        if(event.Kind==MontageEventKind.Completed) { /* finished normally */ }
        if(event.Kind==MontageEventKind.Interrupted) { /* interrupted */ }
    }
}
```

Assign a SkeletalMesh and a suitable AnimInstance/AnimBlueprint to the mesh. For visible animation the AnimGraph needs a Slot matching the Montage. The example shows the binding logic; no ready-made animated character is shipped.

### API

| Function | Behavior |
| --- | --- |
| `ue_montage_play(name, mesh, montage, handler, rate=1.0f, position=0.0f, section=default<UENameValue>):float` | Returns the length reported by Unreal, or `0` if the engine refused to start. |
| `ue_montage_stop(name, blend_out=0.1f):bool` | Stops this subscription's instance; `Interrupted` is still delivered. `false` if no active instance. |
| `ue_montage_cancel(name):bool` | Revokes the subscription and stops its instance without any callbacks; already-queued events are dropped. |
| `ue_montage_playing(name):bool` | Is the instance active? During blend-out it may already return `false` before the terminal event arrives. |

Before playing, the plugin checks references, the game world, skeleton compatibility, a positive finite rate, position, section and the handler contract. `SoftAnimMontage` works with the regular [async asset loading](../Data/AssetLoading.md).

The handler must be `[ue_function] def Handler(event:MontageEvent):void` in the DSL class. Pass the method as `self.Handler` (or its name as a string literal); the macro maps it to the exported entry point and the signature is checked at authoring time. A handler with the wrong parameter type (see the negative example `montage_bad_callback.das`, which takes `int`) is rejected with a montage-callback mismatch diagnostic before the Blueprint is created.

### MontageEvent

| `Kind` | When it arrives |
| --- | --- |
| `Completed` | Finished without interruption. |
| `Interrupted` | An interrupted playback has fully ended. |
| `BlendOut` | A normal, uninterrupted blend-out started. |
| `NotifyBegin` | A Montage Notify, or the start of a Montage Notify Window. |
| `NotifyEnd` | The end of a Montage Notify Window. |

`NotifyName` is set for notify events and is `NAME_None` otherwise. The API uses the native `OnPlayMontageNotifyBegin/End`; arbitrary AnimNotify classes and AnimBlueprint `AnimNotify_*` functions are not connected automatically.

### Lifetime and replacement

- Reusing a name replaces the previous subscription in this context. Instances are told apart by native instance ID even when the same asset plays.
- Completion/interruption releases the subscription before the handler runs, so the handler can safely start the next montage with the same name. `dsl_montage_actor.das` uses `RepeatOnComplete` to replay once from `Completed`.
- Events are delivered on the game thread after the current animation pass. Native delegates hold no pointers to the VM or daScript functions.
- Destroy/EndPlay, cancel and a successful hot reload revoke the ID, unbind the adapter's delegates and stop only its own instances. A rejected reload keeps the working VM, playback and handlers. A Montage is not restarted automatically after a successful reload.
- If the end/blend delegates of a new instance are already bound by other code, the adapter refuses to overwrite them. Cleanup removes only delegates the adapter still owns.
- Unreal's usual group rules apply: `Montage_Play` stops montages of the **same** group, not all groups. The single root-motion montage rule is the engine's.

Try it: `.\Scripts\TestMontage.ps1 -SkipBuild` creates the example Blueprint and a map in a fresh folder.

## Sounds and Niagara

`ue_effects` plays a sound (`SoundBaseRef`), a Niagara system (`NiagaraSystemRef`) or both together at a world position. Typed soft references `SoftSound` and `SoftNiagaraSystem` are also available.

```das
require ue_effects

// Inside a ue_class method; PickupSound/PickupVFX are properties.
if(ue_effect_at("pickup",self.PickupSound,self.PickupVFX,
        self.Trigger.K2_GetComponentLocation(),"OnPickupEffectFinished",0.35f,1.4f,0.4f)) {
    self.EffectsStarted+=1
}

[ue_function(BlueprintCallable)]
def OnPickupEffectFinished():void {
    self.EffectsFinished+=1
}
```

| Function | Notes |
| --- | --- |
| `ue_effect_play(name, sound, system, transform:UETransform, handler="", volume=1.0f, pitch=1.0f):bool` | Full transform. |
| `ue_effect_at(name, sound, system, location:UEVector, handler="", volume=1.0f, pitch=1.0f, scale=1.0f):bool` | Location plus uniform scale. |
| `ue_sound_play(name, sound, location, handler="", volume=1.0f, pitch=1.0f):bool` | Sound only. |
| `ue_niagara_spawn(name, system, location, handler=""):bool` | Niagara only. |
| `ue_effect_active(name):bool`, `ue_effect_stop(name):bool`, `ue_effect_stop_all():int` | Only this script instance's effects; `stop_all` returns the number stopped. |

Semantics:

- Up to 64 active effect names per script instance. Starting the same name again replaces the previous effect.
- Components are created as transient instance components of the owning Actor and are kept alive against GC. When both parts finish, the components are removed and then the optional handler (a `ue_function` with no parameters and no result) is called once. No handler still means automatic cleanup. An explicit stop does not call the completion handler.
- A looping Niagara system must be stopped explicitly or with its owner; the callback waits for the whole pair.
- Destroy/EndPlay and a successful hot reload stop effects, remove the adapter's delegates and revoke any queued callback before the VM is replaced. A rejected reload keeps the running version and subscriptions. Other components of the owner are untouched.
- `false` is returned (and a previous effect with that name is kept) for an empty sound/system pair, on a dedicated server, or when the NiagaraSystem is not ready yet. Deferred start until the system is ready is not implemented.
- Wrong types, an invalid callback and non-numeric values are rejected. `volume` must be in `0..16`, `pitch` in `(0, 4]`.

Examples: `dsl_playable_pickup.das` plays a sound and burst on pickup; `dsl_playable_character.das` plays a hurt effect from `OnTakeAnyDamage` for positive damage. `PickupSound`/`PickupVFX` and `HurtSound`/`HurtVFX` are editable in the Blueprint. `.\Scripts\CreatePlayableCharacter.ps1 -SkipBuild` exports the demo with its own `NS_Pickup`/`NS_Hurt` systems.

## Limitations

- Effects spawn at a world location only: socket attachment, fades, attenuation overrides and Niagara user parameters are not exposed by `ue_effects`.
- Montage support does not include visible-pose/root-motion guarantees or networked playback.

See also: [Asset loading](../Data/AssetLoading.md), [Gameplay services](GameplayServices.md), [Callback arguments](CallbackArguments.md), [Soft references](../DSL/SoftReferences.md)
