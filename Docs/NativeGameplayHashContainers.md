# Generated native TMap and TSet

The native gameplay manifest uses schema 5. It records recursive `key`/`value` descriptors for maps and an `element` for sets; dependencies are collected from both keys and values. Generated accessors and method wrappers use the same checked snapshot ABI as authored containers.

The five selected Engine receiver classes still expose 337 fields and 643 methods. Their non-reflected C++ overloads and private properties remain excluded. To exercise native reflection without weakening this policy, the selection also contains `UDasNativeHashProbeComponent`, explicitly marked `fixture: true` in the manifest. It is an ABI validation component, not an additional authored class parent or a new Engine gameplay API. Its 15 fields/27 methods include inherited ActorComponent members and ten dedicated container properties/ten dedicated methods. One additional fixture struct and enum are generated.

The executable examples live in `Plugins/DasForUE/Content/Das/Scripts/native_gameplay_hash_probe.das`. `api` below is the validation component's `NativeHashProbeRef`; these field/method names belong to the fixture.

```das
var counts:table<string;int>
counts["one"]=11
api.Counts:=counts
var echoed <- api.EchoCounts(counts)
api.UpdateCounts(counts) // UPARAM(ref): preserves existing keys, adds "native"
var output:table<string;int>
api.ReadCounts(output) // out: replaces previous contents

var modes:table<EDasNativeHashKey>
insert(modes,EDasNativeHashKey.First)
api.Modes:=modes

let actor=ue_ref(type<ActorRef>,ue_self_object())
var labels:table<ActorKey;string> // reuse the imported canonical ActorKey alias
labels[ue_key(actor)]="owner"
api.ActorLabels:=labels
```

Supported keys/set elements: int, string, name, supported native enum and typed UObject keys. UObject keys use `$UEObjectKey<Reference>`/`ue_key(reference)`, not raw pointer addresses or mutable reference structs as DAS table keys. `null` keys are permitted by the existing ABI; duplicate identities collapse to one map/set entry. Iteration order is unspecified.

Map values use supported existing exact carriers for int, float, double, bool, string, name, enum, native object references, supported math and validated generated structs. The classifier requires the exact carrier `FProperty::SameType`, including native enum storage and object class constraints. Unsupported combinations stay in the export's skipped list. Float/bool/struct keys, interfaces as hash keys, nested container elements/values and byte/text/class values are excluded in this step.

Native structs may contain supported map/set members; generated source dependency ordering and schema/identity validation include both sides of each map. The validation fixture exercises a struct containing `TMap<int32,FString>` and `TSet<FString>`.

Fields return owned copies. Mutate a local table and assign the whole writable field back with `:=`. Direct nested native-field mutations remain rejected rather than silently modifying a temporary. Readonly fields expose only getters. Input containers are copied into the native frame; outputs are decoded before publication, and call-frame cleanup follows the shared method adapter. All existing context/world/mutation/shape/liveness checks remain active.

Destroyed UObject keys are rejected when a native snapshot is read. An empty whole-container replacement can clear such a field without reading its stale contents. Context weak-reference semantics and existing snapshot limits (4096 aggregate container entries, 16 MiB payload) remain unchanged. No container covariance or implicit key-class downcasts are introduced.

Validation complete: 22 generator tests; fresh export/generated-artifact comparison; strict PIE (1 success, 0 warnings/failures/skips, 27 negative native DSL fixtures); Win64 packaged Development build/cook/stage/archive, foundation, gameplay and separate saved playable-map launch all passed under NullRHI. Stable logs: `native-hash-check.log`, `native-hash-pie.log`, `native-hash-pie-report.json`, `native-hash-cooked.log`, `native-hash-cooked-runtime.log`, `native-hash-foundation.log`, `native-hash-map.log` in `logs/`. Physical rendering/input devices and Shipping were not tested. The shared PIE/cooked probe checks map/set fields, inputs, returns, out and inout; scalar/name/enum/object keys; nested structs; readonly access; duplicate/null object identity; native C++ payload readback and rejection/recovery after a key object is destroyed. Separate negative DSL fixtures cover wrong key/value/element types, wrong enum identity, incompatible object-key class, wrong out type and readonly writes.
