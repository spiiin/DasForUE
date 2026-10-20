# Typed input and collision names

## Enhanced Input

```das
ue_enhanced_bind(self.JumpAction, TriggerEvent.Started, self.OnJump)
ue_enhanced_unbind(self.JumpAction, TriggerEvent.Started)
```

`TriggerEvent` covers Triggered, Started, Ongoing, Canceled and Completed. A misspelled enum member is a compile error. Existing string overloads remain accepted; literal strings in user source produce `DAS_DSL_STRING_TRIGGER`. Dynamic string expressions are not statically linted. Generated binding modules are excluded from this warning.

## Collision profiles

```das
require ue_collision_profiles

let profile:CollisionProfile = CollisionProfile.OverlapOnlyPawn
self.Trigger.SetCollisionProfileName(ue_collision_profile_name(profile), true)
```

`CollisionProfile` is generated from the current project's loaded `UCollisionProfile` configuration when `ue_foundation` initializes. It includes project-specific names as well as the engine presets; it is not a hard-coded list of built-ins. Convert a value with `ue_collision_profile_name` for native self APIs expecting `UENameValue`. Generated API packages use their own `UEName(value:string)` wrapper; construct it from `ue_name_text` as shown below. The result uses the existing context-owned FName transport.

This is a daScript configuration enum, not a reflected UEnum or a UPROPERTY storage type. Store a `UENameValue` in reflected data. An int or string cannot accidentally substitute for a typed profile; the internal conversion also checks that the ID belongs to the generated catalogue and the profile still exists in the current configuration. The binding remains guarded to the normal Unreal execution role.

IDs are stable hashes of case-normalized UTF-8 profile names, independent of enumeration order. Hash collisions are rejected. Ordinary identifier names remain readable (BlockAll, NoCollision, OverlapOnlyPawn, etc.). Punctuation/leading digits/daScript keywords are normalized and receive a hash suffix. Distinct profile names that normalize to one label get distinct labels. The diagnostic `DasCollisionProfileConstants()` exposes the generated symbol-to-FName map for tooling.

The enum catalogue is a snapshot for the loaded foundation module. After editing the project's collision-profile configuration, restart the editor/game process to rebuild its symbols. Removing a profile is checked again during conversion; stale values fail instead of resolving to a different profile. Use FName for persisted data and an explicit conversion at API boundaries.

The generated API package already accepts the converted value:

```das
require ue_api_collision
let body = ue_ref(type<C_PrimitiveComponent_5efdd477>, component.handle)
ue_body_SetCollisionProfileName(body,
    UEName(value=ue_name_text(ue_collision_profile_name(CollisionProfile.NoCollision))), true)
```

Require the collision API package for that generated wrapper. See the [input guide](../Gameplay/Input.md) and `dsl_playable_pickup.das` for the ordinary class-method form.
