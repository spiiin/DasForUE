# Authored FText and uint8

Require `ue_text` for the `FText` alias and text constructors. `uint8` is the
built-in daScript unsigned byte, reflected as a raw Unreal `FByteProperty`
(without an enum). `FText` aliases `UETextValue` and reflects as `FTextProperty`.

```das
require ue_text

[ue_class(Blueprintable)]
class Example : Actor {
    properties = ue_properties(EditAnywhere, BlueprintReadWrite) {
        var Priority:uint8 = uint8(255)
        var Title:FText = ue_text("MyGame", "ExampleTitle", "Example")
    }

    [ue_function(BlueprintCallable)]
    def SetTitle(value:FText):FText {
        self.Title=value
        return self.Title
    }

    [ue_function(BlueprintCallable)]
    def Update(var priority:uint8&; @ue_out var title:FText&) {
        priority=uint8(42)
        title=self.Title
    }
}
```

Both types support authored properties, `ue_struct` fields, method parameters,
returns and mutable `ref`/`out` parameters. Typed object accessors work from
ordinary DAS functions too: `actor.Priority=uint8(7)` and `actor.Title=value`.
Authored byte signatures keep `uint8`; existing generated native byte APIs
retain their checked `int` ABI.

Byte defaults accept integer literals or `uint8(integer literal)` in 0–255.
Declarations reject out-of-range defaults before conversion. Explicit runtime
`uint8(...)` conversions retain ordinary daScript narrowing semantics.

Text defaults accept literal `ue_text(namespace,key,source)` or
`ue_text_invariant(source)`. Localized namespace/key must be nonempty. Runtime
values retain Unreal's native FText history; class/struct/function defaults
persist as `NSLOCTEXT(...)` or `INVTEXT(...)`. Blueprint frames carry an owned
native FText, never an interpreter token or a display-only string. Use
`ue_text_to_string` only when a display string is actually needed. Translation
catalog gathering is a separate localization workflow; these declarations do
not automatically create translations.

Containers of these types are now supported; see DSLTextByteContainers.md.
Arbitrary computed text/byte defaults remain a separate step. Existing native
text and enum APIs retain their existing ABI.

Examples: `dsl_text_byte.das` and `dsl_text_byte_calls.das` in the plugin's
`Content/Das/Scripts` directory. The gameplay PIE and packaged Development
probes include byte boundary roundtrips, typed accessors, struct leaves,
localized default persistence, text identity and ref/out copyback.

Validation: Editor and Win64 Development builds, strict gameplay PIE (one pass,
zero warnings/failures/skips), native generator artifact check and packaged
foundation/gameplay/playable-map smoke all passed under NullRHI. Four negative
declarations cover overflowing/negative byte defaults, string-as-text and missing
localization identity. Text defaults compare localization history rather than
Editor display-string equality or cooked pointer identity. Stable evidence is
stored in `logs/authored-values-*`; no Shipping or physical rendering test is
claimed.
