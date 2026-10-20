# Defaults

Defaults are values written into the generated class's CDO and component
templates when the class is imported. They come from inline field initializers
and from an `ue_defaults` block. Defaults are declaration data: they are resolved
at import, saved in the Blueprint, and never re-run at `Initialize`, `BeginPlay`
or construction. Method parameters can have literal defaults too.

## Inline initializers and the `ue_defaults` block

```das
[ue_class(Blueprintable)]
class Example : Actor {
    properties = ue_properties(EditAnywhere, BlueprintReadWrite) {
        var Score:int = 0
        var SpawnType:ActorClass
        var Visual:ObjectRef
        var Pose:UETransform
    }
    components = ue_components(VisibleAnywhere, BlueprintReadOnly) {
        var Mesh:StaticMeshComponentRef
    }
    defaults = ue_defaults() {
        self.Score = 31
        self.SpawnType = ue_class_default(type<ActorRef>)
        self.Visual = ue_asset_default("/Engine/BasicShapes/Cube.Cube")
        self.Pose = ue_transform(ue_vector(10.0lf,20.0lf,30.0lf),
            ue_rotator(0.0lf,45.0lf,0.0lf), ue_vector(2.0lf,2.0lf,2.0lf))
        self.Mesh.StaticMesh = ue_asset_default("/Engine/BasicShapes/Cube.Cube")
        self.InitialLifeSpan = 30.0f
    }
}
```

- The label (`defaults`) is arbitrary and disappears from the compiled class. The
  block is consumed before type inference; it creates no runtime field or closure.
- An assignment in the block overrides the field's inline initializer.
- Duplicate assignments, unknown or private fields and replacing a component
  reference are rejected.

| Target | Writes to |
| --- | --- |
| `self.Property` (own field) | Generated class CDO |
| `self.Property` (field of an authored ancestor) | This class's CDO only ([inherited properties](#inherited-properties)) |
| `self.NativeProperty` (e.g. `InitialLifeSpan`, `bUseControllerRotationYaw`) | Native host CDO value ([native host](#native-host-properties-and-subobjects)) |
| `self.Component.Property` | That component's SCS template in this Actor |
| `self.RootComponent.Prop`, `self.CharacterMovement.Prop` | Native default subobject of the host |

Examples: `dsl_extended_defaults.das`, `dsl_native_components.das`.
Try it: `Scripts/TestExtendedDefaults.ps1` (add `-PIE` or `-Package`).

## Value forms

| Value | Spelling |
| --- | --- |
| int, float, double, bool, string | literals (or [computed expressions](#computed-defaults)) |
| FName | `ue_name_value("Speed")` |
| enum | `MyEnum.Member` |
| FText | `ue_default_text(...)` / text literal |
| Vector, Vector2D, Rotator, Transform | `ue_vector(...)`, `ue_vector2d(...)`, `ue_rotator(...)`, `ue_transform(t,r,s)`, `ue_transform_identity()` |
| typed reflected struct | `ue_default_text("(Field=Value,...)")` (type taken from the property) |
| `UEStructValue` property | `ue_struct_default("/Script/Module.Type", "(...)")` |
| class reference | `ue_class_default(type<ActorRef>)` |
| asset reference | `ue_asset_default("/Game/Folder/Asset.Asset")`; `""` means null |
| arrays, sets, maps | gen2 literals, see [containers](#container-defaults) |

- In declaration defaults, `ue_class_default(type<T>)` and `ue_asset_default` are interpreted as data. Ordinary `ue_class_default(type<T>)` calls return the typed CDO view; see [typed identities](IdentitiesAndState.md). Import validates the target against the property's class or
  MetaClass. Asset defaults must be top-level assets (not Actors, components, CDOs
  or classes); class defaults may be abstract.
- The result is a normal hard reference on the CDO/template, so Unreal saves it and
  cook discovers the dependency. Soft/weak properties are not valid targets.
- Non-null defaults for authored object references are not supported.
- Values are checked for exact kind, range and finiteness.

## Component templates

`self.Component.Property` sets a value on that Actor's SCS template. Native and
authored components accept editable scalar, name, math, struct, class and asset
values. It changes neither the component class CDO nor sibling templates; a new
Actor receives the values when its components are constructed.

```das
defaults = ue_defaults() {
    self.Child.RelativeLocation = ue_vector(0.0lf, 0.0lf, 50.0lf)
    self.Child.RelativeRotation = ue_rotator(0.0lf, 30.0lf, 0.0lf)
    self.Child.bHiddenInGame = true
    self.Behavior.Bonus = 42
}
```

Only one property level is supported (`self.Child.RelativeLocation`, not deeper
paths). Inherited components use the same syntax
([Inheritance](Inheritance.md#components-in-a-hierarchy)).

## Native host properties and subobjects

Editable reflected properties of the native base can be set without redeclaring
them:

```das
[ue_class(module_id="gameplay",Blueprintable)]
class TimedActor : Actor {
    defaults=ue_defaults() {
        self.InitialLifeSpan=10.0f*3.0f
        self.bHidden=false
        self.RootComponent.RelativeScale3D=ue_vector(1.0lf,2.0lf,3.0lf)
    }
}

[ue_class(Blueprintable)]
class Player : Character {
    defaults=ue_defaults() {
        self.BaseEyeHeight=55.0f
        self.bUseControllerRotationYaw=true
        self.CharacterMovement.MaxWalkSpeed=450.0f
    }
}
```

- `RootComponent`, `CharacterMovement` and similar names identify the host's fixed
  native default subobjects; no SCS component is created or replaced. ActorComponent
  hosts accept native component defaults such as `bAutoActivate`.
- Names use the exact, case-sensitive Unreal spelling. Unknown, non-editable,
  edit-const or template-disabled targets are rejected before an asset is created.
- Native enum and container defaults are not supported on native host properties.
- Authored descendants inherit these values and may override them.

Examples: `dsl_native_defaults.das`, `dsl_native_defaults_child.das`,
`dsl_native_defaults_component.das`, `dsl_pawn_player.das`, `dsl_character_player.das`.
Try it: `Scripts/TestNativeDefaults.ps1`.

## Inherited properties

A child can set defaults for editable properties declared by any authored
ancestor. Do not redeclare the property:

```das
// BaseCounter is imported from its public source module.
[ue_class(module_id="game",Blueprintable)]
class ChildCounter : BaseCounter {
    defaults=ue_defaults() {
        self.Score=20
        self.Position=ue_vector(4.0lf,5.0lf,6.0lf)
        self.SpawnType=ue_class_default(type<ActorRef>)
    }
}
```

- The parent keeps the property, its type, metadata and stable ID; no child variable
  is generated. Only the child's CDO changes. Works for Actor and ActorComponent
  hierarchies. `BlueprintReadOnly` does not block an editable class default;
  non-editable fields are refused.
- The model stores these as `property_overrides` keyed by the ancestor property ID;
  the nearest override in the chain wins.
- Import parents before children; a child update is refused if an ancestor's
  declaration or defaults have not been applied yet.

Try it: `Scripts/TestInheritedDefaults.ps1`.

## Computed defaults

Initializers and `ue_defaults` assignments may use pure scalar expressions,
including calls to ordinary helper functions:

```das
def starting_score(seed:int):int { return seed * 4 }

[ue_class(module_id="gameplay",Blueprintable)]
class Counter : Actor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Seed:int=2+3
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Score:int=0
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    NextScore:int=0

    defaults=ue_defaults() {
        self.Score=starting_score(self.Seed*2)
        self.NextScore=self.Score+2
    }
}
```

This yields CDO values `Seed=5`, `Score=40`, `NextScore=42`.

- Results: `int`, `float`, `double`, `bool`, `string`. Math constructors take computed
  scalar coordinates; `ue_asset_default`, `ue_class_default`, `ue_default_text` and
  the text of `ue_struct_default` accept computed strings. Type identities stay static.
  A helper returning a whole vector, transform, struct or container is not
  supported; build it from scalar results.
- `self.Field` reads **source** defaults: earlier assignments in the block, inline
  initializers, inherited source values and editable native scalar defaults. It
  never reads a live object, the CDO or manual editor changes. Component state and
  instance methods are not available.
- Allowed: scalar locals, conditionals and pure builtin/math/string functions.
  Rejected: global state, UObject/World calls, indirect calls, unsafe code, loops,
  recursion, object allocation, generators, cycles, non-finite or oversized results.
  The evaluator runs in an isolated context with depth and memory budgets. It is
  a constrained defaults facility, not a sandbox for untrusted code.
- Only resolved values are saved; no evaluator survives into the compiled program.
  If a helper changes, reimport updates values that still match the old source
  default and keeps manual overrides. Runtime work belongs in
  `Initialize`/`ConstructionScript`/`BeginPlay`.

Example: `dsl_computed_defaults.das`. Try it: `Scripts/TestComputedDefaults.ps1`.

## Container defaults

```das
def start_value(seed:int):int { return seed * 4 }

[ue_class(module_id="gameplay",Blueprintable)]
class Example : Actor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Seed:int=2
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Numbers:array<int> = [1,start_value(self.Seed)]
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Tags:table<string> = {"red","blue"}
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Weights:table<UENameValue;double> = {ue_name_value("Speed")=>2.5lf}
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Assets:array<CurveAssetRef> = [
        ue_asset_default("/Game/DasAssetDefaults/ArrayAsset.ArrayAsset"),
        ue_asset_default("")
    ]
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    AssetMap:table<UENameValue;CurveAssetRef>

    defaults=ue_defaults() {
        self.Numbers=[self.Seed,start_value(self.Seed)]
        self.AssetMap=({ue_name_value("Main")=>ue_asset_default("/Game/Curves/Main.Main")})
    }
}
```

- `CurveAssetRef` is a native wrapper constrained to `/Script/Engine.CurveFloat`
  (see `ue_default_assets.das`); `ObjectRef` works when no narrower type is needed.
- Put a space between `>` and `=` in the type (`array<int> =`); otherwise daScript
  lexes `>=`.
- Inside `ue_defaults`, parenthesize table literals: a bare `{` at statement level is
  a block in gen2.
- Supported: arrays of int/float/double/bool/string/name/enum/math values and asset
  references; sets of int/string/name/enum/object keys; maps with those keys and
  scalar/name/enum/math/asset/struct values. Structs may nest containers. Empty
  literals work for every supported container type.
- Scalar leaves may be computed. Whole-container helper results and indexing in
  calculations are not supported. Authored object-reference keys/values and opaque
  structs cannot have non-empty defaults.
- Duplicate keys/elements are rejected using Unreal equality (FName comparison is
  case-insensitive).
- Each container is merged as **one value**: if it still equals the old source
  default it is replaced, otherwise the manual edit is kept whole.
- Limits per value tree: depth 16, 8192 typed nodes, 4096 container entries, 16 MiB
  storage. Errors name the path, e.g. `default.Catalog[0].value.Assets[1]`.

## Parameter defaults

Trailing input parameters of `ue_function` methods (and static library functions)
can have literal defaults:

```das
[ue_class]
class Counter : Actor {
    [ue_function(BlueprintPure)]
    def Add(left:int=7; right:int=-2):int { return left+right }

    [ue_function(BlueprintCallable)]
    def Example():int {
        return self.Add()+self.Add(10)+self.Add(10,3) // 5+8+13
    }
}
```

- Types: `int`, `float`, `double`, `bool`, `string` literals (negative numbers,
  Unicode and empty strings work).
- Not allowed: defaults on out/ref or WorldContext parameters, complex types,
  `UENameValue`, or computed expressions.
- Omitted arguments work in same-object calls, in calls through typed references
  (including from free functions), and on disconnected Blueprint pins. `ProcessEvent`
  and C++ callers must still supply all parameters.
- Defaults are call-site values filled in before the call; they do not change
  virtual dispatch or `Super` rules.
- Changing a default needs a reimport; it updates metadata and new nodes. Pin values
  that users set explicitly are kept.

Examples: `dsl_defaults_actor.das`, `dsl_defaults_component.das`,
`dsl_defaults_child.das` and their `_calls.das` companions.

## Reimport and migration

- Initial import applies defaults to the new CDO, templates and native subobjects.
- Default-only changes reimport with a safe three-way merge: values that still
  equal the previous source default receive the new value, manual CDO/instance/
  Blueprint edits are kept, and removing a default returns unchanged values to the
  parent's actual default. An explicit child default blocks parent propagation even
  if equal to the old parent value. The merge does not reinstance objects or rerun
  gameplay; save the packages afterwards.
- `Das.DSL.Reimport /Game/Folder/BP_Child` uses `PreserveOverrides` automatically
  for default-only changes.
- Changing component-template defaults or inherited-component overrides on an
  existing imported class is still refused; create a new Blueprint.

Full policies: [Migrations](../Workflow/Migrations.md).

See also: [Classes](Classes.md) · [Components](Components.md) ·
[Inheritance](Inheritance.md) · [Containers](Containers.md) ·
[Soft references](SoftReferences.md) · [Apply and hot reload](../Workflow/ApplyAndHotReload.md)
