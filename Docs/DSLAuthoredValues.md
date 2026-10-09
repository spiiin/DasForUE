# Authored enum/struct values in ue_class

First create/load the reflected types with `Das.DSL.Types.Apply`. The class source
then refers to the exact loaded Unreal asset paths:

```text
@ue_property=(EditAnywhere, BlueprintReadWrite)
Mode : UEEnumValue = ue_enum_value("/Game/MyTypes/State.State", "Idle")

@ue_property=(EditAnywhere, BlueprintReadWrite)
Config : UEStructValue = ue_struct_default("/Game/MyTypes/Settings.Settings",
                                       "(Count=15,Title=\"Initial\")")

[ue_function(BlueprintCallable, arg_value_enum="/Game/MyTypes/State.State")]
def SetMode(value:UEEnumValue):void { self.Mode = value }

[ue_function(BlueprintCallable, return_enum="/Game/MyTypes/State.State")]
def GetMode():UEEnumValue { return self.Mode }
```

These fields become real typed enum-byte and struct Blueprint properties. Enum
arguments use `arg_<name>_enum`, results `return_enum`. Struct arguments/results
retain `arg_<name>_struct`/`return_struct`. Mixed frames preserve type identities;
the same integer value from a different enum is incompatible. Types must already
be loaded before compile-only class authoring validation. No global type lookup by
short name or automatic creation during macro compilation is performed.

UEEnumValue is an immutable context-owned POD token. `ue_enum_value(path, member)`
checks a named member; `ue_enum_number(value)` reads its explicit integer. Unknown,
reserved MAX and values outside 0..254 are rejected. Hidden native enum metadata
is checked in Editor; authored enums reserve their MAX member by name in both
Editor and packaged runtime. Foreign or expired enum tokens fail. The token table
reuses enum/value pairs and is limited to 4096 live entries per context.

Blueprint enum pins retain their UEnum identity. Internal PackValue/UnpackValue
custom thunks transport the byte plus exact enum path through the generic frame.
Invalid input refuses before script/property mutation; a malformed native output
aborts Blueprint execution and leaves the destination unchanged. Return identity
is validated again in runtime. The initial enum pin domain is typed byte enums.

UEStructValue remains an owned immutable FInstancedStruct snapshot. Loaded native
structs and scalar-only UUserDefinedStruct assets are accepted after reflection
validation. Runtime resolves authored field names through GetAuthoredNameForField;
native internal GUID-suffixed names remain valid too. Defaults translate friendly
names into internal property names before ImportText and reject duplicate aliases,
unknown fields, malformed text and incompatible storage. Mixed native/daScript
string and scalar data is copied by reflected property; no layout equality is used.

Existing self access, snapshots/GC, stable-ID rename, retirement and rollback
helpers recognize enum kind 13 and its type path. Authored struct snapshots use the
same native migration storage as supported native structs. Type asset declarations
remain immutable in this stage; enum member/layout changes still require a later
explicit type/data migration policy.

This bridge uses UEEnumValue and UEStructValue. Plain daScript `State` and `Settings` adapters are now implemented in
DSLDirectTypes.md. Automatic stable-ID-to-asset resolution, nested/complex
authored structs and type hot reload remain separate extensions.

Example: Plugins/DasForUE/Content/Das/Scripts/dsl_authored_values.das (paths initially
use /Game/DasAuthoredTypes). Tests: TestDSLAuthoredValuesAsset.ps1 and
TestDSLAuthoredValuesPIE.ps1. Results: DSLAuthoredValuesValidation.md.


Within reflected ue_class methods and field initializers, these raw snapshot
constructors now accept scoped short literal names or `ue_type(GameSettings)` /
`ue_type(PlayState)` in the type argument. Natural typed fields/signatures remain
preferred. See DSLTypeCalls.md for registry rules and standalone-function limits.
