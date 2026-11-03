# Native object reference macro validation

2026-11-03, UE 5.8.3. Shared-agent job `native-object-ref-v1-20261103`: PASS, reflection macro suite 114 checks.

`UENativeObjectRef` reuses `DSLExpandReferenceType` with the existing `ue_native_object` annotation and one `UObjectHandle` field. `DSLObjectNormalize` recognizes both qualified and unqualified spellings. The annotation checks an already loaded native UObject class at compile time; existing native property, method, signature and reference-conversion transports are reused.

The suite now includes 13 additional checks: catalog-independent compilation; rejection of nonliteral/empty/extra arguments, unknown classes, reflected value structs, interfaces, authored asset paths and empty paths; repeated/qualified nominal alias compatibility; nullable values; wrong-runtime-class rejection; accepting instances derived from a selected native class. Existing schema compilation checks additionally cover a native skeletal-mesh property and reflected function return type, and the existing native actor argument transport runs through the macro-generated view.

The ActionRoguelike alias in `rogue_presentation_types.das` was migrated to `UENativeObjectRef("/Script/Engine.SkeletalMesh")`. Gameplay/PIE was not part of this targeted macro suite.
