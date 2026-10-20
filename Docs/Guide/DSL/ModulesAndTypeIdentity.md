# Modules and type identity

Every authored class, struct, enum, interface, property and function has a
**stable ID** that links the daScript declaration to its Unreal asset across
renames and reimports. This page explains how IDs are derived, how source type
names are resolved to Unreal types, and how registries and source catalogs make
those lookups explicit.

## Stable IDs and module_id

Without options, IDs are derived from the source file and declaration names:

| Declaration | Default ID |
| --- | --- |
| class `ScoreBehavior` in `dsl_component_behavior.das` | `dsl_component_behavior.ScoreBehavior` |
| member `Bonus` | `dsl_component_behavior.ScoreBehavior.Bonus` (automatic) |
| with `module_id="game"` | `game.ScoreBehavior` |

```das
[ue_class(module_id="example", id="example.counter", Blueprintable)]
class Counter {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    @ue_id="example.counter.score"
    Score : int = 10

    [ue_function(id="example.counter.evaluate",BlueprintCallable)]
    def Evaluate(input:int):int { return self.Score + input }
}
```

- The namespace defaults to the file name without `.das`. Set `module_id` when you
  want a stable shared namespace, especially before renaming a source file.
- `id=` on `ue_class`/`ue_struct`/`ue_enum`/`ue_interface`/`ue_function`, and `@ue_id`
  on fields and components, set explicit IDs. Keep the old ID when you rename a
  declaration so the existing asset still matches.
- `Das.DSL.Apply <file> <asset> <class id>` takes the class ID, not the class name.
- Assets created under one identity are not migrated to another: changing the ID
  means importing into a new asset.

## Resolving type names

Wherever an annotation or signature names a type, the bridge resolves it, checks
its kind (class, interface, enum, struct) and stores the canonical Unreal path.
Ambiguous names are rejected; there is no first-match guessing.

| Spelling | Resolves to |
| --- | --- |
| `/Script/Engine.Actor`, `/Game/Folder/Settings.Settings` | that exact object, kind-checked |
| `Actor`, `DasCallPayload`, `EDasCallMode` | native object of the expected kind, or an imported native alias |
| `Engine.Actor`, `DasRuntime.DasCallPayload` | loaded native `/Script/Module.Type` |
| `GameSettings`, `PlayState` | a source declaration (current module or imports), including its explicit ID |
| `direct.Settings`, `direct.State` | a full stable ID through the selected registry |
| `CounterContract` | a source interface, or `module_id.CounterContract` |

Unquoted identifiers in annotations are plain strings; the bridge performs the
lookup. Source declarations in the current module and in imported modules are both
considered.

```das
[ue_class(module_id="game", Blueprintable),
 ue_implements(interface=CounterContract)]
class CounterActor : Actor { /* reflected fields and methods */ }
```

### Raw handle constraints

Natural typed signatures (`PlayState`, `GameSettings`, `ActorRef`, `UEVector`)
infer their Unreal type. Raw handles (`UEStructValue`, `UEEnumValue`,
`UObjectHandle`) need constraint options on `ue_function`:

```das
[ue_class(module_id="game", Blueprintable,
          type_registry="/Game/Types/TypesRegistry.TypesRegistry")]
class Example : Actor {
    [ue_function(BlueprintCallable,
                 arg_value_struct=GameSettings,
                 return_struct="direct.Settings")]
    def Echo(value:UEStructValue):UEStructValue { return value }
}
```

Options are `arg_<param>_class/enum/struct` and `return_class/enum/struct`. A
redundant constraint on a typed parameter is accepted only if it resolves to the
same type; a conflicting one is an error. Constraints on parameters of an unrelated
kind are unused-option errors.

### `ue_type` in method bodies

Inside reflected methods and field initializers, low-level calls that take a type
identity accept `ue_type(Name)` instead of a hard-coded path:

```das
[ue_function(BlueprintCallable, arg_target_class=Object)]
def Supports(target:UObjectHandle):bool {
    return ue_implements_interface(target, ue_type(DasProbeInterface))
}
```

| Calls | Type argument position |
| --- | --- |
| `ue_object_value` | first: native class |
| `ue_cast_object`, `ue_cast_nullable_object`, `ue_object_is_a` | second: native class |
| `ue_implements_interface` | second: interface |
| `ue_enum_value`, `ue_enum_from_number` | first: enum |
| `ue_struct_value`, `ue_struct_default` | first: struct |

- `ue_type` is a compile-time marker: it accepts one identifier, alias or literal
  (`ue_type("Engine.Actor")`, `ue_type("game.CounterContract")`), never an expression.
  The call decides the expected kind, so an Actor passed as an interface is rejected.
- A short string literal in the same position resolves the same way:
  `ue_struct_value("Settings")`, `ue_enum_value("State", "Idle")`. Full paths keep
  runtime validation; dynamic strings keep the low-level behavior.
- Scope is the reflected class method or initializer. Standalone functions,
  helpers and other modules do not inherit the class's registry.
- A type token does not add a cook dependency.

Prefer natural typed fields and signatures; `ue_type` is for the snapshot and
interface-frame APIs. Examples: `dsl_type_calls.das`, `dsl_type_syntax.das`,
`dsl_type_syntax_registered.das`.

## Enum and struct lookup: type_folder and type_registry

Authored enums and structs are separate assets. A class or library that uses them
by name selects exactly one lookup source:

| Option | Lookup |
| --- | --- |
| `type_folder="/Game/Folder/Types"` | exact folder/name convention |
| `type_registry="/Game/Folder/TypesRegistry.TypesRegistry"` | stable IDs through a `UDasTypeRegistry` asset |

The two are mutually exclusive. A missing, ambiguous or wrong-kind authored type
never falls back to unrelated global assets. If neither the registry/folder nor a
source declaration matches, a unique native `/Script` type may still resolve.

A registry stores, per entry, the ID, kind, a hard asset reference and (for structs)
the persistent GUID; enum assets also serialize their ID. It works in cooked builds
without editor metadata. Renamed source declarations that keep their IDs still
resolve the original assets:

```das
[ue_enum(id="direct.State", module_id="direct", BlueprintType)]
enum PlayState {
    Idle = 7
    Active = 42
}

[ue_class(module_id="authoredvalues",
          type_registry="/Game/DasRegisteredTypes/TypesRegistry.TypesRegistry",
          Blueprintable)]
class TypedActor {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Mode : PlayState = PlayState.Idle

    [ue_function(BlueprintCallable)]
    def SetMode(value:PlayState):void { self.Mode = value }
}
```

Create the types and registry, then the class:

```text
Das.DSL.Types.Apply dsl_direct_types.das /Game/DasRegisteredTypes/Types /Game/DasRegisteredTypes/TypesRegistry
Das.DSL.Apply dsl_registered_types.das /Game/DasRegisteredTypes/BP_Actor authoredvalues.TypedActor
```

- The optional third `Types.Apply` argument publishes the registry; reapplying the
  same set reuses it. Save the types, registry, Blueprint and map.
- **Cooking:** a path string in daScript is not a cook dependency. Assign the
  registry to the Actor's `Type Registry` property in the map, or otherwise give
  your project a hard reference; the registry then keeps its types in the cook.
- Duplicate or missing IDs, duplicate assets, wrong kinds or GUIDs and changes to
  existing bindings are rejected. Rebinding registry entries needs an explicit
  migration. There is no global asset scan.
- Short names: a matching module-local ID wins; otherwise a unique registered ID
  leaf or asset name may match.

Example: `dsl_registered_types.das`. Try it: `Scripts/TestDSLTypeRegistry.ps1`.

## Interfaces without a folder

Interfaces accept exact paths, native names and authored stable IDs. With a
`type_folder`, authored short names are restricted to that folder. Without one,
authored interfaces are found through saved searchable tags in the editor and
loaded generated classes in cooked builds. See [Interfaces](Interfaces.md).

## Source catalog

A typed reference to another authored class normally names its script:

```das
[ue_object_ref(script="counter_actor.das", type="CounterActor")]
struct CounterActorRef { handle:UObjectHandle }
```

With a source catalog (`ue_type_catalog.json` in the runtime Scripts folder) the
script can be omitted, and `type` may be a class name or a stable ID:

```das
[ue_object_ref(type="CounterActor")]
struct CounterActorRef { handle:UObjectHandle }
```

Select a different catalog with `catalog="my_catalog.json"`. Unknown or ambiguous
names fail; `script=` together with a catalog is rejected. The expected stable ID
is checked when the source declaration resolves, so a stale entry cannot bind a
different class. Catalog edits join compile dependency tracking and the
last-good reload rules.

### Generating catalogs and reference modules

The `DasSourceCatalog` commandlet builds the catalog and a module with reference
wrappers for all selected classes, from the compiled authoring model:

```powershell
UnrealEditor-Cmd.exe YourProject.uproject -run=DasSourceCatalog `
  -Scripts=counter_actor.das,score_component.das `
  -Output=ue_type_catalog.json -Module=ue_project_refs -unattended -nop4
```

```das
require ue_project_refs
let actor = ue_ref(type<CounterActorRef>, target)
actor.AddScore(5)
```

| Option | Purpose |
| --- | --- |
| `-Scripts=a.das,b.das` | Class roots whose reference wrappers are generated |
| `-Output=`, `-Module=` | Catalog file and generated module name |
| `-TypeScripts=` | Extra enum/struct modules to re-export (type-only catalogs omit `-Scripts`) |
| `-ExplicitTypeImports` | Disable automatic discovery; use only `-TypeScripts` |
| `-InterfaceScripts=` | Interface declaration or binding modules to include |
| `-Check` | Compare existing outputs without writing |

- By default, enum/struct modules imported by the selected classes (including
  transitive imports) are discovered and re-exported publicly, so one `require`
  exposes both wrappers and value types. Module names must match their file names.
- Modules mixing `ue_class` with value or interface declarations cannot be
  consolidated; keep value types in dedicated modules.
- The catalog does **not** generate interface method adapters; use the binding
  generator for those and include the module with `-InterfaceScripts`.
- Output is sorted by stable ID; duplicate class names get hashed wrapper names.
  Duplicate IDs, locations and name collisions are refused. The catalog and module
  are written separately, so regenerate both after a failed write.
- Limits: schema version 3 (1 and 2 still readable), 1024 classes, 256 value
  modules, 1024 value types, 1 MiB. Files stay inside the Scripts folder.

Examples: `ue_example_source_catalog.json`, `ue_example_catalog_refs.das`,
`ue_gameplay_catalog.json`, `dsl_catalog_access.das`.

## Annotation hygiene

- `ue_function`, `ue_class` and other declaration annotations are singletons:
  repeating them, or repeating a scalar option even with the same value, is an
  error. `ue_implements` is repeatable.
- Contradictory declaration kinds are rejected.
- `out=`/`ref=` lists may repeat, but parameter names must stay unique.

See also: [Classes](Classes.md) · [Structs](Structs.md) · [Enums](Enums.md) ·
[Interfaces](Interfaces.md) · [Authored references](AuthoredReferences.md) ·
[Migrations](../Workflow/Migrations.md)
