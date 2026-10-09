# Typed interface factory and source value bindings

The supported daScript factory syntax uses an explicit type argument expression:

```das
let api = ue_interface(type<CounterContract>, target)
let answer = api.Query(5)

let state = api.EchoState(InterfaceState.Active)
var settings = InterfaceSettings()
settings.Count = 33
settings.Title = "Generated 世界"
let copied = api.EchoSettings(settings)
```

The generator emits exact overloads for selected interface-reference types only.
It delegates to the existing checked weak-reference constructor; null handling,
context/lifetime/membership and actual/declared signature guards remain unchanged.
This is verified daScript type<...> syntax, not the earlier C++-style proposal
ue_interface<CounterContract>(target). CounterContract_from remains compatible.

## Explicit source value mapping

GenerateBindings.py --source-types manifest.json connects the ergonomic method layer
to a separately compiled public declaration module. Low-level wrappers retain their
existing generated nominal value types and ABI. Compiler-generated converters map
natural source enum members by identity/symbol (not ordinal casts), and scalar/string
struct fields by explicit native-to-source member mapping. Method inputs, return
values, ref/out copyback and supported enum default expressions use these converters.

Manifest shape:

```json
{
  "module": "ue_interface_values",
  "registry": "/Game/Types/TypesRegistry.TypesRegistry",
  "types": [
    {
      "path": "/Game/Types/InterfaceState.InterfaceState",
      "kind": "enum",
      "name": "InterfaceState",
      "id": "interfacevalues.State",
      "members": { "Idle": 7, "Active": 42 }
    },
    {
      "path": "/Game/Types/InterfaceSettings.InterfaceSettings",
      "kind": "struct",
      "name": "InterfaceSettings",
      "id": "interfacevalues.Settings",
      "members": {
        "count_2_<native-guid>": "Count",
        "title_4_<native-guid>": "Title"
      }
    }
  ]
}
```

Native struct keys above are schematic; use exact field paths from the exported IR.
This configuration remains authoritative and does not guess assets/declarations by
global names. The module supplies normal ue_enum/ue_struct definitions. Types from
the mapped module are publicly imported; duplicate generated short aliases for those
types are suppressed. Unmapped low-level types and native generation remain compatible.

The Python generator checks manifest shape, module/identity names, enum symbol/value
sets, struct field coverage and supported field kinds. Emitted internal
ue_source_value_binding annotations validate the actual source declaration ID,
explicit registry binding, canonical native path, GUID and existing enum/struct
layout/default rules during compilation. Wrong IDs reject before any interface call.
Actual values are converted with ordinary checked script code; native pointers or
unresolved JSON metadata are not stored in caller values.

This first mapping stage supports source enums and authored structs with primitive
numeric/bool/string fields. Nested structures, containers/name/object fields and
container element mapping remain later extensions; unsupported struct shapes reject.
No blanket implicit conversions between all generated and source types are added.
The explicit registry needs a real cook hard reference, as before. Structural
migration, reflected interface storage, broader authoring out/ref and VM reentry
remain separate work.

GenerateAuthoredInterfaceProbe.py demonstrates deriving an explicit manifest from
the known declaration module and actual reflected assets for an isolated test.
Production generation uses GenerateBindings.py --module ... --source-types ....
The executable scenario uses InterfaceState values 7/42 and initialized InterfaceSettings,
then verifies a Unicode/int roundtrip on a plain Blueprint receiver in editor/game.
