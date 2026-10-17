# Shared type descriptors and FProperty creation

`FDasDeclaredType` is the common recursive descriptor used by authoring and
runtime property creation. `EDasTypeKind` gives its existing string kinds a
typed interpretation through `GetKind()`. This avoids adding another parallel
type model or changing the source DSL/JSON schema.

`DasPropertyTypes.h` exposes:

- `DasPropertyKind`: classify an existing reflected property.
- `DescribeDasProperty`: recover its recursive descriptor.
- `ValidateDasPropertyType`: validate the complete tree before allocation.
- `CreateDasProperty`: create an unattached property tree. The caller attaches
  it with `AddCppProperty`, then binds and links its owner.

Creation requires the game thread and already loaded referenced types. Rejected
types produce a diagnostic and do not add fields to the owner. Limits of 16
levels and 1024 descriptor nodes guard malformed trees. Fixed C++ arrays and
unsupported property classes are rejected by reflection description.

## Logical type and physical storage

`Identity()` remains the logical type identity used by existing authoring.
`EnumStorage` and `StorageIdentity()` retain the physical distinction between
`FByteProperty` with an enum and byte-backed `FEnumProperty`. They are transient
reflection details, not extra authored JSON metadata. This distinction matters
when matching native container fields with `SameType`.

`StorageIdentity()` identifies the shape and referenced type paths; it is not
a complete struct layout fingerprint. Existing carrier layout invalidation
continues to check the actual struct fields, offsets and alignment.

## Integrated paths

Dynamic array/set/map carriers and delegate parameter/result properties use the
same factory. Map keys and set elements receive the required hash flags there.
Runtime marshalling classification and DSL reflected-kind checks share the
same property classifier.

Usage policies preserve context restrictions: container references currently
require native classes; nested bare containers, interface hash keys/elements,
and class-handle container elements are unsupported. Delegate signatures keep
their narrower parameter policy, including byte and localized text leaves.
Delegate/event properties themselves
still require their explicit signature contract rather than a scalar factory
case.

Static UHT carriers, Blueprint pin creation, per-family carrier caches and
layout fingerprints remain in place. A single carrier registry and shared
Blueprint pin adapter are subsequent refactoring steps.

## Validation

`CheckDasPropertyTypes` runs from the foundation suite and recursive-defaults
probe. It exercises scalar and class fields; arrays; sets; and the cross-product
of seven key storage variants and fourteen value variants. Tests compare
descriptor round trips, `SameType`, size, alignment and carrier compatibility,
including both native enum storage representations. Negative cases verify
diagnostics and that rejection leaves the owner unchanged.

Editor saved-defaults Create/Reopen/Recompile and authored-delegate
Create/Reopen checks exercise the integrated paths. Packaged defaults and
foundation probes exercise the runtime implementation outside the editor.

Validation completed 2026-10-17:

- Editor and Win64 Development builds succeeded.
- Defaults Create/Reopen/Recompile passed on the final factory implementation.
- Authored delegates Create/Reopen passed.
- Delegates PIE: 1 succeeded, 0 warnings/failures/skips.
- Packaged defaults and foundation passed; foundation reports 0 failures and
  explicitly includes `shared_type_descriptor_and_property_factory`.
- All runtime/editor probes above used NullRHI; no visual rendering or Shipping
  configuration validation is implied.

Evidence: `logs/type-factory-editor-final-build.log`,
`logs/type-factory-editor-suite.log`, `logs/type-factory-delegates-*.log`,
`logs/type-factory-delegates-pie-report/index.json`,
`logs/type-factory-receipt.log`, `logs/type-factory-package-suite.log`,
`logs/type-factory-runtime-packaged.log`,
`logs/type-factory-foundation-packaged.log`.
Packaged map: `/Game/DasComputedDefaults/5861056fa5ef465ca6fb52d577f78f83/Map`.
Archive: `Build/TypeFactoryPackaged`. No commit was created.
