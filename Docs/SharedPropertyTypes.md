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

Dynamic carriers now use FDasContainerRegistry (see below). Static UHT carriers
and Blueprint pin creation remain separate; a shared Blueprint pin adapter is
a subsequent refactoring step.

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


## Shared dynamic container registry

`Runtime/Private/DasContainerRegistry.h/.cpp` replaces ten family-specific
cache maps and their separate lifetime/layout checks. Existing carrier path
encoders/resolvers remain the canonical naming boundary. All eleven dynamic
construction paths now delegate to the same `GetOrCreate` operation and
`ResetDasContainerCarriers` releases their registry ownership on shutdown.
Static UHT primitive carriers keep their native identity and lifetime.

The registry validates the descriptor before allocation, then captures its
physical storage and referenced definitions. The fingerprint records struct
size/alignment, field classes/names/offsets/sizes/flags, nested container fields,
bool storage masks, class identity and relevant class flags, enum underlying
type/member names/values and authored member IDs. Runtime object identity is
included; these fingerprints are transient, not JSON or saved metadata.
Lazy GC/replication class flags are deliberately excluded.

An entry is reused only when its definition fingerprint still matches. A changed
definition is rejected without replacing or modifying the live carrier. The
registry also rejects occupied paths, invalid descriptors and capacity overflow.
One global limit of 16384 dynamic entries replaces the per-family limits.
Fingerprint traversal is bounded to 64 levels, 16384 nodes and 1 MiB of text.

Successful carriers are rooted once by the registry. Reset removes those roots;
objects referenced elsewhere remain alive. An occupied name is not taken over
after reset. Once an unreferenced carrier is collected, it can be created again.
Calls and reset run on the game thread.

`DasContainerRegistryProbe.cpp` uses isolated registries and private fixtures to
check repeat lookup, shape conflicts, capacity, invalid requests, collision with
another UObject type, GC retention/release/recreation, struct size/field-flag
changes, nested struct drift, enum values and authored member IDs. It runs inside
the shared property probe, in both defaults and foundation suites.

Registry validation completed 2026-10-17: final Editor/Win64 Development builds,
Create/Reopen/Recompile, reopen of the pre-registry saved defaults fixture
5861056fa5ef465ca6fb52d577f78f83, gameplay PIE (1 succeeded, 0 warnings/failures/
skips), packaged defaults and foundation (0 failures) all passed under NullRHI.
The isolated registry probes run with the property matrix in Editor and packaged
foundation/defaults. A test-only abstract UObject collision fixture was corrected
to use a concrete UEnum before these final checks.

Evidence: logs/carrier-registry-editor-final-build.log,
logs/carrier-registry-editor-suite.log, logs/carrier-registry-previous-assets.log,
logs/carrier-registry-gameplay-pie-report/index.json,
logs/carrier-registry-receipt.log, logs/carrier-registry-package-suite.log,
logs/carrier-registry-runtime-packaged.log and
logs/carrier-registry-foundation-packaged.log.
Packaged map: /Game/DasComputedDefaults/6e1ee58b667349349a9626d415f6dc30/Map.
Archive: Build/CarrierRegistryPackaged. Shipping/visual rendering were not tested.
## Descriptor-based reflected container lookup

DasContainerCarrierForProperty now follows one path:

FProperty -> DescribeDasProperty -> ValidateDasPropertyType(Container)
          -> DasContainerCarrier -> exact SameType/size/alignment check.

DasContainerCarrier remains the common selector for both declared and reflected
types. It returns a native static UHT carrier or delegates dynamic creation to
the shared registry. The separate NameMapForProperty, ObjectMapForProperty,
EnumMapForProperty and ByteMapForProperty adapters and the parallel reflected
scalar/container classification list were removed. Existing path encoders,
resolvers and supported container policies remain in use.

The property probe covers 134 scalar/class/container combinations. For every
supported container it also checks descriptor selection equals native-property
selection, repeated lookup identity, saved-path resolution, size and alignment.
Negative native lookups cover null/scalar inputs, fixed arrays, nested arrays,
FText map keys, float sets and enum storage wider than a byte. All use isolated
unattached property fixtures; no engine or user fields are changed.

Descriptor lookup validation completed 2026-10-17: Editor and Win64 Development
builds, defaults Create/Reopen/Recompile, reopen of the preceding registry fixture
6e1ee58b667349349a9626d415f6dc30, gameplay PIE (1 succeeded, 0 warnings/failures/
skips), packaged defaults and foundation (0 failures) all passed under NullRHI.
The expanded property matrix and negative native lookup checks ran in both
Editor and packaged probes.

Evidence: logs/descriptor-lookup-editor-build.log,
logs/descriptor-lookup-editor-suite.log,
logs/descriptor-lookup-previous-assets.log,
logs/descriptor-lookup-gameplay-pie-report/index.json,
logs/descriptor-lookup-receipt.log, logs/descriptor-lookup-package-suite.log,
logs/descriptor-lookup-runtime-packaged.log,
logs/descriptor-lookup-foundation-packaged.log.
Map: /Game/DasComputedDefaults/5ad77205ee954a4f83f63b286c0aede0/Map.
Archive: Build/DescriptorLookupPackaged. No Shipping/visual rendering validation.

## Static UHT carrier catalog

The 28 static UHT carrier types are now registered once in StaticCarriers in
DasContainerTypes.cpp. Each entry derives its physical descriptor from the native
Values property through DescribeDasProperty and retains the native struct,
property, path and StorageIdentity. Descriptor lookup and exact saved-path
lookup use this same catalog; reflected-property lookup already passes through
the descriptor selector. Dynamic path resolvers and the dynamic registry remain
separate from the native UHT lifetime.

DasContainerCarrier validates a complete incoming descriptor before matching a
catalog entry. Malformed primitive identities or unexpected child descriptors
cannot select a static carrier merely by matching a kind string. Descriptor
extraction executes independently of check/assert macros.

The property matrix additionally asserts the existing primitive UHT naming
contract and native C++ struct operations. This detects replacement of an
existing static carrier by an equivalent dynamic tree, even when SameType,
size and alignment happen to agree. All 28 static shapes are covered by the
existing array/set/map matrix. Invalid primitive paths and extra children are
checked directly at the descriptor selector.

Static catalog validation completed 2026-10-17: Editor and Win64 Development
builds, defaults Create/Reopen/Recompile, reopen of the preceding saved fixture
5ad77205ee954a4f83f63b286c0aede0, packaged defaults and foundation (0 failures)
passed. The matrix covers all 28 static UHT shapes and their naming contract.
Gameplay PIE passed with 1 success and 0 warnings/failures/skips after correcting
the test-holder lifetime: the native library fixture must survive until the
late timer-delivery check; its deletion now follows that check. Timer release
assertions were retained. The initial failed PIE report is preserved separately.
All runs used NullRHI; Shipping/visual rendering were not tested.

Evidence: logs/static-catalog-editor-build.log,
logs/static-catalog-editor-final-build.log, logs/static-catalog-editor-suite.log,
logs/static-catalog-previous-assets.log,
logs/static-catalog-gameplay-pie-report/index.json,
logs/static-catalog-gameplay-pie-initial-report.json,
logs/static-catalog-receipt.log, logs/static-catalog-package-suite.log,
logs/static-catalog-runtime-packaged.log,
logs/static-catalog-foundation-packaged.log.
Map: /Game/DasComputedDefaults/855762fa32dd43238cb7f2fc630eeb00/Map.
Archive: Build/StaticCatalogPackaged.

## Shared Blueprint pin adapter

DasEditor/Private/DasTypePins.h/.cpp provides DasTypeToPin and DescribeDasPin.
Forward conversion resolves optional authoring-only identities, creates an
unattached FProperty tree through CreateDasProperty, and delegates pin categories,
container terminals, interfaces and precision to UEdGraphSchema_K2. Reverse
conversion preflights concrete value pins, uses Kismet's property construction
and describes that property through the shared runtime descriptor. Failure leaves
the caller's output unchanged. Neither operation mutates a Blueprint or saves
an asset. The bounded resolver copies the descriptor tree rather than modifying
shared authoring nodes.

DasSchemaType bridges existing EDasMethodSignature values, math signatures and
carrier paths into the common descriptor. Blueprint class fields/method graphs,
parameter/result matching, authored interfaces, authored enum/struct member pins,
struct scalar migration and class numeric graph migration use the shared adapter.
TypeService supplies its model/folder resolver for /DasTypes identities. It keeps
asset creation order and metadata/default policies in the type importer.

Reference/const parameter directions and authored stable identities stay in the
surrounding signature/model. A pin represents a logical enum type; Kismet decides
whether its compiled property uses byte-enum or FEnumProperty storage. Therefore
pin roundtrips compare logical identity, not physical enum storage identity.
Wildcard/exec, unresolved self, soft/weak references and delegate/event signature
pins are outside the value adapter. Serializable delegate signature-holder assets
remain owned by the existing separate service. Archive restoration and specialized
graph nodes still retain their context-specific pin/default construction.
No DSL syntax, saved IR/JSON format or supported authoring policy was changed.

DasForUE.Types.PinAdapter compiles a real temporary Blueprint with 134 scalar,
class, interface, enum, struct and container fields. It checks forward/reverse
logical type, compiled field/pin agreement, carrier-schema bridges, parameter
modifier separation, nested symbolic resolution without input mutation, and
atomic failure for malformed types/pins. Run Scripts/TestTypePins.ps1 to repeat
this focused test.

Pin-adapter validation completed 2026-10-17:

- Editor build and the focused real-Blueprint matrix passed (134 combinations;
  automation: 1 succeeded, 0 warnings/failures/skips).
- Defaults Create/Reopen/Recompile passed.
- Authored enum/struct types Create/Reopen and struct numeric migration
  Create/Reopen passed, including existing rollback assertions.
- Gameplay RuntimePIE and DSL NumericWidenPIE each passed with 1 success and
  0 warnings/failures/skips after rebuilding the concurrently updated runtime
  to match its event fixture scripts.
- Win64 Development cook/stage/archive, packaged defaults and foundation passed;
  foundation reports 0 failures. All runs used NullRHI; Shipping/rendering were
  not tested.

Evidence: logs/type-pins-editor-build.log, logs/type-pins-editor-final-build.log,
logs/type-pins-matrix-report/index.json, logs/type-pins-defaults-suite.log,
logs/type-pins-regressions-suite.log, logs/type-pins-types-*.log,
logs/type-pins-struct-numeric-*.log,
logs/type-pins-gameplay-pie-report/index.json,
logs/type-pins-numeric-widen-pie-report/index.json,
logs/type-pins-package-suite.log and logs/type-pins-foundation-packaged.log.
Map: /Game/DasComputedDefaults/cad7e58e01b14eb3a085e6b519009e38/Map.
Archive: Build/TypePinsPackaged. No commit was created.

## Archive restoration and Blueprint defaults (2026-10-17)

Removed the independent ArchivedPin/ArchivedDefault conversion tables. Archive
restoration precomputes fallible pins through DasSchemaType/DasTypeToPin before
mutation, and shares DasBlueprintFieldDefault with ordinary Blueprint import.
This also preserves the specialized Transform pin grammar and precise numeric
struct literals. Container defaults export Values, not their transport wrapper;
the parsed owned values stay in FDasDefaultRoots through compilation/reinstance.

Rename/archive snapshots now copy native containers through the common carrier
registry and use the shared object/interface reference accessors. The expanded
saved-asset archive probe covers byte boundaries, localized text identity, int
arrays/sets and string-to-int maps across parent/child CDOs, actor overrides and
actors spawned while fields are retired. Existing negative, GUID, rollback and
repeat/idempotence checks remain. Blank lines separate test-only comment markers
from declarations so the tooltip feature does not treat markers as documentation.

Validation: see DSLFieldArchiveValidation.md and logs/archive-pins-suite.log.
This step changes editor migration/import; packaged builds were not repeated.
