# Blueprint Function Libraries

Function libraries use the existing checked UFunction frame/marshalling backend.
The exporter accepts native static BlueprintCallable functions on loaded subclasses
of UBlueprintFunctionLibrary, including engine libraries and user C++ libraries.
Selection class_rules, explicit aliases, supported parameter kinds, defaults and
signature validation work as for instance bindings. Latent and CustomThunk methods
remain rejected with requires_special_call_adapter; unsupported parameter types
retain explicit export diagnostics.

The export IR v5 adds optional static=true and world_context=<lowercase parameter>.
The generator produces a free function with no receiver, hides WorldContext from
inputs, begins a static frame and checks the exported signature before invocation.
Existing instance wrappers remain byte-for-byte unchanged for existing IR.

```das
require ue_library_generated
let magnitude = ue_lib_Abs(-3.0lf)
let seconds = ue_lib_Time()
let total = ue_lib_Add(20,22)
```

These aliases are from Bindings/FunctionLibraries.json; applications can choose
aliases or use class_rules to export other supported library functions.

Runtime calls execute via ProcessEvent on the library CDO. A WorldContext parameter
is pre-filled with the current script owner, pinned for the frame and marked as
assigned. The owner must have a compatible type and a game world. Scripts cannot
override this parameter. Immediately before invocation, the runtime verifies that
the same owner/world remains valid. Worldless pure calls need no game world.
There is no fallback to global editor/PIE world, first player or library CDO.

In editor builds, the metadata must still match the exported WorldContext contract.
The generated contract carries the parameter name for builds where editor metadata
is stripped; reflection still checks parameter shape. Packaged execution has not
been validated by this test.

Scripts/TestFunctionLibraries.ps1 builds the fixture, exports engine+user library
IR, generates ue_library_generated.das and runs DasForUE.FunctionLibraries.PIE.
Coverage includes generated static signatures, native math and GameplayStatics,
user C++ calls, correct owner world in two game worlds, pure worldless calls,
rejection of world-required calls without a world, WorldContext override/mismatch,
non-library static calls and frame/session cleanup. Generator tests also cover
malformed static/WorldContext contracts and compatibility of existing output.

Validation: TestFunctionLibraries.ps1 PASS (one successful PIE test, zero test warnings/errors); 90 generator tests PASS; TestBindingsPolicy.py PASS (9 export cases). Existing editor startup compression-asset warnings occur outside the automation test.
