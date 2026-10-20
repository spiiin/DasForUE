# Low-Level Object Handles and Call Frames

`ue_foundation` exposes an untyped fallback for calling any supported reflected `UFUNCTION` by name: `UObjectHandle` for objects and `UCallHandle` call frames for arguments and results. Generated wrappers, native class bindings and API packages are built on top of this layer; use it directly only when no typed API exists yet.

## Object handles

`UObjectHandle` is an opaque, copyable value holding a weak reference through the current context's registry. It is not a raw pointer or an integer, and scripts need no `unsafe`.

| API | Purpose |
| --- | --- |
| `ue_null_object()` | Empty handle (also for initializing globals) |
| `ue_self_object()` | Handle of the object that owns the script |
| `ue_get_self_object(field)` / `ue_get_object(handle, field)` | Read a Blueprint-visible strong object `UPROPERTY` (chainable) |
| `ue_object_valid(handle)` | Object exists and still matches the expected class |
| `ue_object_is_a(handle, class_path)` | Inheritance test |
| `ue_cast_object(handle, class_path)` | Checked cast that keeps the expected class |
| `ue_cast_nullable_object(handle, class_path)` | Same, but null stays null |
| `ue_object_get_int(handle, field)` / `ue_object_set_int(handle, field, value)` | Blueprint-visible `int32` property (write honours `BlueprintReadOnly`) |

Classes are given by full path, e.g. `/Script/Engine.Actor`, and must already be loaded; these APIs never load assets.

```das
var target = ue_null_object()

[export]
def begin_play() {
    target = ue_cast_object(ue_get_object(ue_self_object(), "Peer"), "/Script/Engine.Actor")
}
```

Lifetime rules:

- A handle does not keep the object alive. An Actor counts as unavailable as soon as `Destroy` starts, even before GC.
- Using a null, expired or foreign handle is a daScript error; `ue_object_valid` returns `false`. An expired non-null handle never silently becomes `nullptr`.
- Null is allowed as an object argument.
- After a logic reload the handle registry is recreated: re-acquire global handles (for example in the next callback). Structural reinstancing does not redirect old handles to the new instance.

## Fixed-signature adapters

The oldest adapters cover five signatures and remain supported:

| API | UE signature |
| --- | --- |
| `ue_call_int(object, name, value)` | `int32 → int32` |
| `ue_call_float(object, name, value)` | `float → float` |
| `ue_call_void_int(object, name, value)` | `int32 → void` |
| `ue_call_object(object, name)` | `() → UObject*` |
| `ue_call_void_object(object, name, argument)` | `UObject* → void` |

```das
let self = ue_self_object()
let actor = ue_cast_object(ue_get_object(self, "Peer"), "/Script/Engine.Actor")
ue_call_void_object(self, "SetOwner", actor)
let owner = ue_call_object(self, "GetOwner")
verify(ue_object_is_a(owner, "/Script/Engine.Actor"))
```

Use `verify` (or compute into a local first) for checks with side effects: daScript forbids side effects inside `assert`.

## General call frames

```das
options gen2
require ue_foundation

[export]
def main(input:int):int {
    let call = ue_begin_call(ue_self_object(), "K2_GetActorLocation")
    ue_invoke_call(call)
    let x = ue_result_double(call, "ReturnValue.X")
    ue_release_call(call)
    return int(x)
}
```

| API | Purpose |
| --- | --- |
| `ue_begin_call(object, name)` | Validate the whole signature and create a `UCallHandle` |
| `ue_arg_int/float/double/bool/string/byte/name/object/class(call, path, value)` | Set an input leaf |
| `ue_arg_enum(call, path, enum_type_path, symbol)` | Set an enum input by symbol |
| `ue_arg_array(call, path, size)` / `ue_arg_hash(call, path, count)` | Create or reset an input array / map / set |
| `ue_invoke_call(call)` | Check that every input is filled, then `ProcessEvent` once |
| `ue_result_int/float/double/bool/string/byte/name/object/class(call, path)` | Read a return/out/inout leaf |
| `ue_result_enum(call, path, enum_type_path)` | Read an enum result as its symbol |
| `ue_result_array_size(call, path)` / `ue_result_hash_size(call, path)` | Size of an output container |
| `ue_release_call(call)` | Free the frame; every copy of the handle becomes invalid |

Calls go through `ProcessEvent`, so native and Blueprint functions both work. Paths address scalar leaves: nested struct fields use dots (`Payload.Metrics.Ratio`), array elements use indices (`values.0`, `values.1.metrics.ratio`), map slots `values.0.key` / `values.0.value`, set slots `values.0`. Struct shapes come from the concrete `UFunction`; foreign struct memory cannot be passed and types are never matched by size.

### Parameter directions

| UE parameter | Set before invoke | Read after invoke |
| --- | --- | --- |
| Input / const reference | Yes | No |
| Pure `out` | No | Yes |
| Mutable reference, including `UPARAM(ref)` | Yes | Yes |
| `ReturnValue` | No | Yes |

Every input leaf (including all fields of an inout struct and every element after the last array reset) must be set explicitly; there is no automatic C++ default. An empty array needs an explicit size of 0. Writing an out/return, reading a plain input, or leaving an inout unset is an error.

```das
let call = ue_begin_call(ue_self_object(), "GetOwner")
ue_invoke_call(call)
let owner = ue_result_object(call, "ReturnValue")
let set = ue_begin_call(ue_self_object(), "SetOwner")
ue_arg_object(set, "NewOwner", owner)
ue_release_call(call)
ue_invoke_call(set)
ue_release_call(set)
```

### Value rules

- Exact scalar types: `ue_arg_int` does not set an enum or a byte, `ue_arg_string` does not set an FName and vice versa.
- Enums: the type path must match the field's own `UEnum`. Short or qualified symbols are compared case-insensitively; display names, numbers, redirects and bit combinations are not accepted. `_MAX` symbols are rejected. An undeclared value returned by UE fails when read.
- FName: `ue_arg_name` creates the name in the frame; `ue_result_name` returns a daScript copy of `ToString()`. Empty text and `None` mean `NAME_None`; names of `NAME_SIZE` or more characters are rejected.
- Bool is read through `FBoolProperty`, including bitfields; strings are copied UTF-8 ↔ FString.
- Object inputs must match `FObjectProperty::PropertyClass`; `ue_result_object` returns a weak handle with the parameter's class, null for a null or destroyed object.
- Containers: at most 65,536 elements per array/map/set. Duplicate map keys/set elements and non-finite float keys are rejected before the call.

### Lifetime and errors

- The frame owns parameter memory (`FStructOnScope`), including strings and nested structs. String and name results are copied and stay valid after release.
- The target is held weakly and pinned only during `ProcessEvent`. Object inputs and object outputs are pinned by the frame until release or the end of the current script execution, so GC inside the called function is safe.
- Frames live only within the current script execution or callback. On exit, including errors, all remaining frames are released. Do not keep them across ticks or reload. At most 256 frames may be alive at once; release frames in long loops.
- Incomplete inputs, wrong types, unknown names and unsupported signatures fail before `ProcessEvent`. Reading a wrong result type, an oversized output or an invalid enum fails after the call; side effects of the executed function are not rolled back. A frame whose output failed validation is marked failed and its results cannot be read, even after `try/recover`; release it and create a new one. Invoking twice is an error.
- Construction-unsafe functions: generated code passes its policy with `ue_require_call_context(call, construction_unsafe)`; such calls are rejected in a ConstructionScript before dispatch. A low-level call with no policy and no available metadata is rejected in ConstructionScript. This is an execution guard, not a security sandbox.
- Preparing or invoking calls inside `bind_events` is forbidden. Calls are synchronous, on the game thread.

## Legacy object-source subscription

`ue_subscribe_object_int(source, "OnDasSignal", "on_signal")` subscribes an exported function to a `BlueprintAssignable` multicast event of any object reachable through a handle. Prefer `ue_event` and authored delegates ([Delegates and events](../../Guide/DSL/DelegatesAndEvents.md)).

## Examples

`Plugins/DasForUE/Content/Das/Scripts/actor_location.das` (call on an Actor owner). `object_calls.das`, `marshalling.das`, `inout_values.das` and `enum_names.das` in the same folder demonstrate every feature but require the test `UDasObjectFixture`.

## Limitations

- Calling back into daScript from a `UFunction` invoked through a low-level frame is not supported; ordinary native and Blueprint functions work.
- Not supported here: RPC and latent calls (static library functions are reached through generated wrappers and [function libraries](../../Guide/DSL/FunctionLibraries.md)), opaque native struct state (structs with `AddStructReferencedObjects` are rejected), weak/soft/interface object getters, structural reload during a synchronous `ProcessEvent`.
- Type checks happen at runtime; use [generated bindings](../../Reference/GeneratedBindings.md) or [native class bindings](../../Reference/NativeClassBindings.md) for compile-time typing. These externs are not AOT-compiled.

See also: [Generated bindings](../../Reference/GeneratedBindings.md) · [Native class bindings](../../Reference/NativeClassBindings.md) · [Object and class references](../../Guide/DSL/ObjectAndClassReferences.md) · [Type support](../../Reference/TypeSupport.md)
