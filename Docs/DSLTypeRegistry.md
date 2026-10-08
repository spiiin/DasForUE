# Stable type IDs and cooked direct values

`ue_class(type_registry="/Game/Folder/TypesRegistry.TypesRegistry")` resolves
annotated plain enums/structs by their stable IDs through a UDasTypeRegistry asset.
It is mutually exclusive with `type_folder`; the existing folder/name workflow
remains supported.

The registry stores typed binary Unreal properties: ID, kind, hard asset reference
and (for structs) the native persistent GUID. It does not store JSON descriptions
or depend on editor metadata at runtime. UDasDefinedEnum also serializes its stable
ID. Resolution validates the catalog and exact asset identity, then the existing
adapter checks enum members/values and struct layout/defaults.

IDs still default to `module_id.TypeName`; explicit IDs are optional. Preserve an
old ID when renaming the daScript declaration. The registered example intentionally
uses `PlayState`/`GameSettings` with IDs `direct.State`/`direct.Settings` and resolves
to Unreal assets named `State`/`Settings`.

```das
[ue_enum(id="direct.State", module_id="direct", BlueprintType)]
enum PlayState {
    Idle = 7
    Active = 42
}

[ue_class(module_id="authoredvalues",
          type_registry="/Game/DasRegisteredTypes/TypesRegistry.TypesRegistry",
          Blueprintable)]
class TypedActor {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Mode : PlayState = PlayState.Idle

    [ue_function(BlueprintCallable)]
    def SetMode(value:PlayState):void { self.Mode = value }
}
```

To create the original types and catalog, then the class using renamed source
declarations, run in the Unreal editor console:

```text
Das.DSL.Types.Apply dsl_direct_types.das /Game/DasRegisteredTypes/Types /Game/DasRegisteredTypes/TypesRegistry
Das.DSL.Apply dsl_registered_types.das /Game/DasRegisteredTypes/BP_Actor authoredvalues.TypedActor
```

Save the types, catalog, Blueprint and map. Assign the catalog to the Actor
adapter's `Type Registry` property in the map (or arrange an explicit hard cook
dependency through your project). A string path in daScript alone is not a cook
dependency. The catalog's hard references include its registered types in cook and
keep them alive while the catalog is referenced. The packaged test cooks only its
map and follows the Actor's hard reference; it does not use CookAll or CookDir.

The optional third Types.Apply argument publishes the catalog. Reapplying an exact
binding set reuses the same object. Publishing an old authored enum upgrades its
missing runtime ID from editor authoring metadata and marks it dirty; save it too.
Duplicate/missing IDs, duplicate assets, wrong kinds/GUIDs, conflicting packages
and changed existing bindings are rejected. Catalog rebinding/definition changes
still require a future explicit migration policy. No global asset scan or automatic
discovery across project folders is performed.

Example: Plugins/DasForUE/Content/Das/Scripts/dsl_registered_types.das.
Test: Scripts/TestDSLTypeRegistry.ps1; add `-Cooked` for build/cook/stage/archive
and an actual game launch. GUID source files are cleaned up; saved fixtures belong
only to the isolated smoke project. See DSLTypeRegistryValidation.md.


Names used by raw-handle arg_name_enum/struct and return_enum/struct annotations
can now resolve through the same explicitly selected catalog as natural declared
types. See DSLTypeResolution.md for source aliases, qualified IDs and ambiguity
rules. Catalog ownership, hard cook references and GUID validation are unchanged.
