# Type Support

This page summarizes which value kinds can appear in which positions of authored DSL code (classes, structs, interfaces, delegates, events and timers) and in generated native API. It is a map; the linked pages have the exact rules. A loaded, registered concrete enum/struct/class identity is required wherever a type is named, and "Yes" never means unrestricted signatures.

<!-- BEGIN GENERATED TYPE SUPPORT -->
## Registry-derived contracts

Generated from the live registry export in [Bindings/TypeSupport.json](../../Bindings/TypeSupport.json). Update with `GenerateTypeSupport.py --manifest logs/type-support-live.json`; verify with `--check`.

**Scope:** representative property descriptors checked by `ValidateDasPropertyType`. `Value`, `Snapshot member`, and `Delegate parameter` mean the three **property ABI usages**, not acceptance by every authored DSL parser, signature validator or callback. Container columns validate the complete composed descriptor with this sample as the child (the other map slot is `int`). Native class, interface and weak-reference samples remain separate.

`Yes` applies to the shown descriptor only. `No` has the exact validator reason in the JSON. `Unreported` means an adapter has no representative sample; it is never rendered as supported.

| Representative descriptor | Value | Snapshot member | Delegate parameter | Array element | Map value | Map key | Set element |
| --- | --- | --- | --- | --- | --- | --- | --- |
| <code>array&lt;int&gt;</code> | Yes | Yes | Yes | No | No | No | No |
| <code>bool</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>byte</code> | Yes | Yes | Yes | Yes | Yes | Yes | Yes |
| <code>class:/Script/CoreUObject.Object</code> | Yes | Yes | Yes | No | No | No | No |
| <code>double</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>enum:/Script/Engine.ECollisionChannel</code> | Yes | Yes | Yes | Yes | Yes | Yes | Yes |
| <code>float</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>int</code> | Yes | Yes | Yes | Yes | Yes | Yes | Yes |
| <code>map&lt;int, int&gt;</code> | Yes | Yes | Yes | No | No | No | No |
| <code>name</code> | Yes | Yes | Yes | Yes | Yes | Yes | Yes |
| <code>object:/Script/CoreUObject.Object</code> | Yes | Yes | Yes | Yes | Yes | Yes | Yes |
| <code>weak&lt;object:/Script/CoreUObject.Object&gt;</code> | Yes | Yes | Yes | No | No | No | No |
| <code>object:/Script/CoreUObject.Interface</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>set&lt;int&gt;</code> | Yes | Yes | Yes | No | No | No | No |
| <code>string</code> | Yes | Yes | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/CoreUObject.Rotator</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>struct:/Script/CoreUObject.Transform</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>struct:/Script/CoreUObject.Vector</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>struct:/Script/CoreUObject.Vector2D</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>struct:/Script/EnhancedInput.InputActionValue</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>struct:/Script/GameplayTags.GameplayTag</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>struct:/Script/GameplayTags.GameplayTagContainer</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>struct:/Script/DasRuntime.DasSoftClass_f6005a7380ac4daaf37e8e855ef43965</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>struct:/Script/DasRuntime.DasSoftObject_f6005a7380ac4daaf37e8e855ef43965</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>text</code> | Yes | Yes | Yes | Yes | Yes | No | No |

### Registered script facets and default policies

These are adapter/facet declarations, not an independent support table. `—` means no dedicated hook is declared; generic lowering may still apply. View flags describe specialized view uses, not complete field/signature support. `explicit`/`implicit_empty` is the adapter default policy, not a claim that a particular property, parameter or computed-default expression is accepted.

| Adapter | View module | Specialized view uses | Frame / method bridges | Default policy | Default kind |
| --- | --- | --- | --- | --- | --- |
| <code>array</code> | — | — | — / — | <code>explicit</code> | — |
| <code>bool</code> | — | — | — / — | <code>explicit</code> | — |
| <code>byte</code> | — | — | — / — | <code>explicit</code> | — |
| <code>class</code> | <code>ue_native_gameplay</code> | native_property, self_property, authored_reference | — / — | <code>explicit</code> | — |
| <code>double</code> | — | — | — / — | <code>explicit</code> | — |
| <code>enum</code> | <code>ue_native_gameplay</code> | native_property | — / — | <code>explicit</code> | — |
| <code>float</code> | — | — | — / — | <code>explicit</code> | — |
| <code>int</code> | — | — | — / — | <code>explicit</code> | — |
| <code>map</code> | — | — | — / — | <code>explicit</code> | — |
| <code>name</code> | <code>ue_foundation</code> | native_property, self_property, authored_reference | — / — | <code>explicit</code> | — |
| <code>object</code> | <code>ue_native_gameplay</code> | native_property, self_property, authored_reference | — / — | <code>explicit</code> | — |
| <code>set</code> | — | — | — / — | <code>explicit</code> | — |
| <code>string</code> | — | — | — / — | <code>explicit</code> | — |
| <code>struct:/Script/CoreUObject.Rotator</code> | <code>ue_foundation</code> | native_property, self_property, authored_reference | <code>rotator</code> / <code>rotator</code> | <code>explicit</code> | <code>rotator</code> |
| <code>struct:/Script/CoreUObject.Transform</code> | <code>ue_foundation</code> | native_property, self_property, authored_reference | <code>transform</code> / <code>transform</code> | <code>explicit</code> | <code>transform</code> |
| <code>struct:/Script/CoreUObject.Vector</code> | <code>ue_foundation</code> | native_property, self_property, authored_reference | <code>vector</code> / <code>vector</code> | <code>explicit</code> | <code>vector</code> |
| <code>struct:/Script/CoreUObject.Vector2D</code> | <code>ue_foundation</code> | native_property, self_property | — / — | <code>explicit</code> | <code>vector2d</code> |
| <code>struct:/Script/EnhancedInput.InputActionValue</code> | <code>ue_foundation</code> | — | — / <code>input_value</code> | <code>explicit</code> | — |
| <code>struct:/Script/GameplayTags.GameplayTag</code> | <code>ue_gameplay_tags</code> | native_property | — / — | <code>implicit_empty</code> | — |
| <code>struct:/Script/GameplayTags.GameplayTagContainer</code> | <code>ue_gameplay_tags</code> | native_property | — / — | <code>implicit_empty</code> | — |
| <code>struct:family</code> | <code>ue_native_gameplay</code> | native_property | — / — | <code>implicit_empty</code> | — |
| <code>text</code> | <code>ue_foundation</code> | native_property, self_property, authored_reference | — / — | <code>explicit</code> | — |

Coverage: **22 registered adapters, 25 representative descriptors**.

**AOT:** per binding; not inferred from a type adapter. A scalar-compatible binding can be AOT eligible while an opaque overload of the same operation is closed. A type-level `Yes` would be misleading.

**Not inferred by this registry export:** authored field/signature and default-expression syntax; event and timer callback payload contracts; generated native API and unregistered engine transports.

<!-- END GENERATED TYPE SUPPORT -->

The authored and generated-API summaries below are contextual guidance maintained separately:
those subsystems have additional policies that the adapter registry does not yet expose.
The generated tables above deliberately do not turn property-ABI acceptance into a general DSL/API promise.

## Authored DSL

| Value kind | Field | Param in | Param out / ref | Return | Array element | Map key / set element | Map value | Event payload (`ue_event`, delegates) | Timer argument | Default value |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `int`, `float`, `double`, `bool`, `string` | Yes | Yes | Yes | Yes | Yes | `int`, `string` (not float/bool) | Yes | Yes | Yes | Yes (property and parameter literal) |
| `uint8` (byte) | Yes | Yes | Yes | Yes | Yes | Yes | Yes | Yes | Yes | Yes (literal 0–255) |
| FName (`UENameValue`) | Yes | Yes | Yes | Yes | Yes | Yes | Yes | Yes | Yes | Property only |
| FText (`UETextValue`) | Yes | Yes | Yes | Yes | Yes | No | Yes | Yes | Yes | Yes (`ue_text`/`ue_text_invariant` literal) |
| `UEVector2D`, `UEVector`, `UERotator`, `UETransform` | Yes | Yes | Yes | Yes | Yes | No | Yes | Yes | Yes | Property only |
| Enum (native or authored, concrete) | Yes (byte storage) | Yes | Yes | Yes | Yes | Yes | Yes | Yes, exact type | Yes, exact type | Property only (named member) |
| Value struct (native or authored) | Yes, supported fields | Yes | Yes | Yes | Yes | No | Yes | Yes, registered layout | Yes, registered layout | Property only |
| Object / interface reference | Yes | Yes | Yes | Yes | Yes | Objects via `UEObjectKey`; not interfaces | Yes | Yes | Yes | Asset paths only (null or top-level asset) |
| Class reference | Yes | Yes | Yes | Yes | Limited (see notes) | No | No | No | No | Yes (`{"class":...}` constrained by MetaClass) |
| Soft object / soft class | Yes | Yes | Yes | Yes | Yes | No | Yes | No | No | Empty, or literal in standalone array/map properties |
| `array` / `table` (map, set) | Yes | Yes | Yes | Yes | No nesting | No nesting | No nesting | Supported carrier shapes | Authored class callbacks | Yes |
| Delegate / event | Authored delegate fields, multicast event properties | Single delegates | No | Single delegates | No | No | No | Subscription API, not a payload | No | No |
| Nested containers, fixed C++ arrays, raw pointers | No | No | No | No | No | No | No | No | No | No |

### Notes by position

- **Fields.** Become real reflected `UPROPERTY`s with normal Blueprint pins, serialization and cook rules. Writes through `self` validate type and `BlueprintReadOnly`. See [Structs](../Guide/DSL/Structs.md), [Containers](../Guide/DSL/Containers.md), [Self access](../Guide/DSL/SelfAccess.md).
- **Parameters and returns.** Up to 16 parameters per method or delegate frame. `var x:T&` is inout, `@ue_out var x:T&` is output-only; const references with direction markers are rejected. Output-only values start as the type's default (zero/empty, null reference, first valid enum member, struct default, zero vector/rotator, identity transform). See [Functions](../Guide/DSL/Functions.md).
- **Parameter defaults.** Only trailing input literals on authored methods: `int`, `float`, `double`, `bool`, `string`, plus byte (`uint8(n)`) and text (`ue_loctext(...)`, `ue_text_invariant(...)`) literals. Out/ref, `WorldContext`, complex types and computed expressions are rejected; authored interface stubs have no defaults. See [Defaults](../Guide/DSL/Defaults.md).
- **Property defaults.** Use one recursive materializer for scalars, names, text, enums, math, structs, assets, classes and containers (array/set/map literals, `ue_defaults()` blocks and audited computed scalar leaves). Persisted object references must be top-level assets or classes, not actors or subobjects. Nonempty soft-reference members inside authored structs are rejected. Shared limits: depth 16, 8192 typed nodes, 4096 container entries, 16 MiB, 1 MiB input JSON.
- **Hash keys / set elements.** Restricted to byte, int, string, FName, supported enums and object keys. Float, bool, FText, struct, math, class, soft and interface keys are rejected. FName keys follow Unreal equality (case-insensitive). Containers inside an authored `ue_struct` use a narrower key policy (int, string, `uint8`); Actor class fields support all key families.
- **Class references.** Supported for class fields, method parameters and results, and inside generated struct arrays of native API calls. Containers of class references, class references in the authored interface DSL, delegates, events and timers are not supported.
- **Delegates.** Single delegates allow return/ref/out in their own signature; multicast signatures return void and allow in/ref, not out-only. Delegate values cannot be nested in delegate signatures or passed by ref/out. See [Delegates and events](../Guide/DSL/DelegatesAndEvents.md).
- **Event payloads.** Native `ue_event` subscriptions accept by-value and native const-ref parameters; mutable native ref/out subscriptions are not supported. Native struct payloads must come from the canonical generated registry. See [Callback arguments](../Guide/Gameplay/CallbackArguments.md).
- **Timer arguments and queued callbacks.** Return void and take 0–16 by-value arguments, without ref/out or defaults. Scalar timer overloads also work with ordinary exported functions; automatic array packing is done for authored class callbacks. Invalid replacement is rejected before the existing timer is removed. See [Timers](../Guide/Gameplay/Timers.md).
- **Payload validation.** Events and timers share the same owning-copy rules: exact snapshot identity, finite numbers and math, text and array budgets, and object lifetime checked before delivery.

## Generated native API

| Value kind | [`ue_generated`](GeneratedBindings.md) | [Native class bindings](NativeClassBindings.md) | [API packages](../Api/README.md) | [Frame transport (developer)](../Dev/Runtime/LowLevelCalls.md) |
| --- | --- | --- | --- | --- |
| Scalars, FString | Yes | Yes | Yes | Yes |
| `uint8` | — | Yes (0..255 checked) | Yes | Yes (0..255 checked) |
| FName | `UEName` | `UENameValue` | `UEName` | Yes |
| FText | — | `UETextValue` | `UETextValue` | — |
| Enum | `E_*` enums | `[ue_native_enum]` types (0..254) | `E_*` enums | by symbol |
| Vector / Rotator / Transform | `S_*` structs | canonical math | canonical math | field paths |
| Other value structs | `S_*` structs | supported generated structs | `S_*` structs | field paths |
| Object reference | `C_*` views | typed refs, automatic upcasts | `C_*` views + typedefs | `UObjectHandle` |
| Class reference | — | Yes | Yes | Yes |
| Soft object / class | — | Yes | No generic ABI (dedicated adapters only) | — |
| Weak object fields | — | — | Yes (no strong pin) | — |
| `TArray` | Yes | Yes | Yes | Yes |
| `TMap` / `TSet` | `H_*` wrappers (scalar/enum) | `table` (scalar/name/enum/object keys) | Scalar/enum shapes | Yes |
| Default arguments | Typed, from `CPP_Default` | Trailing scalars and enums | None (explicit) | None |

Opaque engine values with dedicated transports in packages: `UEHitResult`, `UEConstraintHandle`, Geometry, input events, `UEEventReply`, PaintContext, WidgetAnimation handles and `InputActionValue`. See the [API package pages](../Api/README.md).

Not supported anywhere: nested containers, fixed C++ arrays, raw pointers, arbitrary C++ types without reflection, interface keys and weak-object containers.

See also: [Limitations](Limitations.md) · [Built-in types](../Guide/DSL/BuiltinTypes.md) · [Enums](../Guide/DSL/Enums.md) · [Soft references](../Guide/DSL/SoftReferences.md) · [Struct snapshots](StructSnapshots.md)
