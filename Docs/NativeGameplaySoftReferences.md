# Generated native soft references

Native reflection now recognizes `FSoftObjectProperty` and `FSoftClassProperty` before ordinary object/struct handling. Accepted fields and method signatures use the same checked immutable soft values as authored DSL properties.

```das
require ue_native_gameplay

def use_asset(api:NativeHashProbeRef) {
    api.SoftAsset=ue_soft(type<SoftObject>,"/Engine/BasicShapes/Cube.Cube")
    let asset=api.EchoSoftAsset(api.SoftAsset)
    let path=ue_soft_path(asset)

    var items:array<SoftObject>
    push(items,asset)
    api.SoftAssets:=items
    api.SoftAssets[0]=default<SoftObject>

    var previous:array<SoftObject>
    api.ChangeSoftAssets(items,previous)
}
```

`SoftObject` and `SoftActorClass` are shared public aliases. The generator emits aliases for each required native object/class constraint and reuses canonical native object wrappers. Identical aliases resolve to the same nominal soft type; users do not need to name private carrier types.

Supported shapes:

- Scalar soft object/class fields, parameters, returns, `ref` and `out`.
- Arrays of soft references and maps with existing supported keys and soft reference values, including method arguments, returns, `ref` and `out`.
- Native value structs containing soft fields and soft arrays/maps.
- Ordinary property syntax, indexed writes and nested field writes; detached arrays/maps/structs retain snapshot semantics.

The manifest retains the shared `kind="struct"` transport and adds `soft_reference={base,class_ref}` to each soft leaf. The exporter never recursively exposes the private soft carrier's `Values` field as a native value struct. The generator validates native base paths and canonical carrier identity. Compiled field/method signatures include the soft base constraint so a changed constraint requires regeneration. No reflection pointer is persisted across reload. Internal pack/unpack helpers are private to each source module so imported modules can safely use the same container types.

Paths are preserved without loading. `ue_soft_get` only resolves loaded values; `ue_soft_load` and managed asynchronous loading remain explicit. Asset-only path rules, nullability, weak lifetime, and rejection of subobjects are unchanged. Soft map keys and sets remain unsupported. Implicit soft-reference conversions to a parent constraint are a separate follow-up.

The native ABI fixture is `UDasNativeHashProbeComponent`; `native_soft_probe.das` calls its reflected methods and fields from the soft reference actor's real BeginPlay. `Scripts/TestSoftReferences.ps1` checks PIE and Foundation; `Scripts/TestSoftReferencesPackaged.ps1` builds/cooks/runs the same behavior in Development without editor metadata.

Validation (2026-10-17): Editor Development build, all 125 Python checks, Foundation in Editor and packaged Development (zero failures), dedicated real PIE (one success; zero warnings/failures/skips), cook/stage/archive and packaged native calls passed. Negative compiler checks reject object/class, array and ref mismatches. The packaged probe also verifies actual native C++ soft fields. Evidence and source hashes: `logs/native-soft-validation.json`, `logs/native-soft-source-manifest.json`. Validation ran against a frozen source copy to avoid concurrent edits.

The main repository Editor DLL was rebuilt successfully after validation.
