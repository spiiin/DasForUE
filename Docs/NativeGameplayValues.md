# Native uint8, enum defaults and FText

`require ue_native_gameplay` imports the five selected native receiver classes,
including TextRenderComponent. Manifest schema 3 allows byte/text kinds; current
selection has 323 fields, 606 methods and 35 native enums. The generated member
registry is compiled into runtime and never loaded from JSON during gameplay.

```das
require ue_native_gameplay
let movement=ue_character_self().CharacterMovement
movement.SetMovementMode(EMovementMode.MOVE_Custom,255)
movement.SetMovementMode(EMovementMode.MOVE_Custom) // NewCustomMode=0

let camera=ue_component_get(type<CameraComponentRef>,ue_ref(type<ActorRef>,ue_self_object()),"Camera")
let transform=camera.GetSocketTransform(ue_name_value("None")) // RTS_World

let text_renderer=ue_component_get(type<TextRenderComponentRef>,ue_ref(type<ActorRef>,ue_self_object()),"Label")
text_renderer.K2_SetText(ue_text("MyGame","PlayerLabel","Player"))
let shown=ue_text_to_string(text_renderer.Text)
```

## Raw uint8

Raw bytes are distinct from enum-backed FByteProperty. Source uses DAS int,
preserving readable defaults and diagnostics rather than wrapping a narrowing
cast. Inputs and writable fields reject negative numbers and values above 255
before mutation/invocation. Fields, inputs, return and mutable out/ref call
adapters use the actual reflected byte storage. Readonly metadata still applies:
CharacterMovement.CustomMovementMode is read-only; use SetMovementMode.

## Native enum defaults

CPP_Default metadata is resolved against the parameter's actual UEnum. A valid
native symbol becomes an ExprConstEnumeration with its generated source type.
No C++ expression is evaluated. Defaults must form a trailing suffix. Unknown,
hidden/reserved or unsupported metadata defaults stay explicit parameters.
This extends generated native methods, not authored ue_function default syntax.

## Text values

UETextValue is an opaque, context-local immutable snapshot of a real FText. Copies
between Unreal fields, call frames and DAS preserve FText history and localization
identity. Conversion to string is explicit and only produces a display string;
it is never used to transport native text back to Unreal.

- ue_text(namespace,key,source): keyed/localizable text; namespace/key required.
- ue_text_invariant(value): intentionally culture-invariant text.
- ue_text_to_string(value): current display string.
- ue_text_namespace(value), ue_text_key(value): current identity, or empty string.
- ue_text_is_invariant(value): culture-invariant flag.

The context owns up to 4096 distinct text snapshots, reusing identical native
text snapshots (data, revisions and flags). Zero is empty text. Tokens are never pointers; invalid/foreign
tokens are rejected. Values and their native FText storage live with the script
context; resetting/destroying it releases snapshots. This follows existing
context-bound name/enum lifetime rules and does not introduce a new GC system.
Key/source lengths are bounded. Native-provided string-table/format histories
remain native FText histories; dedicated construction/format/string-table APIs
are future work. Automatic localization gathering from DAS sources is not
implemented: keyed text still needs matching localization resources to translate.

TextRenderComponent.Text remains BlueprintReadOnly. K2_SetText is the Blueprint-callable native
mutation path. The playable Character example has a Label component and sets its
keyed text in BeginPlay. The source property is a TextRenderComponentRef; only the
native text value uses UETextValue. This stage targets native API access and
ordinary DAS values; authored reflected text fields/functions and containers
of text/byte are separate extensions.

## Verification scope

Integrated probes cover uint8 endpoints/default/range rejection, typed native
enum defaults, TextRenderComponent K2_SetText/readback, native localization identity,
invariant text, native text return/out and raw byte return via real Kismet calls.
Negative fixtures reject float-to-byte calls, string-to-FText calls and writes
to readonly Text. Generator tests cover byte/text kinds, canonical text-renderer
references and default metadata. Engine verification results are recorded in
Plan.md when complete.


Validated 2026-10-17: 13 generator tests, GenerateNativeGameplay -Check, strict
PIE (exactly 1 success, 0 warnings/failures/notRun), Win64 Development build/cook/
stage, packaged foundation (0 failures), packaged gameplay and separate saved-map
launch. Sixteen source-negative fixtures include the three new byte/text cases.
The shared probe registers native polyglot test data before testing a real
Kismet localization-table FText out result; no translations are assumed to exist
in project resources. Empty invariant text keeps its flags, and source text
remains unchanged after a native text transformation.

Logs: native-byte-text-pie.log, native-byte-text-pie-report.json,
native-byte-text-check.log, native-byte-text-cooked.log,
native-byte-text-cooked-runtime.log, native-byte-text-foundation.log,
native-byte-text-map.log. Tests use NullRHI and synthetic input; actual text
rendering, physical devices, Shipping and networking are not verified.
