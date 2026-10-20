# Classes

A `[ue_class]` declaration in a `.das` file describes an Unreal class: its parent,
reflected properties, components, defaults, Blueprint functions, events and
lifecycle hooks. `Das.DSL.Apply` turns it into a real Blueprint whose methods run
the daScript bodies. This page shows a complete class, gives an overview of the
annotation set and covers the class-level rules; the detailed pages are linked
from each section.

## A complete example

A trimmed version of `Plugins/DasForUE/Content/Das/Scripts/dsl_playable_character.das`:

```das
options gen2
module dsl_playable_character public
require ue_native_gameplay public

// Assign the mapping context and input actions in Blueprint, then possess
// this Character with a local PlayerController.
[ue_class(Blueprintable)]
class PlayableCharacter : Character {
    properties=ue_properties(EditAnywhere,BlueprintReadWrite) {
        // Mapping Context: WASD, mouse XY and Space.
        var Controls:InputMappingContextRef
        var MoveAction:InputActionRef
        var JumpAction:InputActionRef
    }
    state=ue_properties(BlueprintReadWrite) {
        var Score:int=0
        var ControlsBound:bool=false
    }
    components=ue_components(VisibleAnywhere,BlueprintReadOnly) {
        var Camera:CameraComponentRef
        var Label:TextRenderComponentRef
    }
    defaults=ue_defaults() {
        self.bUseControllerRotationYaw=true
        self.Camera.RelativeLocation=ue_vector(0.0lf,0.0lf,64.0lf)
        self.Camera.bUsePawnControlRotation=true
        self.Label.RelativeLocation=ue_vector(0.0lf,0.0lf,130.0lf)
    }
    def BeginPlay():void {
        self.Label.K2_SetText(ue_loctext(PlayerLabel, "Player"))
        self.CharacterMovement.MaxWalkSpeed=450.0f
    }
    def Tick(delta:float):void {
        if (!self.ControlsBound && ue_character_controls_ready(ue_character_self())) {
            self.BindControls()
        }
    }
    [ue_function(BlueprintCallable)]
    def AddScore(amount:int):void {
        self.Score+=amount
        self.Label.K2_SetText(ue_text_invariant("Score: {self.Score}"))
    }
    [ue_function(BlueprintCallable)]
    def BindControls():void {
        let pawn=ue_character_self()
        ue_enhanced_add_context(ue_pawn_controller(pawn),self.Controls,0)
        ue_enhanced_bind(self.MoveAction,TriggerEvent.Triggered,self.OnMove)
        ue_enhanced_bind(self.JumpAction,TriggerEvent.Started,self.OnJump)
        ue_enhanced_bind(self.JumpAction,TriggerEvent.Completed,self.OnJump)
        self.ControlsBound=true
    }
    [ue_function(BlueprintCallable)]
    def OnMove(value:InputActionValue):void {
        let axis=ue_input_axis2d(value)
        ue_character_move(ue_character_self(),float(axis.y),float(axis.x))
    }
    [ue_function(BlueprintCallable)]
    def OnJump(value:bool):void {
        if (value) { self.Jump() } else { self.StopJumping() }
    }
}
```

What this declares:

- A Blueprint deriving from Unreal's `ACharacter` (through the `ADasCharacter` adapter).
- Editable input-asset references, runtime state fields and two SCS components
  (a camera and a text renderer) created by Unreal for every instance.
- Class-default values for a native property and for component templates.
- Lifecycle hooks (`BeginPlay`, `Tick`) that need no annotation.
- Blueprint-callable functions; `self.Jump()` and `self.CharacterMovement` are
  inherited native members.

Apply it from the editor console, then save the asset:

```text
Das.DSL.Apply dsl_playable_character.das /Game/MyGame/BP_PlayableCharacter dsl_playable_character.PlayableCharacter
```

The last argument is the stable class ID. Try it: `Scripts/CreatePlayableCharacter.ps1`
builds the full example with input assets, HUD and a map.

## Annotation overview

| Annotation | Applies to | Purpose | Details |
| --- | --- | --- | --- |
| `[ue_class(...)]` | `class` | Authored Actor, Pawn, Character or ActorComponent Blueprint | this page, [Inheritance](Inheritance.md) |
| `@ue_property=(...)` | field | One reflected property with flags | [Properties](#properties) |
| `name = ue_properties(...) { var ... }` | field group | Several properties sharing flags/metadata | [Declaration groups](#declaration-groups) |
| `@ue_component` / `ue_components(...) { }` | field / group | SCS component created for each instance | [Components](Components.md) |
| `defaults = ue_defaults() { self.X = ... }` | block | CDO and component-template default values | [Defaults](Defaults.md) |
| `[ue_function(...)]` | method | Reflected UFunction: callable, pure, event | [Functions](Functions.md) |
| lifecycle names / `[ue_lifecycle]` | method | `Initialize`, `ConstructionScript`, `BeginPlay`, `Tick`, `EndPlay` | [Lifecycle](Lifecycle.md) |
| `[ue_event(source=..., name=...)]` | method | Subscribe a method to a multicast delegate | [Delegates and events](DelegatesAndEvents.md) |
| `[ue_library(...)]` | `class` | Static Blueprint Function Library | [Function libraries](FunctionLibraries.md) |
| `[ue_struct(...)]` | `struct` | Authored Unreal struct value type | [Structs](Structs.md) |
| `[ue_enum(...)]` | `enum` | Authored Unreal enum | [Enums](Enums.md) |
| `[ue_interface(...)]` / `ue_implements(interface=...)` | `class` | Blueprint interface and its implementation | [Interfaces](Interfaces.md) |
| `[ue_object_ref(...)]` | `struct` | Typed reference to another authored class | [Authored references](AuthoredReferences.md) |

Identity options (`module_id`, `id`, `@ue_id`), registries and source catalogs are
described in [Modules and type identity](ModulesAndTypeIdentity.md).

## Choosing the parent

Use ordinary daScript inheritance from a native host marker exported by `ue_dsl`:

| Base | Generated Blueprint parent | Notes |
| --- | --- | --- |
| `Actor` (or no base) | Actor bridge host | Default when no parent is given |
| `Pawn` | `ADasPawn : APawn` | Real Pawn with native collision/movement |
| `Character` | `ADasCharacter : ACharacter` | Keeps native capsule root, mesh and `CharacterMovement` |
| `ActorComponent` | `UDasScriptComponent` host | Component Blueprint; add it to an Actor with `@ue_component` |
| another `ue_class` | that class's Blueprint | See [Inheritance](Inheritance.md) |

```das
require ue_dsl

[ue_class(Blueprintable)]
class Counter : Actor {
    properties = ue_properties(EditAnywhere, BlueprintReadWrite) {
        var Score:int=0
    }
}

[ue_class(Blueprintable)]
class ScoreComponent : ActorComponent {
    properties = ue_properties(EditAnywhere, BlueprintReadWrite) {
        var Bonus:int=10
    }
}
```

The legacy spelling `[ue_class(parent="Character", Blueprintable)]` with
`class Player { ... }` selects the same host; short names and full paths such as
`/Script/Engine.Character` both work. Combining `parent=` with an explicit
daScript base is rejected. If a module exports another `Actor` alias, qualify the
base as `ue_dsl::Actor`.

Rules:

- Recognition uses the resolved marker declaration and its `ue_native_base`
  annotation, not the class name: a same-named unannotated class is rejected.
- Arbitrary native parents are not supported; each needs its own adapter.
  Unsupported roots fail explicitly. Changing an existing Blueprint's native parent
  is rejected.
- The markers are type markers, not constructors: creating a daScript value does
  not spawn an Actor. Use spawn and component APIs and typed references
  (`ActorRef`, generated `...Ref` wrappers) for objects.
- Native C++ virtual functions do not become overridable; the native lifecycle
  bridge still chains to native `Super` behavior.
- Replacing a Character's native capsule root with an authored root component
  is rejected.

## Properties

Annotate each field with `@ue_property`. Fields become ordinary Blueprint
variables and are accessed as `self.Score` in methods or `actor.Score` through a
typed reference.

```das
@ue_property=(EditAnywhere, BlueprintReadWrite)
@Category="State"
@DisplayName="Current score"
Title:string="Example"
```

| Option | Meaning |
| --- | --- |
| `EditAnywhere` / `VisibleAnywhere` | Editable or read-only in the Details panel |
| `BlueprintReadWrite` / `BlueprintReadOnly` | Blueprint access; read-only also forbids writes from daScript methods |
| `@Category`, `@DisplayName`, `@ToolTip` | Literal string metadata |
| `@ue_id="module.Class.Field"` | Explicit stable ID (normally automatic) |

Conflicting flags (both read-only and read-write) and unknown or repeated flags
are errors. Every data field of an authored class must be reflected (individually
or in a group); undeclared script-private instance fields are rejected. Supported
types are listed in [Built-in types](BuiltinTypes.md),
[Containers](Containers.md) and [Type support](../../Reference/TypeSupport.md).

Class-level metadata such as `DisplayName` and `ToolTip` can also be given on
`ue_class`.

## Declaration groups

A field initialized with `ue_properties(...) { ... }` declares several properties
at once. The label (`properties`, `state` above) is arbitrary and does not become
a property, a stable ID or runtime data.

```das
[ue_class(Blueprintable)]
class Example {
    properties = ue_properties(EditAnywhere, BlueprintReadWrite, Category="State") {
        var Score:int=0
        var Speed:double=100.0lf
    }
    // Title shown for this object.
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    @Category="State"
    Title:string="Example"

    status = ue_properties(BlueprintReadOnly) {
        var Ready:bool=false
    }
}
```

- Flags mean the same as on `@ue_property`. `Category`, `DisplayName` and
  `ToolTip` are literal named arguments; per-field metadata overrides the group
  value. Conflicting read/visibility flags are errors.
- Groups may contain only explicitly typed mutable `var` declarations. Statements,
  inferred types, references, empty blocks and nested groups are rejected. The
  block never executes.
- Several groups and individual fields can coexist; names must be unique across
  fields and labels.
- `ue_components(...)` is the component equivalent (Actor classes only);
  `ue_properties` also works inside a plain `ue_struct`.
- There is no `ue_functions(...)` group: daScript does not allow `def` inside an
  expression block. Annotate functions individually.

## Comments become tooltips

A comment directly above a reflected declaration becomes its Unreal `ToolTip`:

```das
// Player counter.
[ue_class(Blueprintable)]
class Counter {
    properties = ue_properties(EditAnywhere, BlueprintReadWrite) {
        // Current player score.
        var Score:int=0
    }
    // Returns the score plus the input value.
    [ue_function(BlueprintCallable)]
    def Evaluate(input:int):int { return self.Score+input }
}
```

- Targets: classes, interfaces and their method stubs, reflected methods,
  properties, components, enum types, struct types and struct fields. Individual
  enum entries are not collected.
- Use `//`, `///`, `/* */` or `/** */` above the declaration and its annotations,
  or above each `var` inside a group. Adjacent line comments keep line breaks;
  block indentation and leading `*` are removed. UTF-8 is preserved.
- A blank line or code in between breaks attachment; trailing comments are
  ignored. A comment on a group label is not shared by its members.
- Explicit `ToolTip` metadata (including group metadata) wins. The limit is 4096
  characters.

Fixtures: `dsl_comment_tooltips.das`, `dsl_comment_types.das`.

## Methods in a class

| Method form | Result |
| --- | --- |
| `[ue_function(BlueprintCallable)] def F()` | Blueprint-callable UFunction (final) |
| `[ue_function(BlueprintPure)] def F()` | Pure data node without exec pins |
| `def const F()` | Const method (readonly `self`), combinable with any of the above |
| `[ue_function(BlueprintEvent)] def F()` | Overridable event with a default body |
| `def BeginPlay()`, `def Tick(delta:float)`, ... | Lifecycle hook, not a UFunction |
| `[ue_event(source=owner, source_type=ProbeActorRef, name=OnDasSignal)] def F(...)` | Delegate subscription |
| unannotated `def F()` | Script-only helper, no Blueprint node |

See [Functions](Functions.md), [Lifecycle](Lifecycle.md) and
[Self access](SelfAccess.md) for what method bodies can do with `self`.

## Applying and updating

- `Das.DSL.Apply <file.das> /Game/Path/BP_Name <class id>` creates or updates the
  Blueprint; `Das.DSL.Reimport /Game/Path/BP_Name` reuses the saved binding.
  Assets stay dirty until saved.
- Editing only method bodies is a body-only reload; no Blueprint recompile is needed.
- Declaration changes (fields, signatures, flags, components) go through
  Apply/Reimport and the migration policies; some changes on existing
  component-bearing or Pawn/Character Blueprints are refused rather than applied
  silently. Create a new Blueprint in that case.

See [Apply and hot reload](../Workflow/ApplyAndHotReload.md) and
[Migrations](../Workflow/Migrations.md).

See also: [Inheritance](Inheritance.md) · [Lifecycle](Lifecycle.md) ·
[Components](Components.md) · [Defaults](Defaults.md) · [Functions](Functions.md) ·
[Self access](SelfAccess.md) · [Modules and type identity](ModulesAndTypeIdentity.md) ·
[Examples](../GettingStarted/Examples.md)
