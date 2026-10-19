# Reflected type declaration backend validation — 2026-10-07

UE 5.8.3 / pinned daScript 0.6.4, Win64 Development.

- TestDSLTypesAsset: separate Create/Reopen processes PASS. Native annotation
  extraction creates enum and struct assets; enum values 7/42 and struct Count=10,
  Unicode Title default survive serialization and loading.
- An ordinary Blueprint accepts the generated enum/struct as property types,
  compiles and saves/reopens with the same type identities. Its enum default Active
  retains value 42. Exact repeated Apply reuses assets; changed declarations refuse
  before mutation. Native enum and struct layout/default/identity drift is checked.
- UUserDefinedEnum editor serialization was experimentally confirmed to renumber
  sparse values; runtime UDasDefinedEnum avoids that path. No engine source changed.
- TestDSLObjectFieldsPIE regression PASS, one successful automation test without
  warnings; existing UObject field/migration/GC behavior remains intact.
- TestEditor: 411 foundation checks, failures=0, PASS. New checks cover both native
  annotations, explicit values/scalar fields, model roundtrip, duplicate/reserved
  enum values, unsupported object struct fields and implicit enum-value rejection
  without partial output mutation.
- TestPackaged: native build/cook/stage/archive/launch, 411 foundation checks,
  failures=0, PASS. The declaration example is included by verified dependencies.
  Packaged tests exercise compile-only type declaration collection/model validation;
  the editor-generated fixture assets are tested by Save/Reopen, not this cook map.
- Disposable negative/probe source files cleaned up. Assets remain under the smoke
  project's DasFrameAssetProbe. Production Apply leaves assets dirty for review/save.

Authored type use in ue_class method frames/properties, direct enum/struct value
adapters, nested/complex struct fields and type-change data migration are pending
next stages. This slice does not assert binary layout equivalence or general type
hot reload. Enum values are currently constrained to Blueprint byte-compatible
explicit values 0..254; structs are editable scalar/name values only.
