# Changelog

All notable changes to DasForUE2, newest first. The format loosely follows
[Keep a Changelog](https://keepachangelog.com/). Short commit hashes are given
where a change can be attributed to a single commit.

Target environment for every entry: Unreal Engine 5.8.3 (source build),
daScript 0.6.4 (pinned), Win64 Development. Start at the
[documentation index](Docs/index.md).

## 2026-10-21

### Added

- Saving a `.das` file reimports the loaded Blueprints bound to it with the safe automatic policy; body-only
  edits stay live through hot reload, also during PIE. Actors with components accept declaration
  changes as long as their components stay the same. Default-value changes apply during PIE to the class and
  the editor world (running actors keep their values); other refusals during PIE are retried when PIE ends. Classes with soft reference fields (such as the playable pickup) accept declaration changes
  again; only direct container fields still block structural rollback. A save that adds members and changes
  defaults applies them in two steps, and scripts changed without a save are picked up when a map opens and
  before Play. `Das.DSL.AutoReimport 0` turns it off
  ([Apply and hot reload](Docs/Guide/Workflow/ApplyAndHotReload.md#automatic-reimport-on-save)).

- Reload matrix: `DasForUE.ReloadMatrix.Editor` and `.PIE` (suite `reload-matrix`) apply every field and
  method change to Actor, composed Actor, Pawn and Character classes and record the outcome;
  [Reload matrix](Docs/Reference/ReloadMatrix.md) is generated from the expected outcomes.

- Authored hierarchies: a child can switch to another parent with the same native root (reparented in place,
  rolled back on failure), saving a parent script reimports its children, and explicit field migrations
  accept classes with authored children ([Inheritance](Docs/Guide/DSL/Inheritance.md)).
- DataAsset, PrimaryDataAsset and SaveGame classes accept declaration changes: saved DataAsset instances are
  migrated with the class and marked dirty, removed fields are archived on the instance
  ([Data assets](Docs/Guide/Data/DataAssets.md#changing-the-class)).
- Struct field migration updates the rows of DataTables that use the struct in place (rename, add, drop, convert;
  rolled back on failure); a struct used only by tables no longer needs a DSL Actor root.
- GAS assets update in place: reimporting into the same folder (or saving the script, through auto-reimport)
  rewrites changed effects, abilities, calculations and cues and adds new attributes, keeping the classes that
  other assets reference ([Changing definitions](Docs/Guide/Gameplay/GASAuthoring.md#changing-definitions)).
- Auto-reimport asks before a field rename, removal or numeric retype that also modifies saved maps or other
  Blueprints: a notification lists the changes and the affected assets (Apply / Details / Cancel) instead of the
  "requires ... with dependencies" refusal. New default values saved together with such a change are applied
  first ([Apply and hot reload](Docs/Guide/Workflow/ApplyAndHotReload.md#automatic-reimport-on-save)).
- Authored ActorComponent classes accept declaration changes: their templates in Actor Blueprints, placed and
  running components migrate like Actors ([Components](Docs/Guide/DSL/Components.md#changing-an-authored-component-class)).

### Changed

- A script that cannot run on its actor (for example a Blueprint made from an older version of the script,
  or a failed hot reload) is reported on screen and in the PIE Message Log, not only in the log
  ([Examples](Docs/Guide/GettingStarted/Examples.md)).
- The playable example map uses a movable sun with a sky atmosphere: no "Lighting needs to be rebuilt"
  message and no black sky.
- Generated example folders (`DasExamples/Playable`, `DasExamples/AI`) are no longer kept in git.

### Fixed

- A Blueprint that the editor marked as needing a compile without a real change (after saving its package,
  or after a template default update) no longer blocks reimport: default updates accept it, structural
  migrations compile it first. A freshly created DSL Blueprint is up to date right away.
- Changing the default of a byte or text field is merged like other defaults instead of being refused.
- Pawn and Character declarations accept added fields and methods like other actors, also during PIE; a
  possessed Pawn stays possessed after the reinstance. Structural rollback snapshots every instance kind.
- Saving a script that renames fields (same `ue_id`), removes fields or changes a numeric field type applies
  the matching migration automatically (rename, archive, exact conversion); read-only/editable flags and
  method removals, renames and signature changes are applied as well. Archive works on Pawn and Character.
- Classes with delegate or event fields accept declaration changes; delegate and event fields can be added
  and removed, and an `ue_implements` interface can be added on save. Changing a delegate/event signature or
  removing an interface is still refused.
- Components of an imported Actor can be added, removed, renamed (same `@ue_id`), re-attached and get new
  template defaults on save; default changes also reach running PIE actors whose value the game has not
  changed.
- Classes with array, map or set fields accept declaration changes (new fields and methods, defaults) instead
  of body-only reimport; container contents of instances are kept.
- `Tests/suites.json` gains `defaults-merge` and `actor-component`; the defaults merge probe was not part of
  any suite and had been failing since before this change.

## 2026-10-20

### Added

- Callback arguments accept method references: `ue_timer_start("score",1.0,self.Pulse,true)`,
  `ue_enhanced_bind(action,TriggerEvent.Started,self.OnJump)`, `ue_widget_bind_click(hud,"Damage",self.OnDamage)`
  and the same for input, latent delays, effects, montages, AI MoveTo and asset loads. Handlers are
  checked at compile time, including button handlers ([Functions](Docs/Guide/DSL/Functions.md#methods-as-callbacks)).
- `TriggerEvent` enum for Enhanced Input bindings; trigger strings are still accepted
  ([Input](Docs/Guide/Gameplay/Input.md)).
- `ue_event` handlers on native sources (self of a class with a native parent, native components)
  are checked at compile time: an unknown delegate name or a wrong number of parameters is a
  compile error ([Delegates and events](Docs/Guide/DSL/DelegatesAndEvents.md)).

### Changed

- Self, authored and native property access is much faster: registry lookups are indexed, property
  resolution and access contracts are cached (safe across recompilation and reinstancing) and plain
  scalars are accessed in place. A self int read plus write went from about 15 µs to about 0.5 µs.
- Self scalar property reads and writes are about three times as fast again (a per-name site cache on
  top of the cached resolution), and every call into Unreal checks the frozen-physics state once per
  script entry instead of on each call.
- Generated native reference fields (`movement.MaxWalkSpeed`) are about 17x faster: the receiver class
  and the validated field are cached per access site. Authored references cache their identity check.
- Object and component field reads (`character.CharacterMovement`, `self.Camera`) are about 9x faster.
- Property values read or written through accessors, call frames and snapshots no longer rebuild the
  property's type descriptor on every call.
- Operations on self containers with scalar elements (`push`, `pop`, `erase`, `insert`, `clear`,
  `self.Items[i] = v`, `self.Scores[k] += v`) change the property in place instead of copying the whole
  container: push+pop on 1024 ints went from about 200 us to about 0.25 us. Nested container paths and
  struct, enum, object and math elements are unchanged.
- Reading and writing scalar members of authored self structs (`self.Stats.Count += 1`) no longer
  copies the whole struct.
- Class values read or written through generated native references are validated like self and
  authored ones (base class, deprecated classes).

### Fixed

- Authored struct fields of type `UEVector2D` keep their DSL default; the editor formatted it as a
  Transform and failed its type check.
- Authored structs accept GameplayTag map values (`table<string;GameplayTag>`); they were refused
  by `ue_struct` and the authored type validation ([GameplayTags](Docs/Guide/Data/GameplayTags.md)).

### Internal

- Math structs (Vector, Vector2D, Rotator, Transform, InputActionValue) are ported to type adapters:
  identity, validation, defaults, members, packing and call transports
  ([Type adapters](Docs/Dev/Design/TypeAdapters.md)).
- Type adapters (`DasTypeAdapters.h`): one object per value type owns its identity and decisions;
  scalar kinds and GameplayTag are ported, subsystems ask the registry
  ([Type adapters](Docs/Dev/Design/TypeAdapters.md)).
- One value kind table (`DasValueKinds.h`) maps kind names, actor field kinds and method signatures;
  DSL lowering compares `EDasTypeKind` values instead of kind strings
  ([Improvement plan](Docs/Dev/Plans/ImprovementPlan.md), phase 1).
- C++ sources are clang-formatted; `DasRuntime.cpp` and `DasDSL.inl` are split into subsystem parts;
  scalar, enum and class accessor families are generated from one kind table; foundation tests guard the script-visible
  extern surface with `Bindings/FoundationExterns.txt`; test scripts share `Scripts/DasTest.psm1`
  ([Improvement plan](Docs/Dev/Plans/ImprovementPlan.md)).
- `Bindings/Kinds.json` exports the value-kind table for the Python generators (`Scripts/DasKinds.py`);
  foundation tests keep it in sync with the C++ table.
- Regression suites are described in `Tests/suites.json` and run by `Scripts/RunDasTests.ps1`
  (`-Preset quick` skips MethodValues and packaging); `TestCombined.ps1` runs the `full` preset.

## 2026-10-19

### Added

- Create authored DataTables from JSON with `Das.DSL.Table.Create`: rows are
  imported and validated against an authored row struct before any package is
  created, and existing packages are never overwritten
  ([DataTables and curves](Docs/Guide/Data/DataTablesAndCurves.md)) (571bc48).
- Generated UMG `EventReply` wrappers that work inside synchronous widget
  handlers, plus live input navigation queries
  (`Key_GetNavigationActionFromKey`, `Key_GetNavigationDirectionFromKey/FromAnalog`)
  ([Widget events](Docs/Guide/UMG/WidgetEvents.md)) (d2a93bc).
- `OnFocusReceived` / `OnFocusLost` handlers with owned `UEFocusEvent` snapshots
  ([Widget events](Docs/Guide/UMG/WidgetEvents.md)) (d2a93bc).
- Widget animations via a weak `UEWidgetAnimationHandle`
  (`PlayAnimation`, `Forward`, `Reverse`, `TimeRange`), and scripted `OnPaint`
  through a scoped `UEPaintContext` (`DrawBox`, `DrawLine`, `DrawLines`,
  `DrawSpline`, `DrawTextFormatted`)
  ([Paint and animation](Docs/Guide/UMG/PaintAndAnimation.md)) (d2a93bc).
- Managed drag and drop (`ue_drag_drop`): one operation per `OnDragDetected`,
  payload/visual/tag/pivot/offset and synchronous enter/over/leave/drop/cancel
  handlers on `DasScriptUserWidget` ([Drag and drop](Docs/Guide/UMG/DragDrop.md)) (d2a93bc).
- `ue_gameplay_tags`: `GameplayTag` and `GameplayTagContainer` values, hierarchy
  matching, container operations, real tag properties in authored classes and
  structs, and tags in method parameters/results/ref
  ([GameplayTags](Docs/Guide/Data/GameplayTags.md)) (6038cec).
- Image soft brushes: `SetBrushFromSoftTexture` / `SetBrushFromSoftMaterial`
  with managed async loading, cancellation and cleanup
  ([Paint and animation](Docs/Guide/UMG/PaintAndAnimation.md)) (6038cec, 7a6d457).

### Changed

- The UMG / Input package is published with 409 generated wrappers; every remaining
  export refusal is classified by name (policy exclusion, existing adapter or
  technical blocker) ([UMG / Input API](Docs/Api/UMGInput.md)) (d2a93bc, 7a6d457).
- `ue_soft(type<T>, ...)` accepts a soft-type `typedef` declared in the same file
  ([Soft references](Docs/Guide/DSL/SoftReferences.md)) (7a6d457).
- The "self reference property type mismatch" error now names the property (109eae6).

### Fixed

- Integer `:=` assignment to a read-only authored property is rejected again (109eae6).
- `PerPlatformInt/Float/Bool/FrameRate` are rejected as value structs, so the
  generated native gameplay module compiles in packaged builds
  ([Native class bindings](Docs/Reference/NativeClassBindings.md)) (109eae6).
- Cooked Character fixtures keep their generated component Blueprints, so
  component-typed properties resolve in cooked games (109eae6).
- GameplayTags: struct tag-field validation, unparsable container Blueprint defaults
  and Blueprint method execution in commandlets (6038cec).

## 2026-10-18

### Added

- Gameplay services: `LineTrace` / `SphereTrace` with typed results and weak
  references, a Widget Blueprint HUD with `FText` and buttons, native
  `ApplyDamage` and an authored `HealthComponent` with a `Changed` event;
  `ue_event(source="Health", ...)` can subscribe to an owned component
  ([Gameplay services](Docs/Guide/Gameplay/GameplayServices.md)) (ce133dc).
- Playable pickup with a real overlap, score, respawn timer and an async soft
  mesh; one shared `Interactable` interface reused across exported examples
  ([Examples](Docs/Guide/GettingStarted/Examples.md)) (ce133dc).
- Native API discovery: receiver classes are discovered from the native
  declarations in your DAS modules and their imports, and their whole supported
  API is generated ([Native class bindings](Docs/Reference/NativeClassBindings.md)) (ce133dc).
- Managed Montage API (`ue_animation`) with typed Completed/Interrupted/BlendOut/
  NotifyBegin/NotifyEnd events, and managed sound/Niagara cues (`ue_effects`)
  ([Animation and effects](Docs/Guide/Gameplay/AnimationAndEffects.md)) (6aa6a3c).
- Managed AI: `AIControllerRef`, MoveTo actor/location with `MoveResult`, cancel
  by name; `Scripts/CreateAIEnemy.ps1` builds a pursuing-enemy demo
  ([AI and perception](Docs/Guide/Gameplay/AIAndPerception.md)) (6aa6a3c).
- Standalone projects: `Scripts/NewDasProject.ps1`, a project template and
  project-local native generation; scripts live in the project's
  `Content/Das/Scripts` with plugin fallback
  ([Standalone project](Docs/Guide/GettingStarted/StandaloneProject.md)) (5289b40).
- Declarative API packages generated from UE reflection: Math (`ue_api_math`),
  World / Actor / Components, Collision / Physics (`ue_api_collision`, 234
  wrappers), Pawn / Character / AI / Navigation, Animation / Audio / Niagara and
  UMG / Input ([API packages](Docs/Api/README.md),
  [World](Docs/Api/WorldActorComponents.md),
  [Collision](Docs/Api/CollisionPhysics.md),
  [Pawn/AI](Docs/Api/PawnCharacterAINavigation.md),
  [Animation](Docs/Api/AnimationAudioNiagara.md),
  [UMG/Input](Docs/Api/UMGInput.md)) (2a175b6, 45cd1a1, b3651b3, 50aacaa, 6ca78be).
- Native property getters/setters (including `BlueprintGetter/Setter`) are
  honoured, and `UnsafeDuringActorConstruction` methods are guarded at runtime
  instead of being excluded ([Native class bindings](Docs/Reference/NativeClassBindings.md)) (6ca78be).
- Weak native references and checked `UETraceHit` / `UEOverlapResult` views;
  generated trace/sweep functions with a native `HitResult` transport
  ([Collision / Physics](Docs/Api/CollisionPhysics.md)) (4c5c8dd, 6ca78be).
- Object and class reference arrays inside generated structs, enabling
  `PredictProjectilePath_Advanced` ([Collision / Physics](Docs/Api/CollisionPhysics.md)) (f0ae73b).
- Checked `UEConstraintHandle` for `GetConstraint` and the
  ConstraintInstance library; `SetCollisionProfileNameProperty` adapter
  ([Collision / Physics](Docs/Api/CollisionPhysics.md)) (6ca78be).
- Body simulation and PhysicsHandle, and two-body PhysicsConstraint control,
  verified against real Chaos steps ([Collision / Physics](Docs/Api/CollisionPhysics.md)) (d199e74, e61715e).
- Native static `BlueprintCallable` functions outside Function Libraries
  (all `NavigationSystemV1` queries) and navigation queries on a real Recast
  NavMesh ([Pawn / AI / Navigation](Docs/Api/PawnCharacterAINavigation.md)) (c67686b).
- Sight and hearing perception callbacks, and a typed `AIStimulus`
  (`ue_perception`) delivered through
  `ue_event(source="Perception", name="OnTargetPerceptionUpdated")`
  ([AI and perception](Docs/Guide/Gameplay/AIAndPerception.md)) (8ecfa2b, 8a6d826).
- Async physics: `ue_async_physics(script, body_0..body_7)` registers a
  dedicated precompiled VM with a value mailbox and scoped body/`AddForce`;
  every UE extern carries an execution-role guard
  ([Async physics](Docs/Guide/Gameplay/AsyncPhysics.md)) (6ca78be).
- Typed `InputActionValue` (`ue_input_value`) in Enhanced Input handlers and
  authored methods; UMG single-cast delegate parameters as `UEFunctionDelegate`
  ([Input](Docs/Guide/Gameplay/Input.md)) (6ca78be).
- Owned Slate input-event snapshots (`UEInputEvent`, `UEKeyEvent`,
  `UEAnalogInputEvent`, `UECharacterEvent`, `UEPointerEvent`,
  `UENavigationEvent`) and the functions that take them (482d045).
- `UDasScriptUserWidget`: 17 synchronous `NativeOn*` handlers receive an owned
  `UEGeometry` and a typed event and return `UEEventReply`
  ([Widget events](Docs/Guide/UMG/WidgetEvents.md)) (7a681c6, 8eba3a3).
- Authored `SaveGame` classes and managed async slot operations
  ([SaveGame](Docs/Guide/Data/SaveGame.md)) (830f73c).
- `ue_primary_assets`: Primary Asset discovery without loading, async load with
  bundles, cancel and last-owner unload ([Asset loading](Docs/Guide/Data/AssetLoading.md)) (4a5197e).
- Authored `DataAsset` / `PrimaryDataAsset` classes, `Das.DSL.Asset.Create`, and
  cook/staging diagnostics with `Das.DSL.Asset.Check` and the `DasAssetCheck`
  commandlet ([Data assets](Docs/Guide/Data/DataAssets.md)) (cbb00b0).
- `ue_data_tables` typed row reads and `ue_curve_tables` curve evaluation
  ([DataTables and curves](Docs/Guide/Data/DataTablesAndCurves.md)) (2b569b5, 72d0b00).

### Changed

- The playable Character uses `InputActionValue` for move/look and the HUD's
  managed button with Tab switching between game and UI input
  ([Examples](Docs/Guide/GettingStarted/Examples.md)) (6ca78be).
- Lighting in exported demo maps is Movable, so no light baking is needed (5289b40).

### Fixed

- FFI registration order: `UCallHandle` is registered before transport externs (6ca78be).
- Empty C++ registry arrays in native generation; unchanged outputs are no longer rewritten (5289b40).

## 2026-10-17

### Added

- `class Player : Pawn` / `: Character` roots, possession, Character
  move/look/jump and a playable first-person Character example
  ([Classes](Docs/Guide/DSL/Classes.md), [Examples](Docs/Guide/GettingStarted/Examples.md)) (9da97dc).
- Static Function Libraries with `ue_library` and `def static`, including
  WorldContext and parameter defaults ([Function libraries](Docs/Guide/DSL/FunctionLibraries.md)) (33bdd2a).
- Trailing literal parameter defaults on ordinary methods
  ([Defaults](Docs/Guide/DSL/Defaults.md)) (33bdd2a).
- Native fields through `self`: scalars, names, objects/classes, enums, structs,
  arrays, maps and sets ([Self access](Docs/Guide/DSL/SelfAccess.md)) (33bdd2a, 9da97dc, 042a493).
- Reflection-driven generation of native fields and methods for gameplay
  classes, including native enums, `USTRUCT`, `TArray`, `TMap` and `TSet`
  ([Native class bindings](Docs/Reference/NativeClassBindings.md)) (042a493, fe7b330, 73e7d4c).
- Checked object-reference upcasts ([Object and class references](Docs/Guide/DSL/ObjectAndClassReferences.md)) (fe7b330).
- Container shapes: `FName`-keyed maps/sets, enum sets and enum map keys,
  `UObject` sets and object-keyed maps (`ActorKey`, `ObjectKey`)
  ([Containers](Docs/Guide/DSL/Containers.md)) (042a493, fe7b330, 73e7d4c).
- Container defaults with computed leaves, and asset array / `FName`-to-object
  map defaults via `ue_asset_default(path)` ([Defaults](Docs/Guide/DSL/Defaults.md)) (73e7d4c).
- Authored `FText` (`UETextValue`) and `uint8` in properties, struct fields,
  methods, containers, delegates, events, interfaces and timers
  ([Built-in types](Docs/Guide/DSL/BuiltinTypes.md)) (b82a254, b7da031).
- Typed callback arguments: enums, registered structs, objects/interfaces,
  containers and 2–16 arguments in events and managed timers
  ([Callback arguments](Docs/Guide/Gameplay/CallbackArguments.md)) (b7da031).
- Interface calls can dispatch into DAS implementations, including the same
  object and ActorComponents ([Interfaces](Docs/Guide/DSL/Interfaces.md)) (b7da031).
- Typed class references in interfaces, delegates, events and timers
  ([Object and class references](Docs/Guide/DSL/ObjectAndClassReferences.md)) (5dba9ee).
- Typed soft references `UESoftObject<T>` / `UESoftClass<T>`: fields, pins,
  method signatures, path defaults, sync/async load, soft arrays and map values,
  generated native soft fields and safe parent conversions with `ue_soft_upcast`
  ([Soft references](Docs/Guide/DSL/SoftReferences.md)) (ce133dc).
- `ue_struct_edit(snapshot)` for one-copy scoped snapshot edits
  ([Struct snapshots](Docs/Reference/StructSnapshots.md)) (ce133dc).

### Changed

- One shared type descriptor, `FProperty` factory, container registry,
  Blueprint pin adapter and recursive defaults backend replace per-type code
  paths; type support is listed in one matrix ([Type support](Docs/Reference/TypeSupport.md)) (b82a254, 468923d, b7da031).
- Snapshot reads borrow storage and owned temporaries move into the VM, reducing copies
  ([Struct snapshots](Docs/Reference/StructSnapshots.md)) (bb848df, 6f203f0).
- `Scripts/CreatePlayableCharacter.ps1` exports only the playable assets (ce133dc).

### Fixed

- Nullable VM object getters (faa2f92).

## 2026-10-16

### Added

- Typed class references `$UEClass<T>` (`ActorClass`, `ObjectClass`) in
  authored method signatures, with checked `ue_class_cast`
  ([Object and class references](Docs/Guide/DSL/ObjectAndClassReferences.md)) (5876b59).
- Once-per-instance `Initialize()` and repeatable `ConstructionScript()` for
  Actors and ActorComponents, with `Super` chaining and authored calls during
  construction ([Lifecycle](Docs/Guide/DSL/Lifecycle.md)) (5876b59, 84ff982).
- Inherited script-only helpers with explicit `Super` variants
  ([Inheritance](Docs/Guide/DSL/Inheritance.md)) (5876b59).
- `BlueprintPure` and `def const` methods, including pure interface contracts
  ([Functions](Docs/Guide/DSL/Functions.md)) (84ff982).
- `self.Property` assignments in `ue_defaults`, defaults on inherited properties,
  computed defaults from pure daScript expressions, and native Actor/component
  defaults without redeclaring fields ([Defaults](Docs/Guide/DSL/Defaults.md)) (84ff982, 33bdd2a).
- Safe defaults reimport (`Automatic` / `PreserveOverrides`) that keeps manual
  instance values ([Migrations](Docs/Guide/Workflow/Migrations.md)) (84ff982).
- Native base syntax `class Counter : Actor` / `: ActorComponent`
  ([Classes](Docs/Guide/DSL/Classes.md)) (84ff982).

## 2026-10-12

### Added

- Managed event subscriptions on inherited Actors and components
  ([Inheritance](Docs/Guide/DSL/Inheritance.md)) (270a70f).

## 2026-10-11

### Added

- Declaration groups: `properties = ue_properties(...) { ... }` and
  `components = ue_components(...)`; comments become ToolTips
  ([Classes](Docs/Guide/DSL/Classes.md)) (3488585).
- Native component fields, root/attachment declarations, component template
  defaults and typed native component create/get/destroy
  ([Components](Docs/Guide/DSL/Components.md)) (3488585).
- Explicit `BlueprintEvent` methods overridable in child Blueprints, with
  Call to Parent ([Functions](Docs/Guide/DSL/Functions.md)) (3488585).
- Authored delegates (`ue_delegate`, `ue_multicast`), Event Dispatcher fields,
  single-cast delegate fields, delegate-valued parameters/results and ref/out
  in signatures ([Delegates and events](Docs/Guide/DSL/DelegatesAndEvents.md)) (f838a0d, 1feb736).
- Runtime class handles, reflected class properties, `ue_actor_spawn` and
  typed Actor/component queries ([Spawn and queries](Docs/Guide/Gameplay/SpawnAndQueries.md)) (f838a0d).
- Blueprint Function Library wrappers with automatic WorldContext
  ([Function libraries](Docs/Guide/DSL/FunctionLibraries.md)) (f838a0d).
- Authored class inheritance (`class Child : Parent`) with `Super`, inherited
  components, parent lifecycle and inherited component defaults
  ([Inheritance](Docs/Guide/DSL/Inheritance.md)) (5fa213d).
- Managed gameplay APIs: named timers with captured arguments, next-tick timers,
  scalar/value/array event callbacks, legacy and Enhanced Input, mapping
  contexts, latent delay and async asset loading
  ([Timers](Docs/Guide/Gameplay/Timers.md), [Input](Docs/Guide/Gameplay/Input.md),
  [Asset loading](Docs/Guide/Data/AssetLoading.md)) (87f5fde).
- Maps with struct/math/enum/reference values and the authored source catalog
  ([Containers](Docs/Guide/DSL/Containers.md),
  [Modules and type identity](Docs/Guide/DSL/ModulesAndTypeIdentity.md)) (87f5fde).

### Fixed

- Class-reference Blueprint pins and cooked component roots (70125b2).

## 2026-10-10

### Added

- Authored ActorComponent classes, Actor component composition and lifecycle/event
  class methods ([Components](Docs/Guide/DSL/Components.md),
  [Lifecycle](Docs/Guide/DSL/Lifecycle.md)) (9f186c7).
- Typed references between authored objects: fields, method calls with ref/out,
  natural enum/struct/container values ([Authored references](Docs/Guide/DSL/AuthoredReferences.md)) (9f186c7).
- Direct `self` writes for nested structs, math values, references and containers,
  including multi-level indexing ([Self access](Docs/Guide/DSL/SelfAccess.md)) (9f186c7).
- Direct `ue_class` array/map/set fields, math/enum/reference arrays
  ([Containers](Docs/Guide/DSL/Containers.md)) (9f186c7).

## 2026-10-09

### Added

- Implement native interfaces (`implements=`), author Blueprint Interfaces with
  `ue_interface`, and call them through typed bindings such as `api.Query(5)`
  ([Interfaces](Docs/Guide/DSL/Interfaces.md)) (a05cbec, d585726).
- Parameter-local directions: `var x:int&` (inout) and `@ue_out`
  ([Functions](Docs/Guide/DSL/Functions.md)) (a05cbec).
- Authored enums and structs with stable IDs, type registries and cooked
  direct values ([Enums](Docs/Guide/DSL/Enums.md), [Structs](Docs/Guide/DSL/Structs.md),
  [Modules and type identity](Docs/Guide/DSL/ModulesAndTypeIdentity.md)) (a05cbec).
- Enum migrations (append, member rename, removal/value rules, Switch and node
  repair) and struct field add/rename ([Migrations](Docs/Guide/Workflow/Migrations.md)) (a05cbec).
- Authored arrays, maps and sets in structs and as method parameters
  ([Containers](Docs/Guide/DSL/Containers.md)) (a05cbec).
- `ue_object_types` (`ActorRef`, `ObjectRef`) and `ue_type(...)` source type
  references in method bodies ([Object and class references](Docs/Guide/DSL/ObjectAndClassReferences.md)) (a05cbec, d585726).

## 2026-10-07

### Added

- Reflected struct and math values in method signatures and properties
  ([Struct snapshots](Docs/Reference/StructSnapshots.md), [Structs](Docs/Guide/DSL/Structs.md)) (d070fb0).
- Explicit field migrations: `defaults=preserve_instances`, `fields=rename_by_id`,
  `fields=rename_with_dependencies`, `fields=widen_numeric_with_dependencies`,
  `fields=convert_numeric_with_dependencies` with Blueprint graph adapters,
  field archive/restore, and bounded structural rollback
  ([Migrations](Docs/Guide/Workflow/Migrations.md)) (d070fb0, 2d48950, 78a5ae2).
- Typed UObject DSL fields and reflected type declarations
  (`Das.DSL.Types.Apply`) ([Object and class references](Docs/Guide/DSL/ObjectAndClassReferences.md),
  [Structs](Docs/Guide/DSL/Structs.md)) (78a5ae2).

## 2026-10-06

### Added

- Generated bindings from UE reflection (`Scripts/GenerateBindings.ps1`, `-Check`
  for freshness): typed classes, named structs, enums, defaults, arrays, maps and
  sets ([Generated bindings](Docs/Reference/GeneratedBindings.md)) (76931ba).
- Low-level reflected calls: `UObjectHandle`, call frames with multiple
  arguments and out/inout ([Low-level calls](Docs/Reference/LowLevelCalls.md)) (76931ba).
- Persistent per-owner contexts with logic hot reload that keeps the last working
  version ([Apply and hot reload](Docs/Guide/Workflow/ApplyAndHotReload.md)) (76931ba).
- Actor lifecycle callbacks and managed multicast subscriptions
  ([Lifecycle](Docs/Guide/DSL/Lifecycle.md), [Delegates and events](Docs/Guide/DSL/DelegatesAndEvents.md)) (76931ba).
- Native annotation DSL (`require ue_dsl`, `[ue_class]`, `@ue_property`,
  `[ue_function]`) with editor `Das.DSL.Apply` / `Das.DSL.Reimport` and PIE
  reload; scalar, string, `FName` and `double` signatures
  ([Classes](Docs/Guide/DSL/Classes.md), [Apply and hot reload](Docs/Guide/Workflow/ApplyAndHotReload.md)) (f19338f).

## 2026-10-05

### Added

- Plugin foundation: pinned daScript runtime, game-thread execution with
  diagnostics, UFS/Pak file access, native and AOT calls, and an editor
  structural-reload probe that creates real Blueprint classes
  ([Integration choices](Docs/Dev/Architecture/IntegrationChoices.md)) (3dbec34).
