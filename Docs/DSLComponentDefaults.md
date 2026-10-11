# Component template defaults

`ue_defaults` is a compile-time declaration block in an authored Actor. It does not run at BeginPlay and does not create a runtime field or closure.

```das
defaults = ue_defaults() {
    self.Child.RelativeLocation = ue_vector(0.0lf, 0.0lf, 50.0lf)
    self.Child.RelativeRotation = ue_rotator(0.0lf, 30.0lf, 0.0lf)
    self.Child.bHiddenInGame = true
    self.Behavior.Bonus = 42
}
```

The frontend extracts assignments before type inference. The target must be a declared component and one direct property. Supported source values are numeric/bool/string literals and `ue_vector`/`ue_rotator` with three numeric literal arguments. Calls, computations, control flow, duplicate assignments, unknown components and nested member paths reject. The label `defaults` is arbitrary and disappears from the compiled class.

Defaults are stored in each component's authoring contract and survive JSON roundtrip. Native template properties must be editable on templates and have a supported reflected type: int32, float, double, bool, FString, FName, FVector or FRotator. Values are checked for exact kind, range and finiteness. Authored component overrides currently support editable scalar fields (int/float/double/bool/string/name); their declaration is validated before component assets are imported.

Editor preflight checks targets and values before changing the Actor Blueprint. Values are then written to that Actor's SCS component templates. This does not change the component class CDO, another component template or existing instances. A new Actor receives the values during component construction. No arbitrary function executes during import.

The existing component migration guard remains: changing defaults on an already imported Actor is refused rather than overwriting edited templates or live instances. A migration policy is a separate remaining task. Arbitrary method calls, assets/object defaults, containers, Transform and authored struct/math overrides are not implemented by this slice.

Validation: `Scripts/TestNativeComponents.ps1` / `DasForUE.Components.Native` covers JSON roundtrip, spawned native location/rotation/visibility, authored Bonus override, unchanged sibling and class CDO, invalid target/type rejection and frontend rejection of duplicate/unknown/executable/computed defaults. Editor automation uses a game UWorld, not real PIE. Save/reopen and packaged checks remain pending.

Executable example: `Plugins/DasForUE/Content/Das/Scripts/dsl_native_components.das`.