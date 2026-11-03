# Reflected int64

Signed `int64` maps to Unreal `FInt64Property` and Blueprint Integer64 pins.
It has its own `FDasScalarValue.Int64Value` frame slot: values never pass through
`int32`, `float`, or `double` during property access or function dispatch.

Supported authoring includes class/struct properties, input/result/ref/out
parameters, scalar native calls (`ue_arg` / `ue_result_int64`), arrays, map/set
keys and values, multicast handler values, and scalar timer payloads.
Large source literals use the daScript `l` suffix; ordinary int32 literals such as `count:int64 = 0` widen exactly. Computed scalar defaults also
preserve int64. Native indices and APIs declared as int32 retain that contract.

```das
@ue_property=(EditAnywhere, BlueprintReadWrite)
Balance : int64 = 9007199254740993l

[ue_function(BlueprintCallable)]
def Add(amount : int64) : int64 { self.Balance += amount; return self.Balance }
```

Persisted JSON defaults for int64 are **decimal strings**, for example
`"9007199254740993"`, so serialization preserves the full signed range. Fractional,
scientific notation and out-of-range values are rejected. This transport format
does not change the native property type and introduces no runtime JSON lookup.
Integer arithmetic follows daScript semantics; gameplay should check overflow
where balances accumulate. Automatic float/int64 migration is deliberately not
implied by adding the type.

Run `Scripts/TestInt64.ps1` (or the `authored-int64` suite) to exercise exact
endpoints, defaults/reimport, Blueprint frames, native `ue_arg`, ref/out,
persistent `ue_state`, container storage and atomic rejection of overflowing defaults.
