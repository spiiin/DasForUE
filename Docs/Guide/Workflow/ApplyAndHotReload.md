# Apply and Hot Reload

DasForUE2 has two update paths. **Apply/Reimport** console commands turn a `.das` declaration into
Unreal assets (Blueprint classes, reflected enums/structs, Blueprint Interfaces) and update them when
the declaration changes. **Logic hot reload** picks up edited method bodies automatically while the
game or PIE is running, keeping the last working version when the new code is broken.

## Editor console commands

Run these in the Unreal Editor console (or the Output Log command line). The script argument is a
`.das` basename. Scripts are looked up in the project's `Content/Das/Scripts` first, then in
`Plugins/DasForUE/Content/Das/Scripts`; arbitrary external paths are not accepted. Asset paths must
start with `/Game/`.

| Command | Purpose |
| --- | --- |
| `Das.DSL.Apply <script.das> <asset path> <class id>` | Create or update a Blueprint class from a `[ue_class]` declaration |
| `Das.DSL.Reimport <asset path> [policy options]` | Re-read the source bound to an existing DSL Blueprint |
| `Das.DSL.Types.Apply <script.das> <folder> [registry]` | Create reflected enum/struct assets from `[ue_enum]` / `[ue_struct]` declarations, optionally publishing a type catalog |
| `Das.DSL.Types.ExtendEnums`, `Das.DSL.Types.RenameEnumMembers`, `Das.DSL.Types.MigrateEnumValues`, `Das.DSL.Types.MigrateStructFields` | Migrate existing authored types, see [Migrations](Migrations.md) |
| `Das.DSL.Interfaces.Apply <script.das> <folder>` | Create Blueprint Interface assets from `ue_interface` declarations |
| `Das.DSL.Asset.Create <class asset> <new asset>` / `Das.DSL.Asset.Check <asset>` | Create and check authored DataAsset instances, see [Data assets](../Data/DataAssets.md) |
| `Das.DSL.Table.Create <row struct> <new table> <json path>` | Create a DataTable from JSON, see [DataTables and curves](../Data/DataTablesAndCurves.md) |

None of these commands save assets or rewrite source files. Created and changed packages are marked
dirty; review them and save with the normal editor Save.

### Das.DSL.Apply

```text
Das.DSL.Apply dsl_counter_v1.das /Game/DasExamples/BP_Counter example.counter
```

The last argument is the stable class ID (`id=` in `[ue_class]`), not the class name. Apply compiles
the script, builds the authoring model, checks every method, and only then creates or updates the
Blueprint. An existing asset of another type is never replaced.

```das
options gen2
require ue_dsl
[ue_class(module_id="example",id="example.counter",parent="/Script/Engine.Actor",Blueprintable,DisplayName="Counter DSL")]
class Counter {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    @ue_id="example.counter.score"
    @Category="Stats"
    Score : int = 10

    [ue_function(id="example.counter.evaluate",BlueprintCallable)]
    def Evaluate(input:int):int { return self.Score + input }
}
```

Applying a second version of the same class to the same asset updates it in place:

```text
Das.DSL.Apply dsl_counter_v2.das /Game/DasExamples/BP_Counter example.counter
```

The Blueprint stores its source binding (`DasDSLSource`, `DasDSLClassId`) and the canonical
declaration model in package metadata, so it can be reimported later, also after reopening the
editor. Example: `Plugins/DasForUE/Content/Das/Scripts/dsl_counter_v1.das` and `dsl_counter_v2.das`.

### Das.DSL.Reimport

```text
Das.DSL.Reimport /Game/DasExamples/BP_Counter
```

Reimport uses the stored source binding. Without options it picks the safe automatic policy:

- unchanged declaration: only validates the Blueprint (no recompile);
- defaults-only change: merges defaults, preserving manual overrides;
- additive changes (new fields, new methods, metadata/adapter updates): structural update with
  Blueprint reinstancing;
- anything else (rename, removal, type change, editability change, method role change): refused
  unless you pass an explicit migration policy such as `fields=rename_by_id`. See
  [Migrations](Migrations.md).

Syntax/compile errors and unsupported changes are rejected before the Blueprint is touched. If a
failure happens after mutation (for example in the Blueprint compiler), the bounded rollback restores
the previous Blueprint state and reports `Structural migration rolled back:`.

### Das.DSL.Types.Apply

```text
Das.DSL.Types.Apply dsl_type_declarations.das /Game/MyTypes
Das.DSL.Types.Apply dsl_direct_types.das /Game/DasRegisteredTypes/Types /Game/DasRegisteredTypes/TypesRegistry
```

Each declared enum/struct becomes an asset with its declared name in the folder. The optional third
argument publishes a type catalog (registry) used for cooked games and type migrations. Existing
paths are preflighted: an identical repeated declaration reuses the same objects; occupied paths,
identity conflicts and altered definitions are refused (use the migration commands instead). When a
file declares both types and a class, apply the types first, then `Das.DSL.Apply` the class.
Details: [Enums](../DSL/Enums.md), [Structs](../DSL/Structs.md).

### Das.DSL.Interfaces.Apply

```text
Das.DSL.Interfaces.Apply dsl_interface_definition.das /Game/DasInterfaces
```

Creates real Blueprint Interface assets. Identical reapplication reuses the objects; metadata-only
changes update metadata without replacing the generated class; structural contract changes and
conflicting packages are refused before modification. Create and save interfaces before applying
classes that implement them. Details: [Interfaces](../DSL/Interfaces.md).

## Logic hot reload

Each pair "owner object + script file" has its own persistent compiled program and VM context.
Different actors, and the editor world vs. the PIE world, never share script state.

Before each call into a script (method call, lifecycle callback, event, timer, input), the runtime
compares the source file and every file the compiler loaded, including imports. If anything changed,
it compiles a **candidate** version, initializes it and checks the required entry points and
signatures. Only then does the candidate replace the old version, between calls. There is no
background file watcher: the update happens on the next call.

### Last working version is kept

- Compile errors, signature mismatches, a missing required handler or a failing initializer reject
  the candidate; the previous version keeps running with its state. `Reload rejected` is printed to
  the Output Log, and the diagnostic is available on the actor/component as `LastDasError` (widgets
  use `LastInputError` / `LastPaintError` / `LastDragError`).
- An unchanged broken file is not recompiled again, and the same error is not printed on every tick.
- When you fix the source, the next call accepts it automatically.
- A fresh actor that has never had a working version has no fallback: it reports the error and runs
  nothing until the source compiles.
- The candidate's initializer runs without `self`, so it cannot change the live actor before it is
  accepted.
- Runtime errors after acceptance are reported through the error result; already executed side
  effects are not rolled back.

### Body changes vs. declaration changes

- **Body-only change** (method bodies, helper functions): picked up by hot reload. The actor and its
  Unreal property values stay the same.
- **Declaration change** (fields, methods, signatures, metadata): the running context does not switch
  to an unapplied declaration. The runtime compares the candidate's declaration with the current class
  (types, read-only properties, CDO defaults, reflected method signatures) and suggests running
  Apply/Reimport. Apply/Reimport also works while PIE is running: Blueprint reinstancing updates
  instances and references, keeps overrides and gives new fields their defaults.

### What survives a reload

| State | After accepted logic reload |
| --- | --- |
| UPROPERTY values on the actor/component | Kept (the object is unchanged) |
| Annotated globals `var @ue_state Name:Type` | Owned scalar/string values restored before candidate global initialization |
| Ordinary daScript globals | Kept between calls of one version; re-initialized on reload |
| BeginPlay / Initialize delivery | Remembered; not replayed |
| Declared delegate/event subscriptions | Old subscriptions removed, the new version's set installed |
| Named-method timers | Cancelled; start them again from an explicit setup method |
| Inline `$` callback timers | Unchanged generated identity and verified signature: native schedule, pause state and payload kept; handler runs in the new VM |
| Latent actions and async loads | Cancelled; restart them explicitly (e.g. a `StartLoad` method) |
| Input bindings and mapping contexts | Old bindings revoked; the last successfully installed configuration is restored against the new handlers on a following world tick |
| Widget handlers, replies, paint/drag/animation tokens | Old context released; tokens from it are invalid |
| Unmarked VM memory, strings, containers, pointers | Not copied |

A rejected reload changes none of the above: old code, state, subscriptions, timers and bindings stay.
During Apply/Reimport and migrations, idle VM contexts, lifecycle state and managed subscriptions are
preserved across Blueprint reinstancing (see [Migrations](Migrations.md#runtime-state-during-migrations)).

### Annotated state example

```das
options gen2
require ue_foundation

// Explicit state survives logic reload. Each Actor owns its own counter.
var @ue_state count:int = 0
[export]
def main(input:int):int {
    count += input
    return count // Change to count + 100 while playing to observe hot reload.
}
```

Two calls with input `1` return `1`, then `2`; a second actor starts at `1`. After changing the last
line to `return count + 100`, the next calls return `103` and `102` respectively. A syntax error keeps
the previous version running with the same counters. Example:
`Plugins/DasForUE/Content/Das/Scripts/persistent_counter.das`.

See [typed identities and persistent state](../DSL/IdentitiesAndState.md) for supported types, module identity and type-change rejection.

### Ownership and cleanup

Contexts hold a weak reference to their owner. `EndPlay` and `Destroyed` release an actor's contexts
(EndPlay runs with the last accepted version and does not compile new source); world cleanup removes
remaining contexts of that world; post-GC removes contexts of dead objects; shutdown destroys contexts
before daScript stops. Destroy and EndPIE release everything normally.

## Limitations

- Apply/Reimport is a manual editor action; there is no automatic watcher, toolbar UI or standard
  `AssetImportData` factory.
- Compilation and checks are synchronous on the game thread, and each owner compiles its own version.
- Script state of a replaced UObject is not migrated beyond the preservation rules above; there is no
  general migration of arbitrary VM state.
- Rollback is bounded to the operation's own Blueprint/map scope; it is not a general editor
  Undo transaction. First creation of a new Blueprint is not covered by rollback.

Try it: `Scripts/TestDSLPIE.ps1` (Apply/Reimport and reload in PIE).

See also: [Migrations](Migrations.md) · [Lifecycle](../DSL/Lifecycle.md) ·
[Classes](../DSL/Classes.md) · [Defaults](../DSL/Defaults.md) · [Timers](../Gameplay/Timers.md) ·
[Input](../Gameplay/Input.md) · [Delegates and events](../DSL/DelegatesAndEvents.md)
