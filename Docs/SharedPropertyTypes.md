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


## Shared native property values (2026-10-17)

ReadDasPropertyValue / WriteDasPropertyValue in DasPropertyValues.h operate on a
property's value memory, using the common descriptor and existing FDasScalarValue
transport. Reads construct an owned candidate and publish only on success. Writes
check kind, enum identity/member, object/interface/class constraints and snapshot
identity before modifying native storage. Both byte-enum and enum-class storage
use the shared enum helpers. Native math remains an owned struct snapshot in this
API; existing specialized method-frame math adapters are unchanged. Classes use
ClassValue plus ObjectClass, matching the native method-frame transport.

Structs and containers copy native storage through FInstancedStruct and the shared
carrier registry. Supported layouts, snapshot budgets and numeric math validity
retain the existing validation policies. Arbitrary opaque structs, delegates,
fixed arrays and unsupported native types remain rejected. This API does not own
VM handles, bind delegates or perform numeric coercions.

Stable-ID rename, archive capture/restore, numeric migration capture and structural
rollback now call this service, removing their separate value-kind switch tables.
Capture errors are returned before Blueprint mutation rather than reaching checked
casts in editor migration code. Existing migration-policy restrictions remain.

FDasPropertyValueRoots replaces the scalar-only editor object pin helper. Its
FGCObject scope retains complete owned snapshots, including nested references and
snapshot type objects. Reinstance replacement walks supported native value trees
and rebuilds set/map hashes after replacing object keys. The same scope is used
for forward migration and rollback; it releases references at scope end.

The focused DasForUE.Types.PropertyValues automation covers 106 native scalar,
class, enum storage, struct and container combinations. It verifies independent
owned copies, reusable snapshots, atomic kind/identity rejection and unchanged
read outputs on failure. Separate forced-GC assertions cover references in nested
arrays/maps of structs, release after replacement and remapped object set/map key
lookups. Run Scripts/TestPropertyValues.ps1. Test marker:
DAS_PROPERTY_VALUES_RESULT=PASS combinations=106.

Evidence and migration regression results: logs/property-values-*. The existing
reflection call marshalling and self accessors are the next consumers; this step
does not switch their execution paths.


Property value validation results (2026-10-17):

- Editor and Win64 Development Game builds passed. The runtime replacement API
  uses TMap<UObject*,UObject*> directly; Unreal's convenience replacement-map
  typedef is editor-only and is not part of this public runtime contract.
- Final PropertyValues automation: 106 combinations, exactly 1 success and
  0 warnings/failures/skips, including the canonical ClassValue transport check.
- ArchivePIE, RenameScopePIE and NumericConvertPIE: each exactly 1 success and
  0 warnings/failures/skips.
- ArchiveFields Create/Remove/ReopenRemoved/Restore/ReopenRestored passed.
- RenameScope Create/Migrate/Reopen passed.
- NumericConvert Create/Migrate/Reopen/NarrowFloat/ReopenFloat passed.
- StructuralRollback Create/Rollback/Reopen passed, including existing injected
  failure and prior state restoration assertions.

Evidence: logs/property-values-editor-build.log,
logs/property-values-editor-final-build.log, logs/property-values-game-build.log,
logs/property-values-matrix-suite.log,
logs/property-values-PropertyValues-report/index.json,
logs/property-values-migrations-suite.log and the corresponding
logs/property-values-{ArchivePIE,RenameScopePIE,NumericConvertPIE}-report/index.json.
Runs used NullRHI. This step compiled the Game target but did not repeat
cook/stage/packaged execution or Shipping validation. No commit was created.


## Reflected calls use shared property values (2026-10-17)

Reflected invocation now delegates native parameter/result conversion to
ReadDasPropertyValue / WriteDasPropertyValue. The low-level ue_arg_* / ue_result_*
adapters use one scalar-slot association list and the shared native codec. The
short ue_call_* adapters and automatic static-library WorldContext use the same
service. Owned struct/container parameters and results, typed math and typed enum
method adapters also use it, so native storage copying is no longer repeated in
the call adapter layer.

Invocation policy remains in the call frame: signature/direction checks, assigned
input tracking, WholeInputs, recursion/failure propagation, target expiry,
WorldContext validation and per-path object pins. Text/name handles still belong
to the VM context. Delegate signature/lifetime adapters remain separate. Hash-key
finite checks and signed-zero normalization remain in the container-key policy.

Native symbolic enum calls deliberately have a broader policy than authored enum
values: reflected storage can exceed the byte-sized authored range, and low-level
native calls do not depend on editor-only Hidden metadata. These cases retain the
native symbol/storage adapter; supported authored enum values use the shared
codec. Return/out/ref class values retain obsolete/MetaClass guards in addition
to the shared native codec's type checks.

Added real-UFunction script regressions for failed scalar replacement preserving
a previously assigned argument, byte/enum separation followed by a valid result
read, and native hidden-symbol behavior. They run with the existing foundation
suite in Editor and packaged execution (the metadata-specific case is editor-only).


Reflected invocation validation results (2026-10-17):

- Final Editor and Win64 Development Game targets passed UBT verification. The
  binaries used in the final runs are newer than the runtime source changes.
- Foundation passed in Editor and packaged execution with failures=0. New
  failure-recovery and byte/enum regressions passed in both; the native Hidden
  metadata regression passed in Editor.
- PropertyValues matrix (106 combinations) and Gameplay.RuntimePIE each report
  exactly 1 success, 0 warnings/failures/skips. Gameplay includes libraries,
  WorldContext, complex inputs/results, out/ref, object ownership and GC probes.
- Static library Create/Reopen and cooked gameplay fixture export passed.
- Win64 Development cook/stage/archive passed. Packaged foundation, gameplay and
  playable-map runs passed with normal exit. Cook reports 0 errors and the 4
  pre-existing ACL compression-default asset warnings. All runs used NullRHI;
  Shipping and visual rendering were not tested.

Evidence: logs/reflected-values-editor-final-build.log,
logs/reflected-values-game-build.log, logs/reflected-values-editor-suite.log,
logs/reflected-values-foundation-editor.log,
logs/reflected-values-{PropertyValues,RuntimePIE}-report/index.json,
logs/reflected-values-static-{Create,Reopen}.log,
logs/reflected-values-gameplay-export.log, logs/reflected-values-package.log,
logs/reflected-values-package-suite.log and
logs/reflected-values-{foundation,gameplay,playable}-packaged.log.
Map: /Game/DasCookedGameplay/397e5659ef0e46058711c3a77fe7febd/Map.
Archive: Build/ReflectedValuesPackaged. No commit was created.

Self and generated field access now use the same property-value service:

- `DasNativePropertyValues.inl` owns the scalar transport slot table shared by
  field adapters and reflected calls. Native reads/writes delegate to
  `ReadDasPropertyValue` / `WriteDasPropertyValue`.
- Self, authored references and generated native references use it for scalar,
  byte/text, enum, object/interface and class fields. Whole structures (including
  math snapshots) and array/map/set fields use the same owned snapshot codec.
- Receiver identity, generated allowlist/signature, visibility/read-only checks,
  context capabilities and the bind-events write prohibition remain at the
  access boundary. Native movement numeric constraints and class obsolescence
  checks remain in their existing adapters.
- Immutable snapshot leaf/container operations remain a separate script value
  API; this change unifies transfers between those values and live UE fields.

`DasSelfValueTests.inl` checks that rejected scalar writes, wrong snapshot reads
and read-only writes leave native fields intact and permit subsequent valid
access in the same script invocation. Existing native defaults probes cover
native enum/struct/container operations, detached snapshots and error recovery.

`DasResolvedEnumStorage` also provides the shared `Automatic` enum-storage rule
for the FProperty factory and container carriers. Native enum classes resolve to
`FEnumProperty`; byte/unscoped and authored enums resolve to `FByteProperty`.
An explicit Byte/Enum8 choice is preserved. This prevents delegate parameters
and their owned enum-container snapshots from acquiring different identities.

Self field validation results (2026-10-17):

- Editor and Win64 Development Game builds passed. Foundation passed in Editor
  and packaged execution (`failures=0`), including the new self failure/recovery
  test and the existing visibility, type, read-only and bind-events checks.
- PropertyValues reports exactly 1 success, 0 warnings/failures/skips (106 type
  combinations). Saved static-library Create/Reopen and gameplay export passed.
- Native defaults Create/Reopen/Recompile passed, including native enum/struct
  arrays, map/set storage, detached snapshots, object references and failed-write
  atomicity. The same saved native-defaults map passed in packaged execution.
- Cook/stage/archive passed with 0 errors and 4 existing ACL asset warnings.
  Packaged playable-map execution passed. Runs used NullRHI, Win64 Development;
  Shipping and visual rendering were not tested.
- The full Gameplay.RuntimePIE and packaged gameplay suites did **not** pass:
  dynamic callback receiver duplication loses `CPF_ZeroConstructor` on the
  `DasArrayIntSignal` array parameter. The resulting strict signature comparison
  fails despite matching 16-byte frames (flags `10000008000182` versus
  `10000008000382`); callbacks do not finish before the timeout. This remains a
  separate callback-receiver issue, not a successful gameplay validation.

Evidence: logs/self-values-editor-{build,final-build,storage-build}.log,
logs/self-values-game-{initial-build,build}.log,
logs/self-values-foundation-editor.log,
logs/self-values-PropertyValues-report/index.json,
logs/self-values-RuntimePIE-report/index.json,
logs/self-values-static-{Create,Reopen}.log,
logs/self-values-native-{Create,Reopen,Recompile}.log,
logs/self-values-package.log and
logs/self-values-{native,foundation,gameplay,playable}-packaged.log.
Native map: /Game/DasNativeDefaults/8a34a72e8537486fa2f8bf577125f7ab/Map.
Gameplay map: /Game/DasCookedGameplay/1a918bc8bd6743f19c234b074d139f8c/Map.
Archive: Build/SelfValuesPackaged. No commit was created.

Callback receiver follow-up:

`RestoreReceiverTraits` restores `CPF_ComputedFlags` from the source signature
after duplication/linking, recursively for array elements, set elements and map
keys/values. This retains native UHT traits such as array `ZeroConstructor` while
keeping `IsSignatureCompatibleWith(Signature, 0)` strict for both receiver
creation and cache reuse. Mutable ref/out rejection is unchanged.

`CheckDasCallbackTypes` now exercises ten actual UHT array/struct event signatures
as well as the generated array/map/set signatures. It checks exact signature
compatibility, receiver reuse, native invocation and rejection of a mutable
ref/out variant. These regressions are part of Foundation in Editor and packaged
execution; full gameplay tests exercise callback delivery and cleanup.

The shared value reader also records the concrete struct/carrier path in
`FDasScalarValue::ObjectClass`. Dynamic delivery checks this identity before
dispatching, so a valid owned snapshot without the path would be dropped. The
PropertyValues matrix now checks this invariant for every struct/container read.

Callback follow-up validation (2026-10-17) resolves the earlier gameplay failures:

- Editor and Win64 Development Game builds passed. Foundation passed in both
  Editor and packaged execution (`failures=0`), including real native UHT
  signatures, strict property flags, payload identity, reuse and ref/out rejection.
- PropertyValues (106 combinations) and Gameplay.RuntimePIE each report exactly
  1 success, 0 warnings/failures/skips. Gameplay verifies actual callback delivery,
  snapshot isolation, timers, object ownership and cleanup at EndPIE.
- Fresh gameplay export and Win64 Development cook/stage/archive passed.
  Packaged Foundation, gameplay and playable-map probes passed with normal exit.
  Cook retains the 4 known ACL asset warnings and has 0 errors.
- Tests used NullRHI. The final PIE run disabled HomeScreen using the same
  console-variable override as TestGameplayRuntimePIE.ps1, avoiding an unrelated
  external HTTP connectivity timeout. Editor commandlets/cooker used Multiprocess
  to skip redundant SDK validation while another build occupied UBT. Shipping
  and visual rendering were not tested.

Evidence: logs/callback-receiver-{editor-build,identity-editor-build,game-build}.log,
logs/callback-receiver-foundation-editor.log,
logs/callback-receiver-{PropertyValues,RuntimePIE}-report/index.json,
logs/callback-receiver-remaining-suite.log,
logs/callback-receiver-gameplay-export.log,
logs/callback-receiver-package.log, logs/callback-receiver-package-suite.log,
logs/callback-receiver-{foundation,gameplay,playable}-packaged.log.
Map: /Game/DasCookedGameplay/4ac9474c24fd40fcb9607c0451bd16cf/Map.
Archive: Build/CallbackReceiverPackaged. No commit was created.

Snapshot field/container adapter consolidation:

- `ReadPropertyScalarValue`, `WritePropertyScalarValue` and `PropertyScalarValue`
  reuse the existing scalar slot table for both owner fields and resolved value
  memory. Snapshot scalar leaves, map values and key/element reads no longer
  implement their own native scalar decoding. This includes byte and FText.
- Nested struct/container extraction and replacement use `ReadPropertySnapshot`
  and `WritePropertySnapshot`, as do struct-valued map entries. The codec owns
  type identity, validation and copying; no second carrier implementation is
  embedded in the nested-value adapters.
- Scalar and struct map updates use one shared write path. Enum/reference map
  values and enum/object leaves also use shared native value adapters. Operations
  still build a detached snapshot and publish it only after validation succeeds.
- Enum number transport is shared with reflected calls. Symbol-based native
  access retains its existing wide-storage and Hidden-symbol behavior; typed
  authored enum tokens retain their byte/member validation. Enum array resize
  keeps its separate existing initial-member selection policy.
- Path traversal, index checks, map/set topology, ordering, UE equality collision
  checks, object-key identity and container limits remain explicit operations.

`DasSnapshotValueTests.inl` exercises leaf/nested copy isolation, failed scalar
and nested-type assignment, failed insertion of a mismatched struct map value,
recovery with a valid insertion, array bounds, byte values, FText localization
identity and the native-versus-typed enum policy. Foundation runs these adapters
in both Editor and packaged execution (Hidden metadata is editor-only).

Snapshot adapter validation (2026-10-17):

- Editor and Win64 Development Game builds passed. Foundation passed in Editor
  and packaged execution with failures=0; all new snapshot regressions passed.
- PropertyValues (106 combinations) and Gameplay.RuntimePIE each report exactly
  1 success, 0 warnings/failures/skips. Gameplay export passed.
- Native defaults Create/Reopen/Recompile passed, exercising native enum/struct
  arrays, map/set value types, name/enum/object keys, detached snapshots and
  failed-operation recovery. The native-defaults packaged probe passed too.
- Cook/stage/archive and packaged Foundation, gameplay and playable-map probes
  passed with normal exit. Runs used NullRHI and Win64 Development; Shipping and
  visual rendering were not tested. Known ACL asset warnings remain in cook.
- Validation used the binaries built for this snapshot change. Concurrent
  callback-frame extensions were subsequently added outside the tested snapshot
  adapter regions; those extensions are not covered by these build results.
  The snapshot regions, shared native helpers, byte/text adapters and new
  regressions were checked to remain unchanged during validation.

Evidence: logs/snapshot-values-editor-{build,final-build}.log,
logs/snapshot-values-game-build.log, logs/snapshot-values-editor-suite.log,
logs/snapshot-values-foundation-editor.log,
logs/snapshot-values-{PropertyValues,RuntimePIE}-report/index.json,
logs/snapshot-values-native-{Create,Reopen,Recompile}.log,
logs/snapshot-values-package.log, logs/snapshot-values-package-suite.log,
logs/snapshot-values-{native,foundation,gameplay,playable}-packaged.log.
Gameplay map: /Game/DasCookedGameplay/8967b10ca703461ea148dd3bea685469/Map.
Native map: /Game/DasNativeDefaults/16ea797ed32d46969228e637bc0b444d/Map.
Archive: Build/SnapshotValuesPackaged. No commit was created.

Reflected value traversal (2026-10-17):

`DasValueTraversal.h` owns the recursion through struct fields, arrays, sparse
sets and sparse map keys/values. It preserves const memory for validation and
uses mutable memory for reference replacement. Policies receive each value's
role (field, array element, set element, map key or map value), container counts
and storage bytes, and a post-container hook.

`ValidateDasStructSnapshot` now supplies value rules rather than its own loops:
the aggregate 4096-element / 16 MiB budget, math validity, enum membership,
duplicate identities and stricter liveness rules for object hash keys remain.
Type-graph validation still runs first and rejects recursive/opaque layouts.
`FDasPropertyValueRoots::Remap` uses the same topology; its post-container hook
rehashes maps and sets after replacing their references.

GC deliberately remains with Unreal's `AddPropertyReferencesWithStructARO` and
`FInstancedStruct::AddStructReferencedObjects`. A reflected leaf walk cannot
replace native ARO callbacks, struct metadata rooting or incremental GC barriers.
GC is not gated by snapshot validation or its budgets. No new hand-written GC
walker was added.

PropertyValues regressions cover the aggregate nested budget boundary, invalid
math inside a map/struct/array, duplicate object hash identities, real sparse
holes in owned snapshots, remapped-key lookup and original snapshot isolation.
Existing tests force GC over nested arrays/maps, replace references and confirm
the replaced objects are released.

Traversal validation:

- Editor and Win64 Development Game builds passed. PropertyValues reports one
  success, zero warnings/failures/skips (106 type combinations plus regressions).
  Editor Foundation passed with failures=0.
- Native defaults Create/Reopen/Recompile and gameplay export passed.
- Cook/stage/archive passed (zero errors, four known ACL asset warnings).
  Packaged native defaults, Foundation, gameplay and playable-map probes all
  passed and exited normally. These runs use NullRHI / Win64 Development;
  Shipping and visual rendering were not tested.
- At this traversal-validation checkpoint, Editor RuntimePIE was **not green**.
  This historical result is superseded by the callback follow-up below.
  The first run stopped on a reserved
  `label` identifier in the concurrently added callback fixture, subsequently
  fixed by parallel work. The next run reported container-callback / native
  damage checks and timer cleanup failures. A final run after export/package
  reached `callbacks_finish_before_timeout`; the packaged versions of the
  container and native damage checks passed. This stage does not claim those
  Editor callback failures are fixed. A separate existing isolated checkout
  without the new walker also reported the callback timeout; that is supporting
  context, not an exact controlled comparison of identical sources.
- The traversal header, validator, property-value codec/remapper and
  PropertyValues tests retained identical hashes throughout final validation.
  Other callback files in the shared checkout continued changing independently.

Evidence: logs/value-walker-editor-{final,regression}-build.log,
logs/value-walker-game-build.log, logs/value-walker-foundation-editor.log,
logs/value-walker-PropertyValues-report/index.json,
logs/value-walker-native-{Create,Reopen,Recompile}.log,
logs/value-walker-package{,-suite}.log,
logs/value-walker-{native,foundation,gameplay,playable}-packaged.log and
logs/value-walker-RuntimePIE-final-report/index.json (failed callback timeout).
Gameplay map: /Game/DasCookedGameplay/19ee5396db8448b8b47149b83fbfb8e4/Map.
Native map: /Game/DasNativeDefaults/10e45d1a77cb4220981b47c2b6d8b1d2/Map.
Archive: Build/ValueWalkerPackaged. No commit was created.

Callback timeout follow-up (2026-10-17):

The main editor runtime DLL used by the failed runs predated the callback fixes
already present in source. Rebuilding the main checkout included the strict
`IdenticalDasStructSnapshot` comparison in `StoreStructValue`: Unreal's editor
property equality can treat FText values with identical display strings as equal
despite different localization namespace/key/history. Reusing such a snapshot
changes callback arguments. The existing array/map FText regressions cover this;
the Foundation probe now records the concrete collision:
`DAS_SNAPSHOT_IDENTITY_REGRESSION legacy_equal=1 cache_equal=0`.

The old timeout log printed only the scalar fixture even though readiness also
depends on the container fixture. It cannot establish exactly which readiness
predicate failed in that historical run. `DasTextByteTimersReady` now logs both
actors, their handler counters, Ready flags, active timer counts and host errors.
The PIE timeout no longer appends a blank automation error when LastDasError is
empty. No timeout, count assertion or readiness predicate was relaxed.

After rebuilding, all three fresh-process RuntimePIE runs passed, each reporting
one success and zero warnings/failures/skips. The two final runs used the final
diagnostics and both include the math/name/map/set FText-history, rejected timer
replacement, callback cleanup and EndPIE cleanup checks. Foundation passed with
failures=0. This follow-up did not introduce a second FText fix; it rebuilt and
verified the existing fix in the main checkout and improved failure diagnostics.

Evidence: logs/callback-timeout-{build,final-build}.log,
logs/callback-timeout-foundation.log,
logs/callback-timeout-diagnostic-report/index.json and
logs/callback-timeout-final-{1,2}-report/index.json.
Checks used Unreal 5.8.3, NullRHI and Development Editor. No commit was created.

Snapshot ownership and copy reduction (2026-10-17):

`StoreStructValue` now distinguishes borrowed snapshots from owned temporaries.
Both routes validate and deduplicate with the same FText-aware comparison and
share one token allocator. An owned candidate moves its native buffer into the
context table on insertion; a borrowed value is copied on insertion. Snapshot
modifiers still clone their source before changing it, then move the candidate.
Property reads and reflected function results also transfer their temporary
snapshot into the table. Context entries remain immutable through the VM API.

`WriteDasPropertySnapshot` synchronously borrows an immutable snapshot and copies
directly into the destination FProperty memory. It shares the existing codec's
type/value checks and write implementation; validation finishes before writing.
The borrowed reference does not escape. Reflected struct/container arguments and
native snapshot property setters no longer build an intermediate FDasScalarValue
with a second owned copy. Call assignment bookkeeping and object pins remain.

Callback payloads and their common GC roots accept an owning rvalue. Dynamic
event receivers transfer completed local captures into the queue; timer wrappers
transfer their validated local captures into the timer. The const-reference
event API still captures an independent copy. Repeated timers retain their
capture, and delivery still copies borrowed values into the context when no
matching immutable entry exists. Native GC and weak identity checks are intact.

Representative deep-copy counts for a new, non-deduplicated snapshot:

| Route | Before | After |
| --- | ---: | ---: |
| Native function result/property read to VM table | 2 | 1 |
| VM snapshot to reflected argument/property memory | 2 | 1 |
| VM snapshot to a timer capture (before delivery) | 2 | 1 |
| Single native snapshot event through capture to VM table | 3 | 2 |
| Clone/modify snapshot and insert changed value | 2 | 1 |

These counts describe the ownership paths, not measured execution time. Native
destination copies and copies that isolate mutable sources/deliveries remain.
Multi-argument frame assembly still uses the checked per-field value codec.

Regressions verify exact buffer transfer into an owning callback payload, moved
capture GC retention/release, independent capture from a borrowed mutable source,
direct borrowed writes and failed-write atomicity across supported snapshot
types. Existing script tests cover unchanged originals, repeated delivery,
event-handler mutation, FText localization identity and cancelled/released owners.

Ownership validation:

- Editor and Win64 Development Game builds passed. Final rebuilds explicitly
  recompiled DasRuntime.cpp after the last event-dispatch gate/forwarding change.
- Editor Foundation passed before and after that final rebuild (failures=0).
  Callback regressions verify moved-buffer identity, borrowed-source isolation,
  nested GC retention/release and FText-aware snapshot identity.
- PropertyValues passed all 106 combinations plus borrowed-write regressions:
  one success, zero warnings/failures/skips. Native defaults Create, Reopen and
  Recompile passed, as did fresh gameplay export.
- Initial and final RuntimePIE runs each report one success, zero warnings,
  failures or skips. The final run includes the final dispatch implementation.
- Final cook/stage/archive succeeded (zero errors, four known ACL asset
  warnings). All four final packaged probes passed and exited normally: native
  defaults, Foundation (failures=0), gameplay and playable map.
- Scoped runtime/header/adapter/probe files and PropertyValues tests retained
  their recorded hashes throughout the final rebuild-and-test pass. No commit
  was created. Validation uses NullRHI / Development; Shipping and visual
  rendering were not tested.

Evidence: logs/snapshot-moves-validation.json,
logs/snapshot-moves-editor-{build,final-build}.log,
logs/snapshot-moves-game-{build,final-build}.log,
logs/snapshot-moves-foundation-{editor,final-editor}.log,
logs/snapshot-moves-PropertyValues-report/index.json,
logs/snapshot-moves-final-PIE-report/index.json,
logs/snapshot-moves-native-{Create,Reopen,Recompile}.log,
logs/snapshot-moves-package{,-final-suite}.log and
logs/snapshot-moves-{native,foundation,gameplay,playable}-packaged.log.
Archive: Build/SnapshotMovesPackaged.


## 2026-10-17 — combined commit validation and nullable getter fixes

The combined b7da031 baseline plus nullable VM object getter and method-dispatch
fixture fixes passed all 35 Unreal Automation tests without warnings/failures/
skips, 116 Python checks, Editor/Game Development, Foundation, saved fixture
phases, cook/package and all five packaged map probes. Queued callback validation
remains strict. The runner prepares the cross-process ClassReload/MethodValues
dependencies explicitly. See CombinedValidation.md and logs/combined-validation.json
for the source manifest, initial failures, fixes and validation scope. No commit.
