# FText and uint8 containers in authored DSL

The normal DAS container syntax works for fields, method inputs, return values
and mutable ref/out parameters. Byte values retain the builtin `uint8` type;
FText values retain native Unreal text history.

| Shape | Supported |
| --- | --- |
| `array<uint8>`, `array<FText>` | Yes |
| `table<uint8>` (set) | Yes, including 0 and 255 |
| Map values `uint8` or `FText` | Existing int/string/FName/enum/object keys and uint8 keys |
| `table<uint8; T>` | Existing scalar/name/text/byte/enum/object/struct values |
| `table<FText>` or `table<FText; T>` | Rejected; FText has no supported Unreal key hash |
| Nested containers | Still rejected |

Authored `ue_struct` keeps its existing narrower key policy: int, string and
now uint8. Actor class fields support the complete key families above.

```das
require ue_text

[ue_class(Blueprintable)]
class Example : Actor {
    properties=ue_properties(EditAnywhere,BlueprintReadWrite) {
        var Weights:array<uint8> = [uint8(0),uint8(255)]
        var Messages:array<FText> = [ue_text("Game","Welcome","Welcome")]
        var Labels:table<uint8;FText>
        var ByteTags:table<uint8>
    }

    [ue_function(BlueprintCallable)]
    def Append(var values:array<uint8>&; @ue_out var labels:array<FText>&):void {
        push(values,uint8(255))
        push(labels,ue_text("Game","Result","Done"))
    }
}
```

Container property assignment uses the existing clone operation: `actor.Weights
:= values`. Returned containers are owned snapshots (`var copy <- actor.Weights`).
Editing a local snapshot does not mutate the actor until the property is assigned
again. Direct indexed writes and push/insert/erase use the existing clone-and-commit accessors too. Struct and object values keep their existing typed adapters.

Literal byte defaults are range checked before narrowing. Literal text defaults
use `ue_text`/`ue_text_invariant` and persist typed JSON text nodes containing
Unreal text serialization. They never persist VM tokens or flatten FText to a
display string. Defaults compare namespace/key/source and invariant status in
arrays and map values, consistently in Editor and cooked game. Byte-backed enums
remain nominal enum values and cannot masquerade as raw byte containers.

No new computed-default, delegate, event, network or migration behavior is
included here. Existing container limits and reference liveness checks apply.

Examples are `dsl_text_byte_containers.das`, `ue_text_byte_container_types.das`
and `dsl_text_byte_containers_calls.das` under the plugin's Content/Das/Scripts.
The shared gameplay probe checks all key families, 0/255, source/invariant text,
struct containers, direct accessors, method input/return/ref/out and repeated calls
after context release. Negative fixtures reject FText keys/sets and nesting.

Validation 2026-10-17: final checks ran in C:/src/DasForUE2 after merging the
completed shared-default decoder work. Editor and Win64 Development builds,
strict gameplay PIE (1 pass, 0 warnings/failures/skips), packaged build/cook/stage/
archive, foundation (0 failures), gameplay and saved playable-map launch passed
under NullRHI. This includes new container shapes and existing gameplay APIs.
Byte arrays, maps and sets preserve 0/255; localized/invariant text and nonempty
array/set/map defaults survive serialization; typed fields, struct snapshots,
method input/return/ref/out, direct index/push/insert/erase operations and repeated
calls after context release pass. Three negative DSL declarations reject text
keys, text sets and nested containers. Native generated APIs were not expanded.
Packaged map: `/Game/DasCookedGameplay/6d1b431974cf4ec588d3764d3e73aee1/Map`. Evidence: `logs/text-byte-containers-*`.
