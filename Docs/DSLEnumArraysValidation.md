# Authored enum array validation — 2026-10-10

Scripts/TestMethodValues.ps1 -Cooked passed fresh asset creation/reopen, JSON
roundtrip, real PIE and packaged Win64 Development execution.

Fixtures cover direct class properties, MethodGroup.States, input/result/ref/out,
owned copies and whole replacement, self/reference indexed writes and operations.
MethodState has sparse values 7 and 42 and no zero. Runtime failure groups -58..-60
check invalid source value rejection and failed method ref copyback preservation.

Compiler negatives reject AnotherMethodState with the same values, raw integers,
wrong nominal array parameters and readonly mutation/replacement. Native array
identity is checked against the actual UEnum and the sparse value table.

A direct native snapshot probe rejects freshly zero-initialized enum array elements,
accepts value 7, and rejects the reserved enum sentinel. This exercises the array
snapshot validator independently of source enum conversion.

Markers:
- PASS enum_array_nominal_integer_readonly_compile_errors
- PASS enum_array_native_sparse_identity
- PASS enum_array_invalid_zero_and_reserved_native_snapshot_refused
- DAS_METHOD_VALUES_PIE_RESULT=PASS
- DAS_METHOD_VALUES_PROBE=PASS cooked=True

Regression coverage: Actor/Component Create/Reopen, authored direct types
Create/Reopen, foundation smoke and native binding generation -Check.
Shipping/AOT, native enum arrays and structural migrations remain outside this slice.
