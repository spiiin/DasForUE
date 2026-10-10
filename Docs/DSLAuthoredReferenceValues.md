# Enum, struct and object fields through authored references

Authored object views now also support whole-field access for the existing
UEEnumValue, UEStructValue and UObjectHandle representations:

```das
let actor = ue_ref(type<ScalarTargetRef>, target)
actor.Mode = ue_enum_value("EDasCallMode", "Active")
let original = actor.Payload
actor.Payload = ue_struct_with_int(original, "Count", 31)
actor.Target = peer
```

The field name is obtained from the declaration and ordinary member syntax;
no per-class runtime adapter or property-name string is required for access.
Existing value-construction/snapshot APIs remain applicable. Enum values retain
exact enum identity, struct values are owned snapshots, and object/interface
values are checked weak handles. A reflected UObject field remains a strong UE
property reference after assignment; the script handle remains weak.

Struct reads copy the reflected value into a GC-aware snapshot. Assignments
validate snapshot layout/bounds, exact UScriptStruct identity and object liveness
before committing. Changing the target does not mutate a previously read snapshot.
Enum writes require the same UEnum and a valid member; object writes require the
actual declared property class/interface. Null object assignment retains the
existing reference API behavior. Wrong values reject before writing the field.
All access still checks authored class identity, field kind and permissions.

This stage adds whole-field getter/setter adapters. It does not yet map opaque
native structs to natural source structs, infer enum member syntax, expose object
fields as nested authored views, or support actor.Payload.Count and container
mutation. Those are separate source-value/nested-access steps. Existing direct
authored enum/struct facilities on self and function boundaries remain available.
General migration policies remain deferred.

Executable example: dsl_ref_scalar_target.das and dsl_ref_scalar_driver.das under
Content/Das/Scripts. Values exercises all three kinds; WrongStruct/WrongEnum/
WrongObject are intentionally failing runtime methods used by the commandlet.

Validation: [DSLAuthoredReferenceValuesValidation](DSLAuthoredReferenceValuesValidation.md).
