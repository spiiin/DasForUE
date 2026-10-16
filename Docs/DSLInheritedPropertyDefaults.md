# Defaults on inherited authored properties

An authored child can set defaults for editable properties declared by any authored
ancestor, using the existing static `ue_defaults` syntax. Do not redeclare the
property in the child.

```dascript
// BaseCounter is imported from its public source module.
[ue_class(module_id="game",Blueprintable)]
class ChildCounter : BaseCounter {
    defaults=ue_defaults() {
        self.Score=20
        self.Position=ue_vector(4.0lf,5.0lf,6.0lf)
        self.SpawnType=ue_class_default("/Script/Engine.Pawn")
    }
    [ue_function(BlueprintCallable)]
    def Read(input:int):int { return self.Score+input }
}
```

The parent still owns the same FProperty, type, metadata and stable ID. No child
member variable or duplicate property is generated. Only the child's CDO changes;
the parent CDO, sibling classes and declarations keep their own defaults. This
works with authored Actor and ActorComponent inheritance. Literal scalars, enums,
supported math/reflected structs and strong class/asset defaults use the existing
typed default parsers. BlueprintReadOnly does not prohibit an editable class default.
Noneditable fields, unknown fields, duplicate assignments, incompatible literals,
containers/delegates and executable expressions are refused.

## Representation and ownership

An owned source declaration stores inherited defaults in optional
`property_overrides` records. `properties` still contains only owned definitions.
An override references the ancestor property ID and exact name; its type and
access policy come from that ancestor. The effective view resolves the chain from
base to leaf and applies the nearest override. Old JSON with no overrides remains
unchanged.

Partial JSON example (not a complete import document):

```json
{
  "id": "game.ChildCounter",
  "parent_id": "game.BaseCounter",
  "properties": [],
  "property_overrides": [
    {"id": "game.BaseCounter.Score", "name": "Score", "default": 20}
  ]
}
```

The DSL generates these IDs automatically. The record order is not significant.
Runtime validation uses the CDO of the authored class that supplies the nearest
default override, while the reflected property's storage remains owned by its
original declaring class. Base method calls on a child continue to use the correct
instance values. Cooked source fingerprints allow legitimate manual CDO edits.

## Reimport and parent updates

Use `Das.DSL.Reimport /Game/Folder/BP_Child`. Default-only changes use
`PreserveOverrides` automatically; the explicit policy is also supported.

* Changing an inherited default merges previous source, actual child CDO and new
  source values, preserving manual child/instance/ordinary Blueprint edits.
* Adding an override compares against the actual current parent CDO.
* Removing an override returns unchanged values to the actual current parent CDO,
  preserving manual differences.
* An explicit child override blocks propagation of that parent's default, even
  when the explicit value equals the old parent default. Descendants without their
  own override follow the merged value of their immediate archetype.
* Existing inherited component-template overrides remain a separate representation.

Changed defaults do not compile/reinstance Blueprints or rerun construction.
Snapshots cover values, source contracts and import metadata; the same injected
rollback stages and idle/world/dependency preflight apply as in
[DSLDefaultsMigration.md](DSLDefaultsMigration.md). Packages remain dirty until
saved. Compile existing edited Blueprints before requesting a safe merge.
If an ancestor's owned declaration/defaults differ from its imported contract, update that parent first. A child default update refuses before mutation and names the unapplied parent; stale ancestor snapshots inside otherwise unchanged parent models do not cause a false conflict. Initial/body-only child import uses the parent's automatic reimport policy, retaining approved
manual parent defaults. New child defaults survive save/reopen and later Blueprint
compilation without creating fake child variable descriptors.

The low-level `FDasBlueprintService::ApplyAuthoring` refuses override-bearing
models: use `FDasDSLService` so source provenance and merge policy are retained.

## Scope

This stage covers properties declared by authored ancestors in the source chain.
It does not project arbitrary native-parent UPROPERTYs into daScript, add native
host adapters such as Character, change property type/flags, or introduce computed
defaults. Nested native component properties continue to use their existing
`self.Component.Property` syntax and component policy.

## Verification

```powershell
.\Scripts\TestInheritedDefaults.ps1
.\Scripts\TestInheritedDefaults.ps1 -Package
```

The actor fixture covers three authored levels, ordinary Blueprint descendants,
manual parent/child CDO and placed-instance edits, scalar/struct/class/asset
defaults, addition/removal, explicit values equal to old parent defaults, two
rollback stages, an existing VM, order-independent repeated import, save/reopen
and Blueprint recompile. Negative DSL fixtures reject unknown/duplicate/type/
expression assignments before asset creation; IR checks reject duplicate and
noneditable overrides. The component fixture exercises inherited scalar defaults,
manual CDO/instance values, fresh registration and runtime validation. The optional
Win64 Development NullRHI package checks saved values and reflected calls through
base, child and leaf entries, including cooked component defaults.

Validation passed on 2026-10-16: Editor build; Create/Reopen/Recompile; actor and ActorComponent defaults; three-level and ordinary Blueprint inheritance; add/remove and equal-old-parent overrides; two rollback stages; existing VM; order-independent body-only reimport; unapplied-parent preflight; four negative DSL cases and two negative IR cases; Win64 Development NullRHI packaged probe including cooked component defaults; packaged/editor foundation with zero failures. Existing safe-default merge Create/Reopen/Recompile passed. Its Unopened phase initially hit an engine ZenServerState startup assert before the commandlet; a direct retry with -ddc=NoZenLocalFallback -NoZenAutoLaunch passed, as did editor foundation with the same filesystem-DDC flags. No engine/cache service changes were made.
