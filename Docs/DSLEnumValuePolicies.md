# Explicit enum value/removal policies

Edit the enum declaration, preserving names and IDs of surviving members. A
changed number or removed member needs an explicit replacement rule. Save a local
JSON file in Plugins/DasForUE/Content/Das/Scripts:

```json
{
  "schema_version": 1,
  "rules": [
    { "enum_id": "direct.State", "from": 42, "to": 50 }
  ]
}
```

Then run:

```text
Das.DSL.Types.MigrateEnumValues Script.das /Game/Registry.Registry Rules.json
```

For a surviving Running member changed from 42 to 50, the rule must match its new
number. For deletion of Paused=200, a rule such as 200->50 names an existing target
value in the candidate enum. Every changed/deleted value requires one rule;
unknown/duplicate/unused rules and targets outside 0..254 or the candidate domain
are rejected. This policy does not combine addition, rename or member reorder.
Rules are transition-specific; after success, old from-values are no longer a
valid transition. There is no implicit guessing or silent fallback.

The same enum/catalog identity and surviving member IDs remain. Preflight scans
the existing bounded dependency scope and validates old data. It refuses removal
of a connected Switch output or Select option with a link/default override: the
service cannot infer replacement gameplay logic. Empty removed case/options are
discarded during native reconstruction; typed defaults referencing a removed
member can use the explicit replacement member. Other graph links/defaults are
verified, and Blueprint bytecode is rebuilt for the new values/count.

The first value-storage scope is intentionally narrow: declared scalar typed enum
fields on the imported DSL Actor and its instances/CDOs. A snapshot records each
old numeric value and follows object replacement during compilation. Current CDOs
are resolved through their owning Blueprint after compilation. The operation
remaps old values before and after reinstancing; enum model, class/default model,
graphs and changed data are committed together. Failure restores old enum/member
maps, graphs, class/Actor/CDO values and dirty flags.

Container/nested/unrelated enum storage and retired enum archives are rejected
before mutation; map-key/set collision policies require separate work. Existing
single DSL Actor root, ordinary Blueprint/map/catalog, 256-package scope and
stopped PIE/no retained context requirements remain. Assets/maps become dirty;
save them explicitly. Source and rule files are not rewritten by the service.

Test: Scripts/TestDSLTypeRegistry.ps1 -EnumAppend -EnumGraphs -EnumNodes -EnumRename
-EnumValues -Cooked. It changes Running 42->50, then removes Paused 200->50,
checks rollback, linked-branch refusal, saved map/CDO and packaged execution.
See DSLEnumValuePoliciesValidation.md.
