# Current Limitations

This page collects the limitations that still apply to DasForUE2, grouped by area. It is not a list of everything unsupported in Unreal: anything not documented on a feature page should be treated as unavailable. The [type support](TypeSupport.md) page has the detailed type matrix.

## Production, platform and security

- **Shipping builds** are not supported; the plugin is developed and checked with Win64 Development (Editor and packaged game).
- **Full AOT** of the game path is not available. Scripts run in the daScript interpreter; UE externs are deliberately excluded from AOT so their guards cannot be bypassed.
- **Networking** is not supported: no replication, no RPC (net functions are never exported or authorable), no network roles for spawned actors, no async-physics network rewind/resimulation.
- **Clean-machine deployment** has not been established; builds assume the documented toolchain (source-built UE 5.8.3, VS 2022/MSVC 14.44, Windows SDK 10.0.22621). Debug CRT is not supported, and only AVX2 hosts have been used.
- The runtime module cannot be unloaded dynamically.
- **No sandbox.** Script code is trusted. Construction guards, execution-role guards, computed-default restrictions and the async-physics VM limits (64 KiB stack/globals, 1 MiB heaps) protect engine invariants; they do not make untrusted daScript, third-party modules, macros or native extensions safe. An infinite loop in a physics callback can still stall the solver.
- All script execution is on the game thread, except the dedicated async-physics VM, which runs on the physics pre-simulation step with no access to ordinary UE externs.

## Reload, hot reload and migrations

- Logic reload happens on the next call after a source change; there is no background file watcher. Editor import/reimport (`Das.DSL.Apply`) is a manual command without toolbar UI or a standard asset-import factory.
- A failed candidate keeps the last working revision. Rollback after an unexpected Blueprint compiler error is limited.
- Script state is not transferred when a class is reinstanced: a new context is created. Arbitrary VM globals (including math, string and name globals) are not migrated on a valid reload. Handles must be re-acquired; timers and pending latent/async requests are cancelled by an accepted reload and are not restored.
- There is **no universal migration** of types, objects or assets. Only the policies described in [Migrations](../Guide/Workflow/Migrations.md) exist; other declaration changes are refused before any mutation. Explicitly refused today:
  - changing the native parent of an existing Blueprint, or the parent of an authored hierarchy;
  - replacing an inherited component node with a subclass (an Unreal engine constraint);
  - structural or default changes to classes that contain events/delegate fields or direct containers, and to component declarations on already imported or composed actors;
  - structural updates of an existing function-library asset (create a new asset);
  - changes to an existing interface contract or membership;
  - container definition and graph repair migrations; broad dependent Blueprint/map migration.
- Migrations are not recorded as Unreal editor transactions (no undo).
- Generated bindings and the native registry belong to one engine/API version; regenerate after engine or source changes.

## DSL authoring

- Only the documented native parents can be authored (see [Classes](../Guide/DSL/Classes.md): Actor, ActorComponent, Pawn, Character, data assets, SaveGame and similar hosts); arbitrary native parents need a dedicated adapter. Native receiver types exposed by the bindings (PlayerController, CameraComponent …) are not authorable parents.
- No arbitrary C++ virtual overrides or custom constructors. Script-only helpers must be instance methods with value/void results; static helpers and reference returns are refused, and there is no cross-context virtual dispatch for script-only methods.
- Re-entering an active daScript VM is not supported (for example dispatching from an active VM into a daScript-owned actor through an interface, or a `UFunction` that calls back into the VM from a low-level call frame).
- No authored net/RPC methods or latent actions. Pure methods and static function libraries are supported.
- Parameter defaults: trailing literals only; out/ref, `WorldContext`, complex and computed defaults are rejected. Authored interface stubs have no defaults.
- Property defaults: persisted object references must be top-level assets or classes, not actors or subobjects; non-null authored-object reference defaults and authored object-reference keys are not supported; nonempty soft-reference members of authored structs are rejected.
- Delegates: no worker-thread delivery, no native non-`UFUNCTION` delegates, no script lambda bindings. A context holds at most 4096 live delegate values and 256 signature contracts.
- Component composition: at most 32 components per actor, direct authored ActorComponent types.
- Canonical math types (`UEVector` …) have no arithmetic operators; use helper functions or the [Math package](../Api/Math.md).
- Localization gathering from DAS sources is not automatic; keyed text needs matching localization resources.
- SaveGame and DataAsset classes cannot declare world lifecycle hooks, components or async physics hooks; SaveGame has no schema migration or save-format versioning.
- An editor code action that generates delegate handlers (VS Code) is not part of this repository.

## Types, containers and limits

- Not supported anywhere: nested containers, fixed C++ arrays, raw pointers, non-reflected C++ types.
- Hash keys and set elements: byte, int, string, FName, supported enums and object keys only. No float, bool, FText, struct, math, class, soft or interface keys. Containers in authored `ue_struct`s are narrower (int, string, byte keys).
- Class references cannot be stored in authored containers, authored interfaces, delegates, events or timers.
- Containers of weak object references are rejected.
- Opaque engine values (`UEHitResult`, `UETraceHit`, `UEConstraintHandle`, `UEPhysicsBodyToken`, Geometry, input events, `EventReply`) cannot be used as `ue_property` fields, authored `ue_function` signatures or JSON defaults, and are not carried across hot reload.
- Event subscriptions accept by-value and const-ref parameters only; mutable native ref/out subscriptions are not supported. Queued callbacks and timers take 0–16 by-value arguments without ref/out or defaults.
- Methods and delegate frames: at most 16 parameters.
- Bounded resources per context: 4096 struct/hit-result snapshots, 4096 text snapshots, 4096 enum tokens, 4096 aggregate container elements and 16 MiB per snapshot value, 256 simultaneous low-level call frames, 65,536 elements per call-frame container, 1024 object-hash carrier combinations, 65,536 object identities. Defaults share depth 16, 8192 nodes and 1 MiB input JSON.
- Native enums: unique values 0..254 only; wider, signed or aliased enum layouts are skipped. Enum bitmask combinations are not synthesized.
- Snapshot GC needs `options gc` inside long-running calls; frames it cannot attribute (AOT/JIT, foreign frames) are never collected.

## Native API

- Non-reflected C++ is reachable only through hand-written [native binds](../Guide/Native/NativeBinds.md) (`DAS_NATIVE_BINDS`): there is no C++ header parser, no access to private properties, no virtual overrides, no vtable patching.
- C++ calls scripts through [`TDasFunction`](../Guide/Native/NativeBinds.md#calling-scripts-from-c-tdasfunction) with an owner object only (its session runs the call), on the game thread, without output parameters; types are scalars, strings, names, texts, vectors, rotators, transforms and objects.
- Latent, CustomThunk, RPC, delegate-signature, internal, protected and deprecated functions are excluded unless a dedicated adapter exists. Async-proxy factories (such as `CreateMoveToProxyObject`) are replaced by managed adapters.
- API packages: all arguments are explicit (no C++ defaults); soft object/class parameters have no generic wrapper ABI; maps/sets of structs or objects are not supported; object views, `UEName` and enums are not unified between packages, so qualify names when importing several packages. `ue_api_pawn_ai` and `ue_api_animation_audio_vfx` are not installed by default. See [API packages](../Api/README.md).
- A returned reference type does not expose all members of its class; only classes discovered from `[ue_native_object]` declarations get full member access ([Native class bindings](NativeClassBindings.md)).
- Native structs: only those whose members are all supported; structs with native GC hooks, opaque state or recursive layouts are excluded. `PerPlatform*` values are not value structs. Nested writes into native math roots (`movement.Velocity.x=…`) are rejected; assign whole values.
- No input component, Enhanced Input component or local-player-subsystem reflected API; use the managed input modules. Widget lifecycle overrides (`Construct`/`Destruct`), focus-path/pointer notifications and animation-finished callbacks are not bridged.

## Gameplay services

- Spawning: no Instigator, deferred caller customization or network roles; spawning is forbidden in `bind_events`, `EndPlay`, templates and non-game worlds.
- Scene components created by script: only components created by the same owner can be destroyed; tracking is bounded to 1024 live entries; ownership transfer on reinstancing is not provided.
- Effects: a Niagara system that is not ready yet, an empty asset pair or a dedicated server makes `ue_effect_at`/`ue_effect_play` return `false`; delayed start is not implemented.
- Input: mapping contexts are not persisted to user settings; there is no automatic transfer across possession changes or world travel, and no general possession lifecycle hooks.
- Drag & drop: `DetectDragIfPressed` does not create the operation itself; `EventReply`, live navigation queries and PaintContext are valid only inside their synchronous callback.
- Async physics: one body per slot (slots 0..7) of the VM owner's own components; only Box, Sphere, Capsule and StaticMesh components with a single own `BodyInstance`. Skeletal/ragdoll, instanced static meshes, geometry collections, welded, static and kinematic bodies are rejected. No worker-thread VM. See [Async physics](../Guide/Gameplay/AsyncPhysics.md).
- Collision: the managed `ue_sphere_overlap` helper is channel-based and not deduplicated by actor (use the package's `*OverlapActors`/`*OverlapComponents` for object-type queries); constraint handles do not support skeletal/PhysicsAsset accessors; complex and welded bodies are outside the collision package.
- AI: BehaviorTree/EQS have only the generated calls (`RunBehaviorTree`, `UseBlackboard`), no managed API. Typed perception projection covers `OnTargetPerceptionUpdated`.
- Asset preflight is not a cook or packaged-game certificate (`packaged_verified` is always false) and repairs nothing automatically.

See also: [Type support](TypeSupport.md) · [API packages](../Api/README.md) · [Native class bindings](NativeClassBindings.md) · [Migrations](../Guide/Workflow/Migrations.md) · [Apply and hot reload](../Guide/Workflow/ApplyAndHotReload.md)
