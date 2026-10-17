# Type support across the binding

This is a source audit of the current authored DSL, not a promise that every native
Unreal C++ type can be exposed. A loaded, registered concrete enum/struct/object
identity is required where applicable. Native API generation has its own selection
manifest and ABI adapters; a type in the runtime does not expose every native API.

| Type family | Class/struct fields | Authored methods | Authored interfaces | Single/multicast delegates | `ue_event` native subscriptions | Captured timers |
| --- | --- | --- | --- | --- | --- | --- |
| uint8, int, float, double, bool, string, FName, FText | Yes | Yes | Yes | Yes | Yes | Yes |
| Vector2D, Vector, Rotator, Transform | Yes | Yes | Yes | Yes | Yes | Yes |
| Concrete native/authored enum | Yes, byte storage | Yes | Yes | Yes | Yes, exact concrete type | Yes, exact concrete type |
| Concrete native/authored value struct | Yes, supported fields | Yes | Yes | Yes | Yes, registered supported layout | Yes, registered supported layout |
| Typed object/interface reference | Yes | Yes | Yes | Yes | Yes, typed native object/interface ABI | Yes, typed class captures |
| Class reference | Class fields; additional struct/container limits | Yes | No in authored interface DSL | No | No | No |
| array of the eight primitive types in the first row | Yes | Yes | Yes | Yes | Yes, native const-reference array | Yes, authored class callbacks |
| array of enum, struct or object reference | Yes, supported concrete elements | Yes | Yes | Yes, supported concrete elements | Yes, supported canonical carriers | Yes, authored class callbacks |
| set/map | Restricted shapes | Supported carrier shapes | Supported carrier shapes | Supported carrier shapes | Supported carrier shapes | Supported carrier shapes |
| Delegate/event value | Authored delegate fields; multicast event properties | Single delegate inputs/results; no delegate ref/out | No | No nested delegate signatures | Subscription API, not a payload type | No |
| Nested containers, fixed C++ arrays, arbitrary pointers | No | No | No | No | No | No |

"Yes" describes the implemented transport; it does not imply unrestricted
signatures. Methods and delegate frames have up to 16 parameters. Single delegates
permit return/ref/out; multicast signatures return void and permit in/ref, not
out-only. Authored interface stubs have no argument defaults. Queued callbacks
return void and accept zero through 16 by-value arguments in authored classes,
without ref/out or defaults. Multiple arguments use private generated bridges.
See [MultiArgumentCallbacks.md](MultiArgumentCallbacks.md).
Scalar timer overloads also work with ordinary exported DAS callbacks; automatic
array packing is performed for authored class callbacks.

Container keys/elements used for hashing are restricted to byte, int, string,
FName, supported enum and object keys. FText is a value, not a hash key/set element.
Containers cannot contain class references or other containers. Struct fields,
object lifetime, native class constraints and interface keys have additional
validation in the existing property and snapshot services.

## Shared rules

`DasPropertyTypes` already owns reflected type description, validation and
FProperty construction. `DasContainerTypes` selects canonical owned transports
through the shared carrier catalog/registry. These services remain in use.

`DasCallbackTypes` now owns the narrower queued-callback policy: wire name,
method ABI kind, snapshot type, timer availability, native value/const-ref proxy.
The DSL timer collector, event collector, serialized callback validator, native
event signature matcher, proxy selector and timer/event payload validation all
consult this table. Explicit UFUNCTION proxies and DAS registrations remain
necessary ABI adapters; they no longer define independent support policies.

`ValidateDasCallbackPayload` checks owning native copies, exact snapshot identity,
argument direction, supported usage, finite numeric/math values and payload limits.
Both timers and queued events enforce the same text identity/source/display budget
and array limits. Invalid timer replacement is rejected before the existing timer
is removed. Events and timers share a GC-visible owning payload for scalar objects and
references nested in supported snapshots; lifetime is checked before delivery.

The table is not an instruction to accept a new type automatically. Its owning
transport, native proxy (for events), conversion and lifetime must exist first.
The callback probe checks actual proxy signatures against the table, all eight
primitive array transports, oversized and mismatched snapshots, unknown kinds,
object ownership differences and text limits.

## Remaining groups of work

1. Keep standalone supported array/set/map callbacks and multiple arguments
   aligned with the common carrier service. These are implemented; see
   ManagedCallbackContainers.md and MultiArgumentCallbacks.md.
2. Reconcile class references and delegate-valued signatures across authored
   interfaces/delegates; keep unsupported directions explicit.
3. Expand containers only through the common descriptor/carrier services. Nested
   containers and FText hash keys are separate policy decisions.

Network support and broader structural migrations remain deferred by the user.
Further work should take a complete group through DSL, native transport and
runtime before running a combined validation pass.

Examples: `dsl_text_byte_timers.das` and `dsl_text_byte_events.das` now also cover
float/bool/FName arrays. Existing scripts retain their names to preserve imports
and saved fixture identities.


## Validation — 2026-10-17

Editor and Win64 Development game builds passed. The real gameplay PIE reported
1 success with 0 warnings, failures or skipped tests. Packaged foundation returned
PASS with 0 failures, including `shared_callback_capabilities_and_payload_validation`.
Saved gameplay fixture export, cooked gameplay and the playable map passed.
All runs used NullRHI; Shipping and physical rendering were not tested.

The new float/bool/FName event arrays are checked against real native broadcasts,
including source-copy isolation and rejection of oversized/nonfinite arrays.
Timer arrays run next-tick and repeating callbacks, clear source/callback arrays,
retain three independent repeat deliveries, and leave no pending captures after
release. The pre-existing scalar, byte/text and interface regressions also passed.

Commands: `Scripts/TestGameplayRuntimePIE.ps1 -LogPrefix callback-policy` and
`Scripts/TestCookedGameplay.ps1`. Evidence: `logs/callback-policy-report/index.json`,
`logs/callback-policy-foundation.log`, `logs/callback-policy-runtime.log`,
`logs/callback-policy-map.log`, `logs/callback-policy-export.log` and
`logs/callback-policy-game-build.log`.
Cooked map: `/Game/DasCookedGameplay/3702576965754c479f90f3884ecf1763/Map`.
The export/cook retained the existing missing ACL compression-settings asset
warnings; they did not fail the builds or runtime probes.


## Registered callback value extension

Concrete enum and registered struct callbacks now use the shared table and recursive
payload validation. Source enum/struct identities are preserved in event contracts.
Generic native receiver signatures support by-value and native const-ref parameters;
mutable native ref/out subscriptions remain unsupported. Native struct source types
must come from the canonical generated registry, as for methods. This extension does
not bypass that registry to accept arbitrary handwritten native aliases.
See [ManagedCallbackValues.md](ManagedCallbackValues.md) for scope and validation.


Enum/registered struct extension validation also passed: Editor/Game Development,
strict real PIE, saved export and packaged foundation/gameplay/playable map, with
forced GC and normal exit. Canonical native enum delegate storage and native
receiver rooting/PreExit teardown are covered. Final evidence: ManagedCallbackValues.md.


Object callback ownership and nested reference support: [ManagedCallbackObjects.md](ManagedCallbackObjects.md). Standalone supported container callbacks are covered in [ManagedCallbackContainers.md](ManagedCallbackContainers.md).
