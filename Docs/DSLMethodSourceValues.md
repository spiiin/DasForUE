# Natural source values in authored method calls

Methods declared with authored enum/struct types now expose those same source
types through authored references. The caller imports the value module and uses
the types directly:

```das
require ue_method_values

let state = api.EchoState(MethodState.Active)
var settings = MethodSettings()
settings.Point.Count = 11
settings.Point.Note = "hello 世界"
let result = api.EchoSettings(settings)

var previous = MethodSettings()
api.ChangeSettings(settings, previous)

var current = MethodState.Active
var old = MethodState.Idle
api.ChangeState(current, old)
```

The target declarations use ordinary annotated methods:

```das
[ue_function(BlueprintCallable)]
def EchoState(value:MethodState):MethodState { return value }

[ue_function(BlueprintCallable)]
def EchoSettings(value:MethodSettings):MethodSettings { return value }

[ue_function(BlueprintCallable)]
def ChangeSettings(var value:MethodSettings&;
                   @ue_out var previous:MethodSettings&):void {
    previous = value
    value.Point.Count += 5
}
```

The class still names an explicit type_registry or type_folder, as before.
The registry resolves source stable identities to the actual reflected assets.
Imports are explicit; a missing source declaration produces a compiler diagnostic.
Matching integer enum values do not make two enum types interchangeable. Wrong
enum/struct arguments reject during compilation instead of crossing as opaque
UEEnumValue/UEStructValue values.

The signature collector retains qualified source type names and the resolution
scope only for the current compile chain. It does not retain AST pointers from a
nested source compiler. Descriptors are cleared at the outer compile boundary.
The method emitter resolves the imported declarations, validates their current
native identities/layout/defaults, and reuses the existing source pack/unpack
helpers. No second runtime marshalling backend is introduced.

Return values and ref/out copyback use natural source types as well. Conversion of
every result/output completes before caller variables change. Recovering from a
failed callee preserves the caller's ref value. Ordinary DAS functions and DSL
methods use the same overloads. Nested struct fields and strings are supported.
Existing explicitly opaque signatures remain opaque; their APIs are unchanged.

Authored enum/struct signatures now also support container arguments and fields;
see DSLMethodContainers.md for the current carrier policy. Native object/interface and math signatures now retain their source types; see
DSLMethodObjectMath.md. General migrations remain
deferred.

Examples: Content/Das/Scripts/ue_method_values.das,
dsl_method_value_target.das and dsl_method_value_driver.das. The latter two are test
templates: TestMethodValues.ps1 creates GUID sources with a real registry path and
target source name. Generated sources/assets are local test artifacts.

Run Scripts/TestMethodValues.ps1 -Cooked for Create, Reopen, actual PIE and a
packaged game. The test refreshes its own game's makefile and verifies that both
generated source files are present in the receipt before cooking.

Validation: [DSLMethodSourceValuesValidation](DSLMethodSourceValuesValidation.md).

Native object/interface and math method signatures:
[DSLMethodObjectMath](DSLMethodObjectMath.md).
