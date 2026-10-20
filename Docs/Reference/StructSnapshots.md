# Struct values

Prefer typed native or DSL structs in properties and function signatures. Read a field with `ue_struct_get(value, Field)` or normal field syntax; mutate an owned local before assigning it back.

```das
require ue_native_gameplay
var payload = DasCallPayload(Count=10, Label="Initial")
let oldCount = ue_struct_get(payload, Count)
payload.Count = oldCount + 1
let ratio = ue_struct_get(ue_struct_get(payload, Metrics), Ratio)
```

The compiler validates the struct identity, field name and result type. Object members keep the existing boundary GC/weak-reference rules; containers are owned values. See [DSL structs](../Guide/DSL/Structs.md) and [reflected call macros](LowLevelCalls.md).

Generic immutable `UEStructValue` snapshots remain supported for integration code that has no generated type. They cannot validate field names at compile time. Their raw transport contract and legacy examples are in the [developer reference](../Dev/Runtime/StructSnapshots.md).
