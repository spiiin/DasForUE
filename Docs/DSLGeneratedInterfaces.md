# Generated native interface references and calls

The selected native interfaces now participate in the regular reflection exporter
and deterministic binding generator. Selection.json chooses functions and public
aliases; interface dispatch is inferred from the reflected receiver's class kind.
Existing schema-v5 class records gain an optional boolean interface marker.
Ordinary class records and older IR without that marker retain their behavior.

```das
require ue_foundation
require ue_generated

let api = DasProbeInterface_from(target)
let answer = ue_probe_Query(api, 5)
let label = ue_probe_Label(api)
let changed = ue_probe_Stats(api, 10)
// changed.out_counter is the updated inout value.
// changed.out_total is the output-only value.
```

target is an existing UObjectHandle. A native object wrapper can supply its handle;
no native interface pointer or assumed UObject memory layout is exposed. The
generated reference is a nominal daScript struct wrapping the existing context-owned
weak capability. A reference of another interface type does not type-check as the
receiver. Constructor and raw conversion check native membership. Manually forging
the struct does not bypass membership validation at dispatch.

Unambiguous interface names receive a typedef and friendly helpers:
DasProbeInterface_from/raw/null/valid. Ambiguous names omit these helpers and keep
canonical generated names. Helpers also avoid collisions with selected functions,
types and generated result symbols. Interface references do not receive native
UObject inheritance casts. Object wrappers and their existing conversions remain.
ue_native_interface(path=...) is generated identity metadata, not a new user-authored
interface definition. ue_type can resolve imported native interface wrapper aliases.

_from permits null; _valid returns false for null/expired/nonimplementers. Conversion
of a nonnull expired or incompatible handle rejects. Dispatch on null rejects.
Membership and compatibility are rechecked using the existing interface-call frame.
Generated wrappers compare the exported signature to the declared interface method,
not to incidental Blueprint implementation flags. Invocation independently validates
the actual method and interface declaration, preserving reload/signature guards.

The regular generator handles scalar/enum/name/object/struct values, supported arrays,
maps/sets and multiple input/output parameters under existing marshaller capability
limits. Arrays and containers return owned script values. Several outputs use the
existing alias_Result struct with out_<parameter> fields; inout values are passed as
inputs and returned in that result. This API does not mutate caller variables through
references. Void functions such as ue_event_Notify and ue_event_ResetCounter are normal
generated calls, with real Blueprint event execution.

Calls still require the active game-thread context and a reflected implementation on
the receiver. Native Execute_* fallback and VM reentry are unchanged; dispatch into a
daScript Actor from an already active VM callback explicitly rejects. These references
are currently local/script values, not reflected interface UPROPERTY/signature types,
TScriptInterface storage, authored BPI wrapper generation or automatic asset discovery.
The next integration stages are broader authored interface signatures and scoped
wrapper generation for authored definitions; general migrations remain deferred.

Selected example APIs: DasProbeInterface (Query/Label/Weights/Stats),
DasCollectionInterface (EchoMap/EchoSet), DasEventInterface (Notify/ResetCounter).
Executable example: Content/Das/Scripts/generated_interface_calls.das.
Validation: GenerateBindings.ps1, TestBindingsGenerator.py, TestEditor.ps1,
TestBindingsPolicy.py and TestDSLTypeRegistry.ps1 -Interfaces -Cooked.


Selected methods now also have normal dot-call facades with mutable ref/out
parameters. See DSLInterfaceMethodCalls.md for call-side behavior and limits.
