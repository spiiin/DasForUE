# Extended authoring out/ref

Mutable `var value:T&` parameters mean ref/inout. `@ue_out var value:T&` means
output only. These declarations now support checked object/interface wrappers,
direct authored enum/struct values, raw constrained UEEnumValue/UEStructValue,
and UEVector/UERotator/UETransform, in addition to existing primitives/containers.

```das
[ue_function(BlueprintCallable)]
def RefState(var value:InterfaceState&;
             @ue_out var previous:InterfaceState&):void {
    previous = value
    value = InterfaceState.Idle
}

[ue_function(BlueprintCallable)]
def RefSettings(var value:InterfaceSettings&;
                @ue_out var previous:InterfaceSettings&):void {
    previous <- clone(value)
    value.Count += 3
}

[ue_function(BlueprintCallable)]
def RefActor(var value:ActorRef&; @ue_out var previous:ActorRef&):void {
    previous = value
    value = default<ActorRef>
}
```

Interface definitions use the same annotated empty methods. Object/interface
class identity and direct enum/struct identity are inferred from their wrapper/
registry declarations. Legacy raw enum/struct handles still require explicit type
constraints. No CSV parameter lists or reader macro are introduced.

Output-only defaults are valid for their declared type: null object/interface,
first valid enum member (no zero-value assumption), loaded struct defaults, zero
vector/rotator and identity transform. Inout values are copied into independent
working slots before script execution. Nested arrays/maps/sets remain owned.

The generated wrapper publishes each declared output using a kind-specific runtime
adapter. Validation checks object/interface membership, enum identity/value,
snapshot type/bounds, finite math components and normalized transforms. Pending
object outputs are pinned; pending struct snapshots report their GC references.
Publication succeeds only after the implementation returns normally. An error
exposes no runtime output set.

Blueprint enum/interface inputs preserve their direction through wildcard frame
packing. Output graphs use checked enum/interface unpacking and object casts;
struct/math values preserve their native property types. If runtime execution
fails, the Blueprint frame adapter aborts execution before caller copyback.
Direct C++ callers must check the returned success flag before using outputs.

Source binding facades convert all mapped outputs to fresh source values before
mutating caller arguments. Independent output slots remain the model; passing the
same source variable to multiple slots uses declaration-order copyback, not native
pointer aliasing. UObject identity is shared, while struct/container storage is
independent.

The executable fixtures include RefActor, RefEvents, RefState, RefSettings,
RefVector, RefRotator and RefTransform. Negative fixtures cover a wrong enum type,
a wrong struct snapshot and an implementation panic after local mutations.
See DSLExtendedOutRefValidation.md for verification.

Structural migrations and VM reentry remain deferred. Container element shapes
still follow their existing authoring restrictions; this stage does not add new
interface/object/enum array or hash schemas.
