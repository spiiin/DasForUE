# Shared reflected defaults

Defaults now use one recursive materializer rather than a collection of supported
container/type pairs. Each value is interpreted against its FProperty descriptor.
Arrays recurse into Inner, sets into ElementProp, maps into KeyProp/ValueProp and
structs into their reflected fields. Scalar, enum, name, text, class and asset
handlers are reused at every position allowed by the binding's type policy.

## Pipeline

1. The DSL lowers literals, source markers and audited computed scalar leaves
   into JSON values. Container construction is recursive; helpers returning whole
   containers are still outside the evaluator's policy.
2. Strict JSON decoding checks duplicate fields, syntax and wire limits. The
   expected FProperty supplies the type; JSON never chooses a native type.
3. ReadDefault builds a temporary FDasDefaultValue tree. Nodes carry the expected
   property and diagnostic path. No VM handle or loaded UObject enters this tree.
4. WriteDefault materializes the tree in temporary native storage. Asset/class
   loading checks use the same reference resolver at every position. Duplicate
   keys/elements are detected using Unreal equality before rehashing.
5. FDasDefaultRoots keeps snapshots, their type owners and loaded references alive
   during loading and staging. The destination is copied only after success.
   Published UPROPERTY values use normal Unreal GC, serialization and cook rules.

ParseDasPropertyDefault is the common atomic property API; passing Data=nullptr
validates without modifying a destination. ParseDasStructDefault also accepts
structured JSON, and container carriers use the same decoder/materializer.
ExportDasPropertyDefault converts native values into this representation.
DasParseEditableDefault adapts native/component source literals to the shared API
while retaining the existing editability policy. Component creation/attachment
and default migration remain their own operations.

The existing defaults merge still treats each container as one value and preserves
manual edits. Repeated source validation does not reapply defaults to instances.

## Value representation

- Numbers and booleans: JSON primitives, with reflected range/finite checks.
- FString/FName and enum members: strings, checked against the expected property.
- FText: {"text":"NSLOCTEXT(...)"} or an invariant text literal. Namespace/key and
  localization history survive export/import; display text alone is not used.
- Asset references: {"asset":"/Game/Package.Object"}; an empty path means null.
- Class references: {"class":"/Script/Engine.Actor"}, constrained by MetaClass.
- Arrays/sets: arrays of recursively encoded values.
- Maps: arrays of [key,value] pairs.
- Structs: objects keyed by reflected/authored field names. Omitted fields retain
  their initialized native defaults. Unknown and duplicate field identities fail.

Math constructors lower to struct field objects too; persisted math values no
longer need a separate textual struct decoder. A struct field can contain arrays,
sets/maps and further supported structures without another materializer branch.
For direct struct elements the existing ue_default_text marker may carry a literal JSON
field object. A new ergonomic struct-constructor DSL is not introduced here.

There is no migration layer for old saved container defaults. Reimport or recreate
the test Blueprints/maps with the new collector. Adapting explicit native-text
source literals is a syntax boundary, not a saved-asset compatibility decoder.

## Scope and limits

The binding's supported reflected type shapes remain authoritative. A recursive
codec does not make arbitrary UE types, opaque/custom-ARO structs, fixed arrays,
recursive types or nested bare container declarations available automatically.
Native reference constraints still apply; persisted references denote top-level
assets/classes, not transient actors or subobjects. Authored object-reference
keys/defaults remain outside this slice.

Limits are shared across a value tree: depth 16, 8192 typed nodes, 4096 total
container entries, 16 MiB native/string budget, 1 MiB input JSON and bounded leaf
strings. JSON scanning has an independent token/nesting limit. Invalid values
produce a property path such as default.Catalog[0].value.Assets[1].

Adding another leaf type requires its source lowering and leaf codec once.
Array/map/set combinations then reuse those handlers subject to type support;
new combinations still need tests, not a new conversion pipeline.

## Tests

The computed-defaults fixture now covers asset sets, int/string/name-to-object
maps, object-to-double maps, null keys/elements and computed asset paths. Existing
manual edits, source merge, save/reopen/recompile and fresh Spawn checks remain.
CheckDasRecursiveDefaults exercises a struct containing nested arrays, maps of
structs, math, names and references; it runs in Editor and packaged probes.
It checks forced GC, typed roundtrip, byte limits, text localization, duplicate
JSON/Unreal identities, invalid fields/types and atomic failure of a partial load.

## Validation 2026-10-17

Final Editor and Win64 Development builds pass. Create/Reopen/Recompile pass,
including recursive typed roundtrip, forced GC, byte/text identity, malformed and
coerced JSON refusal, atomic failure and manual overrides. Cook/stage/IoStore/
archive and the packaged computed-defaults probe pass. Packaged foundation reports
PASS failures=0. Final commandlet/game logs contain no Error/Fatal entries.

Map: /Game/DasComputedDefaults/249a0e612a37424abc37df4f5efe01ca/Map.
Archive: Build/GenericDefaultsPackaged.
Evidence: logs/generic-defaults-editor-verified-build.log,
generic-defaults-verified-suite.log, generic-defaults-{Create,Reopen,Recompile}.log,
generic-defaults-receipt.log, generic-defaults-package.log,
generic-defaults-package-final-run.log, generic-defaults-runtime-packaged.log,
generic-defaults-foundation-packaged.log. The IoStore index in
generic-defaults-iostore.csv includes ArrayAsset and MapAsset and excludes GCOnly.

Validation uses Win64 Development with NullRHI. This slice does not claim a new
Shipping or dedicated PIE run. Re-run Scripts/TestComputedDefaults.ps1, optionally
with -Package, to recreate assets under the current representation.