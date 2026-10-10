# Whole nested self values

Inside ue_class methods, whole authored struct and native math members of an
authored struct property can be assigned directly:

```das
self.Settings.Point = point
self.Settings.Point.Position = ue_vector(5.0lf, 8.0lf, 3.0lf)
self.Settings.Orientation = rotation
self.Settings.Pose = transform
```

Use daScript's native clone operator for structs containing containers:

```das
self.Envelope.Payload := settings
```

daScript rejects plain = for such noncopyable source structs; := creates the
owned value required by its language semantics. The DSL now handles ExprClone
as well as assignment/move nodes. This is a source-language rule, not additional
UE metadata or a new storage wrapper.

All typed leaves (enum, native reference, authored struct and builtin math)
share the existing typed commit generator. It checks the RHS source type, packs
through existing converters, updates the exact nested reflected struct member
and commits the latest root property snapshot. Runtime struct identity and
math validity checks remain. Readonly roots and compound value arithmetic reject.
Sibling fields stay intact; copied containers remain independent.

Whole property clone assignment through self uses the same existing property
commit path. Indexed self mutations and nested writes through other-object views
remain separate. For those views, edit a local copy and assign the whole property.

Examples: ValueTarget.ReplaceSelfValues; MethodEnvelope.Payload contains the
existing MethodSettings, including nested enum/math/reference members and
struct arrays/maps/sets. The driver checks sibling Sentinel=91, root Total=3,
container roundtrip and independent readback.
Validation: [DSLSelfNestedValuesValidation.md](DSLSelfNestedValuesValidation.md).
