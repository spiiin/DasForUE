# Natural authored property values

Properties declared with an authored ue_struct or ue_enum now retain their source
types through an authored object's typed view. Import the declaration module
alongside the reference module. Existing explicit registries/folders remain;
the generator checks native identity, layout and defaults against those assets.

```das
require ue_method_values
require METHOD_REFS_MODULE

def edit_settings(api:ValueTargetRef):void {
    var settings = api.Settings         // MethodSettings
    settings.Point.Position.x += 1.0lf
    settings.Weights["bonus"] = 2.5lf
    api.Settings = settings
    api.State = MethodState.Active
}
```

METHOD_REFS_MODULE is a template placeholder replaced with a GUID-specific name in executable
method-value test fixtures. Import the matching module for the actual source.

Getter values are independent copies. Editing a local value does not alter the
Unreal property until assigning the complete value back. Nested authored structs,
native math members and supported array/map/set members use the existing owned
snapshot converters. Readonly properties expose getters only; wrong struct types
and wrong enums (even with identical numbers) reject during compilation.

The same generated property operators serve ordinary functions and DSL methods.
Compile-chain descriptors contain qualified type names and registry scopes only;
no nested compiler AST pointers or new runtime metadata format are retained.
The getter/setter passes use the same source type resolver and pack/unpack helpers
as natural method signatures. Native math property access remains supported.

This stage does not add in-place nested writes through another object's view.
Use local-copy/whole-assignment syntax. An explicitly declared UEStructValue or
UEEnumValue field retains that explicit low-level type; natural declarations
do not require those wrappers. Missing imports produce compile errors.

Examples: dsl_method_value_target.das and dsl_method_value_driver.das.
Validation: [DSLPropertySourceValuesValidation.md](DSLPropertySourceValuesValidation.md).

Source converters also check exact reflected snapshot identity at runtime,
 before reading authored struct fields or casting enum numbers.
