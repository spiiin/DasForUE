# Math array validation — 2026-10-10

Scripts/TestMethodValues.ps1 -Cooked passed fresh asset creation/reopen, JSON
roundtrip, real PIE and packaged Win64 Development execution.

Fixture coverage:
- Native FVector/FRotator/FTransform element identity in the generated class.
- Direct class properties and an authored struct array member (MethodGroup.Path).
- Input/result and ref/out methods for all three element types.
- Owned independent copies and whole-field replacement.
- Self/reference scalar element writes, whole transform assignment and multi-level
  vector member writes.
- Non-finite candidate rejection preserving earlier native vector values.
- Wrong array/element types and readonly mutation/replacement compile errors.

Markers: PASS math_array_wrong_type_and_readonly_compile_errors,
PASS math_array_native_element_identity. Runtime failure groups: -55..-57.

Regression coverage: Actor/Component Create/Reopen, direct authored types
Create/Reopen, foundation smoke and native binding generation -Check.
This does not establish Shipping/AOT or structural migration support.
