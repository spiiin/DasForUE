# Authored reflected enum and struct declarations

```text
[ue_enum(module_id="types", BlueprintType)]
enum State {
    Idle = 7
    Active = 42
}

[ue_struct(module_id="types", BlueprintType)]
struct Settings {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Count : int = 10
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Title : string = "Привет"
}
```

These are native daScript EnumerationAnnotation and StructureAnnotation hooks.
They collect a syntax-independent reflected-types model while compiling the
source. `ReadReflectedTypes` never simulates the program or creates Unreal assets.
Classes retain their existing separate authoring model/entrypoint backend.

Create the reflected assets in the editor:

```text
Das.DSL.Types.Apply dsl_type_declarations.das /Game/MyTypes
```

Each declared type gets a package/object with its declared name inside the selected
folder. The command creates and marks assets dirty for review/save; it does not
autosave or rewrite source. Both the script name and `/Game` folder are validated.
Existing paths and declarations are preflighted before new assets are created.
An exact repeated declaration reuses the same objects. Occupied paths, identity
conflicts, altered definitions and detected native asset drift are refused.

Enum declarations require explicit unique integer literals in 0..254. Value 255
and `_MAX` member names are reserved. The backend uses runtime UDasDefinedEnum,
an asset subclass of UEnum, with BlueprintType metadata and a hidden MAX entry.
UUserDefinedEnum is intentionally unsuitable for sparse values because its editor
loading path renumbers entries sequentially. UDasDefinedEnum preserves explicit
values through serialization. Normal Blueprint enum fields can use the asset;
the default symbol Active is verified to retain value 42 across save/reopen.

Struct declarations create UUserDefinedStruct assets using Unreal's struct editor
compiler. Initial fields are int32, float32, double, bool, string and name, with
supported scalar literal defaults, EditAnywhere/BlueprintReadWrite and metadata.
Read-only fields, inheritance, methods, nested/object/container fields need further
support and are refused. Unreal's persistent variable GUID/internal property name
is retained; DasMemberId records the authoring stable ID. User-facing field names
and defaults survive asset save/reopen. Do not assume generated internal property
names equal source names when accessing the native struct.

This stage implements declaration collection, reflected asset creation, validation
and ordinary Blueprint usage. Connecting these authored type identities to fields
and method frames in ue_class, value conversion between native daScript enums/
structs and Unreal storage, and type hot-reload/data migration are the next stage.
Existing native reflected-struct and object APIs keep their current contracts.
There is no direct memory-layout equivalence promise between a daScript struct and
UUserDefinedStruct.

Example: Plugins/DasForUE/Content/Das/Scripts/dsl_type_declarations.das.
Tests: Scripts/TestDSLTypesAsset.ps1; results: DSLTypeDeclarationsValidation.md.


Loaded type assets can now be used by ue_class through checked UEEnumValue/
UEStructValue field and frame adapters; see DSLAuthoredValues.md. Direct plain
daScript type conversion and type-change migration remain follow-up stages.


## Container field extensions

The initial scalar-only scope above has since expanded to nested authored
structs, struct/primitive arrays, maps with int/string keys and scalar/name
values, and int/string sets.
Key-only daScript `table<T>` fields create TSet, with ordinary ue_property
annotations. See DSLSetAuthoring.md, DSLMapAuthoring.md,
DSLPrimitiveArrays.md and DSLNestedTypeAuthoring.md for current support.
Defaults for these containers remain implicit empty values and unsupported
container structural migrations explicitly reject.
