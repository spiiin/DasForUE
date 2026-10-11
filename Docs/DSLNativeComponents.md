# Native component declarations — 2026-10-11

Implemented slice of the Actor/Component feature expansion. This does not complete the full roadmap.

Native wrappers annotated with `ue_native_object` can now appear in `ue_components` and individual `@ue_component` fields. Authored and native components can coexist. Native types must resolve to concrete ActorComponent subclasses. Existing authored declarations keep their JSON representation; native declarations add `native_class`.

```das
require ue_scene_components

[ue_class(module_id="example", Blueprintable)]
class Example {
    @ue_component
    @RootComponent
    SceneRoot:SceneComponentRef

    @ue_component
    @Attach="SceneRoot"
    Child:SceneComponentRef
}
```

`@AttachSocket="SocketName"` persists the SCS attachment socket. Actual named mesh sockets have not been integration-tested. Without an explicit root, the first native SceneComponent with no explicit parent becomes root. Unattached scene children attach to that root. Non-scene components have no scene parent. Cycles, missing parents and multiple roots reject during model validation; scene-only constraints are checked before creating component assets.

The existing native host keeps its fallback ProbeRoot as a child. OnConstruction promotes the declared scene component to the actual Actor root. This host implementation is not a general user-authored ConstructionScript and does not support overriding inherited default-subobject classes.

Test: `Scripts/TestNativeComponents.ps1`, automation `DasForUE.Components.Native`.
- Editor build passed.
- One automation test passed with zero test warnings/errors.
- Mixed native/authored SCS import, JSON roundtrip, Actor spawn, actual root, implicit and explicit attachment, self native reference and unchanged reimport checked.
- Scene create, relative/world transform setters, attach/detach, owned destroy and invalidation exercised in a game UWorld inside Editor automation.
- Missing parent, multiple root and cyclic graph model rejection checked.
- This is not a real PIE or packaged test. Save/reopen, actual mesh sockets, editor reconstruction and cooked validation remain pending.

Source: `Plugins/DasForUE/Content/Das/Scripts/dsl_native_components.das`.
Reports: `logs/native-components-report/index.json`, `logs/native-components.log`.

Remaining authorized roadmap: component template defaults (including a deliberate policy for method calls), general typed component lookup/create/GetOrCreate, Actor spawn and class references, all-Actor/component queries, user ConstructionScript, broader native/authored inheritance and OverrideComponent. Existing component structural migration guards remain active.
2026-10-11: accumulated validation passed native/dynamic/actor-component PIE and cooked native/authored templates, root, attachments and component lifecycle. Loaded cooked actors now restore the declared root in PostInitializeComponents. See [AccumulatedValidation.md](AccumulatedValidation.md).
