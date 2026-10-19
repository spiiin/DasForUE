# Native reference array validation — 2026-10-10

Scripts/TestMethodValues.ps1 -Cooked passed fresh asset creation/reopen, JSON
roundtrip, real PIE and packaged Win64 Development execution.

Fixtures test direct class fields, MethodGroup object/interface array members,
native input/result/ref/out signatures, owned copies, replacement and nested
indexed writes. Null elements round-trip and failed FailActors calls leave the
caller's ref array unchanged. Runtime failure groups: -61..-63.

Compiler negatives reject ObjectRef vs ActorRef, interface vs object array types,
readonly mutation/replacement and authored object-view array declarations.
The generated native properties are checked as actor-object and interface arrays.

The shared editor/PIE/packaged probe assigns a spawned Actor into DirectActors,
destroys it, then verifies a null handle on read and clears the native array.
Existing nested/scalar expired-object checks run alongside this case.

Markers:
- PASS reference_array_nominal_readonly_authored_compile_errors
- PASS reference_array_native_class_interface_identity
- PASS destroyed_native_reference_property_is_null
- DAS_METHOD_VALUES_PIE_RESULT=PASS
- DAS_METHOD_VALUES_PROBE=PASS cooked=True

Snapshot/frame ownership continues through existing FGCObject collectors;
this stage does not add a new ownership mechanism. Regression coverage includes
Actor/Component Create/Reopen, authored direct types Create/Reopen, foundation smoke
and native binding generation -Check. Shipping/AOT and logical authored element
identity/structural migrations remain outside this slice.
