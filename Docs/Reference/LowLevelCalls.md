# Checked reflected calls

Use compile-time call macros over the generated bindings. Import the API package containing the desired function and its library type.

```das
require ue_library_generated
let seconds = ue_static(GameplayStatics.GetTimeSeconds)
let sum = ue_static(DasFunctionLibraryFixture.Add, 20, 22)
```

daScript does not parse chained call expressions such as `ue_static(...)(...)`; arguments belong to the macro call itself.

The library and Unreal function name are resolved during compilation; generated aliases such as `ue_lib_Time` do not appear in source. An absent function, missing binding or stale reflected signature is a compile error. Arguments and results use the selected wrapper's daScript signature. WorldContext remains automatic and requires a valid owning object/world. Construction restrictions, thread roles, weak handles and runtime signature checks remain in force.

```das
require ue_generated
let target = ue_interface(type<DasProbeInterface>, ue_self_object())
let answer = ue_call(type<DasProbeInterface>, Query, target, 5)
var counter = 10
var total = 0
ue_call(type<DasProbeInterface>, Stats, target, counter, total)
```

`ue_call` uses a typed object or interface **reference** type and receiver-first argument order. It shares the ordinary member-call implementation, including authored reference contracts, mutable `ref`/`out` arguments and owned container results. Generated argument defaults remain available. Ambiguous bindings from multiple API packages are rejected; import one canonical package for that call.

```das
require ue_native_gameplay
let payload = DasCallPayload(Count=17)
let count = ue_struct_get(payload, Count)
let ratio = ue_struct_get(ue_struct_get(payload, Metrics), Ratio)
```

`ue_struct_get` requires a typed Unreal value struct. It expands to normal checked field access without allocating a call frame or copying the whole value. Field identifiers are case-sensitive. Raw `UEStructValue` snapshots have no compile-time identity and cannot use this macro.

The underlying frame API remains available unchanged for generated code and fixtures. Its implementation contract is documented in [developer reference](../Dev/Runtime/LowLevelCalls.md).
