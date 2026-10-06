# Reflected struct properties and defaults

Native DSL now supports `UEStructValue` reflected fields in Actor adapter Blueprints.
The initializer carries the loaded native UScriptStruct identity:

```dascript
@ue_property=(EditAnywhere,BlueprintReadWrite)
Payload : UEStructValue = ue_struct_default(
    "/Script/DasRuntime.DasCallPayload", "(Count=10,Label=\"Initial\")")
```

`ue_struct_value("/Script/...")` uses the native constructor default instead.
Initializer arguments must be literal strings: schema extraction does not execute
script initializers. Type/default use authoring JSON v1 `kind: "struct"`, `type_path`
and a string default; no runtime JSON interpreter is introduced.

Defaults use a strict subset of Unreal property export text: parenthesized named
fields, nested structs, quoted/escaped strings, scalar/name/registered enum values.
Unknown or duplicate fields, malformed text and trailing data are refused before
Blueprint mutation. Omitted fields retain the native constructor default. Native
ImportText overrides are disabled; parsed snapshots pass the same reflected value
capability and enum checks as method arguments. Text is limited to 1 MiB.

`self.Payload` reads an immutable snapshot. Assign a changed snapshot back:

```dascript
self.Payload = ue_struct_with_int(self.Payload,"Count",42)
```

Writes validate exact UScriptStruct identity and BlueprintReadOnly. An incompatible
assignment fails before modifying the actor. Snapshot allocation participates in
existing interpreter GC and call boundary reclamation. Reads never expose a pointer
into actor memory; writes copy the native value, including strings/nested structs.

The editor creates a real typed Blueprint variable with a CDO default and normal
Details flags. Reimport checks reflected type/default/readonly before promotion.
Adding another field recompiles/reinstances the Blueprint while preserving existing
actor overrides. Changing an existing type, default, identity or editability still
requires a future migration policy and is rejected. Migration is not implemented.

This step covers generic UEStructValue fields with supported scalar/nested/enum
leaves. UObject/container fields, script-defined UScriptStruct declarations and
direct UEVector/UERotator/UETransform field syntax remain separate capabilities.

Example: Plugins/DasForUE/Content/Das/Scripts/dsl_struct_fields.das.
Verification: Scripts/TestEditor.ps1, TestPackaged.ps1,
TestDSLStructFieldsAsset.ps1 and TestDSLStructFieldsPIE.ps1.
