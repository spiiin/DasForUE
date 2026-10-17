# Enum and registered value structs in events and timers

Authored class timer callbacks and `ue_event` subscriptions now accept concrete
native/authored enum and struct types. Names resolve through the existing explicit
type folder/registry or native type annotations. A raw untyped `UEStructValue` or
`UEEnumValue` callback declaration is not a substitute for that concrete identity.

```das
options gen2
require game_value_types

[ue_class(type_registry="/Game/GameTypes/Registry")]
class Notices : Actor {
    def BeginPlay():void {
        var message = GameMessage()
        message.Score=17
        ue_timer_start("message",1.0f,"Remember",true,message)
        ue_timer_next_tick("state","ChangeState",GameState.Active)
        message.Score=99
    }
    [ue_function(BlueprintCallable)]
    def Remember(value:GameMessage):void {
        // Each repeat receives an independent native snapshot of Score=17.
    }
    [ue_function(BlueprintCallable)]
    def ChangeState(value:GameState):void {}
    [ue_event(name="MessageChanged")]
    def OnMessageChanged(value:GameMessage):void {}
}
```

The example assumes registered `GameMessage`/`GameState` definitions and a matching
BlueprintAssignable multicast `MessageChanged` on the source. Callbacks return void
and take one by-value argument; `var` permits local mutation, without `&` or out/ref.
Timers support both the existing interval/repeat form and next-tick form. Ordinary
exported callbacks can use the low-level enum overload; typed source conversion and
struct packing are generated for authored classes.

Enums retain their exact UEnum identity and member number, rather than becoming
untyped bytes. Only the existing byte-sized enum domain (members 0..254) is supported.
Wrong types or undefined members are rejected. Structs retain their exact loaded
UScriptStruct identity and an owned native FInstancedStruct copy. Nested supported
value fields, enums, strings, FText and containers use the existing snapshot service.
Caller mutation and mutation of one delivery do not alter later repeats. FText keeps
its native localization history.

Queued structs now also support reflected object/interface references with owned
GC traversal and lifetime revalidation; see [ManagedCallbackObjects.md](ManagedCallbackObjects.md).
Class/delegate references, unsupported fields, fixed arrays and opaque/recursive
layouts remain rejected. Element/byte/text/string budgets apply recursively;
numeric fields must be finite.

For native events, the source parameter may be by-value or native `const ref`.
Mutable native ref/out parameters are rejected for these subscriptions. This does
not change the separate delegate Execute/Broadcast ref/out semantics. The source
must have exactly the callback's enum/struct type; structural similarity is insufficient.

## Runtime implementation

The shared callback table has concrete enum and struct policies. Serialized event
declarations include `parameter_type_path` alongside `parameter_kind`, so schema
comparison and subscription collection retain the concrete type. Generated event
wrappers unpack through the same enum/struct conversions as ordinary methods.

A generic native thunk copies the source's reflected argument into native transport.
Receiver UFunctions duplicate the source delegate parameter layout, then use that
thunk. They are cached by compatible reflected signature on the internal proxy class;
enum/struct identities and ref flags participate in signature compatibility. No C++
handler has to be added for each new struct. The cache is capped at 1024 signatures
and is removed during runtime shutdown after subscriptions and contexts are cleared.
Receiver functions are explicitly rooted: a cooked native proxy class can be in
the permanent DisregardForGC pool, which cannot reference merely strong dynamic
objects. Removal first unregisters each function, then removes its root.
Runtime shutdown also runs on OnPreExit, before UObject storage is destroyed;
the later module shutdown is idempotent. This ordering is required for cooked
forced-GC and normal process exit.
The clone is linked before use, retains out/ref frame flags, clears inherited
Blueprint bytecode/fastcall data and uses exact physical signature compatibility.

Native enum-class delegate properties now use their canonical FEnumProperty/byte
underlying storage, matching Kismet, rather than forcing a byte-enum property.
Explicit byte storage and legacy enums remain supported. Existing assets with the
previous native enum delegate layout require normal DSL reimport.

Timer registration validates payload and callback type before replacing a named
timer. Event collection verifies the exact source type. Native event delivery and
timers both use `ValidateDasCallbackPayload`; release, reload and EndPlay retain the
existing cancellation and detachment rules.

Fixtures: `ue_callback_value_types.das`, `ue_callback_value_signals.das` and
`dsl_text_byte_timers.das`. The latter keeps its existing filename and saved fixture
identity while exercising the expanded callback matrix.

## Validation — 2026-10-17

Editor and Win64 Development game builds passed. Real gameplay PIE reported
1 success and 0 warnings, failures or skipped tests. Saved fixture export, packaged
foundation (0 failures), cooked gameplay and the playable map passed under NullRHI.
Packaged runs completed with exit code 0, including normal runtime shutdown.
Shipping and physical rendering were not tested.

The gameplay fixture exercises authored/native enum and struct types, native const
refs, interval/repeat and next-tick delivery, localized FText and nested array copy
isolation, pending captures, listener detachment and native broadcasts after release.
Negative fixtures reject reference-bearing structs. The shared probe covers unknown
enum members, missing and mismatched types, mutable native ref sources, nested
nonfinite values, signature layout and cache reuse. Packaged foundation performs
forced GC after creating generic receiver signatures; GC verification remains enabled.

The initial failures exposed inherited Blueprint bytecode/unlinked cloned function
layout and cooked GC/root teardown requirements. These were repaired in the shared
adapter and lifecycle, without disabling checks or removing functional assertions.
Initial cooked GC/exit failures are preserved as `logs/callback-values-gc-initial.log`
and `logs/callback-values-exit-initial.log`.

Commands: `Scripts/TestGameplayRuntimePIE.ps1 -LogPrefix callback-values` and
`Scripts/TestCookedGameplay.ps1`. Final evidence: `logs/callback-values-report/index.json`,
`logs/callback-values-foundation.log`, `logs/callback-values-runtime.log`,
`logs/callback-values-map.log`, `logs/callback-values-export.log` and
`logs/callback-values-game-build.log`.
Cooked map: `/Game/DasCookedGameplay/05eb2a4db45040399f736892433e1fed/Map`.
Export/cook retains the existing missing ACL compression-settings asset warnings;
runtime checks and clean process exit passed. No commit was created for this step.
