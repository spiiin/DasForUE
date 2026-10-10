# Direct self native reference member writes

Native object/interface members in authored structs support direct assignment
inside ue_class methods, using their imported wrapper types:

```das
self.Settings.Point.Owner = actor            // ActorRef
self.Settings.Point.Events = events          // DasEventInterface
self.Settings.Point.Owner = default<ActorRef>
self.Settings.Point.Events = default<DasEventInterface>
```

Declare the struct members with ue_property as usual; omit their initializers
for native null defaults. The containing struct still uses the explicit authored
registry/folder. Import the native wrappers alongside the authored struct module.

Object/interface and enum leaves share one generated source-typed commit helper
mechanism. RHS types are checked before converting to opaque interop handles;
ObjectRef does not implicitly become ActorRef or an interface wrapper. Readonly
roots reject writes and reference arithmetic is unsupported.

The commit reuses the existing latest-snapshot/member/whole-property update.
Native property class/interface checks and handle liveness checks remain.
Other struct members are preserved. Wrappers retain weak checked semantics;
Unreal property ownership is unchanged. A struct member referencing a destroyed
Actor reads as an empty wrapper and can be cleared through direct self assignment.

This does not add authored class-reference members, soft/class references,
indexed self mutations or nested writes through another object's view. Use a
local copy and whole assignment for other-object struct properties.

Examples: MethodPoint.Owner/Events in ue_method_values.das; ValueTarget's
SetSelfReferences in dsl_method_value_target.das. The test driver exercises
live/null roundtrips and the shared C++ probe injects and destroys an Actor.
Validation: [DSLSelfReferenceWritesValidation.md](DSLSelfReferenceWritesValidation.md).
