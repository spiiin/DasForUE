# Interface calls into DAS implementations

Interface calls from DAS can enter authored DAS Actor and ActorComponent implementations,
including the current object and chains between objects. Generated receiver methods,
`ue_begin_interface_call` frames and the legacy int/string adapters share the same
dispatch scope. Native and ordinary Blueprint receivers continue to use this path.

```das
require ue_text
require ue_text_byte_interface_bindings

def UpdateExample(target:UObjectHandle):void {
    let api=TextByteContract_from(target)
    let title=api.EchoText(ue_text("MyGame","Title","Example"))
    var number:uint8=0
    var current=title
    var previous:uint8=100
    var oldTitle=ue_text_invariant("old")
    let original=api.Change(number,current,previous,oldTitle)
    var bytes:array<uint8> = [uint8(0)]
    var messages:array<FText>
    api.ChangeArrays(bytes,messages)
}
```

The receiver must implement the requested interface and have a compatible reflected
method. Signature fingerprints, function replacement and target lifetime checks run
before invocation. Inputs and outputs retain the existing frame validation and native
snapshot ownership, including localized FText and containers.

Finite recursive calls are allowed. The shared authored/interface/delegate call depth
is limited to 32: an unbounded self-call or cycle fails with a recursion-limit error.
An executing context is not restarted or replaced by hot reload during reentry.

A callee error propagates to the caller and can be caught with ordinary `try ... recover`.
The failed frame exposes no results, and generated wrappers do not copy partial ref/out
values back to caller variables. This does not roll back earlier changes to UObject
fields or other external side effects. After handling the error, later calls remain usable.

Regression fixtures cover self-recursion, two Actors, Actor/ActorComponent cycles,
depth rejection, error propagation, failed output access, recovery, legacy int/string
adapters and generated FText/uint8/array ref/out calls both in the same and in another
context. Sources: `ue_interface_reentry_contract.das`, `ue_interface_reentry_calls.das`,
`dsl_interface_reentry_actor.das`, `dsl_interface_reentry_component.das`,
`ue_interface_reentry_values.das`. The generated value wrapper is
`ue_text_byte_interface_bindings.das` (from `Scripts/GenerateBindings.py`).

Validation 2026-10-17: Editor and Win64 Development builds; strict gameplay PIE
(1 succeeded, 0 warnings/failures/skips); native-interface Create/Reopen probes;
packaged foundation (0 failures), gameplay and playable-map smoke passed under
NullRHI. The legacy nested-struct/indexed-write probes now check native data changes
instead of requiring rejection of these already supported assignments. Shipping
and physical rendering were not tested.

Evidence: `logs/interface-reentry.log`, `logs/interface-reentry-report/index.json`,
`logs/interface-reentry-native-{Create,Reopen}.log`, `logs/interface-reentry-runtime.log`,
`logs/interface-reentry-foundation.log`, `logs/interface-reentry-map.log` and
`logs/interface-reentry-cooked-driver.log`. Packaged map:
`/Game/DasCookedGameplay/de7cfec6a9914cd19b5247889685b402/Map`.

The generated value-wrapper IR is `Bindings/InterfaceReentryValues.json`; its
selection is `Bindings/InterfaceReentryValuesSelection.json`. Regenerate with
`Scripts/GenerateBindings.py Bindings/InterfaceReentryValues.json Plugins/DasForUE/Content/Das/Scripts/ue_text_byte_interface_bindings.das --module ue_text_byte_interface_bindings`
(add `--check` to verify). The selection can be re-exported with DasExport after
the text/byte fixture interface has been created and saved.
