# Existing native interfaces on DSL classes

Declare native interfaces with ue_class(implements="...") or repeatable
`ue_implements(interface="/Script/...")` annotations. Authored interfaces also
accept short names and stable IDs; see DSLAuthoredInterfaces.md. Existing syntax:

```das
[ue_class(module_id="sample",
          implements="/Script/DasRuntime.DasProbeInterface", Blueprintable)]
class InterfaceActor {
    [ue_function(BlueprintCallable)]
    def Query(Input:int):int { return Input + 7 }
    // Implement all other interface methods with their exact native signatures.
}
```

The complete example is Plugins/DasForUE/Content/Das/Scripts/dsl_interfaces.das.
Several interface paths may be comma-separated, up to 16. Interface identities
are native /Script paths in the authoring IR, retained by JSON roundtrip.
Validation checks that each path is a loaded UInterface, that every reflected
interface method has an implementation, and that parameter names/types/directions
and the return type match exactly. Missing or incompatible declarations reject
before native Blueprint mutation. Ordinary UObject methods inherited by an
interface are excluded from this contract.

The Blueprint backend registers FBPInterfaceDescription and creates actual
interface function graphs using Unreal's interface graph terminators. Generated
classes pass ImplementsInterface; C++ Execute_* and ProcessEvent route to the DSL
body. Existing owned frame transport handles arrays and explicit out/ref values.
Definitions, graph links and membership survive save/reopen and cooking. Logic
reimport retains the existing interface contract. Changing interface membership
on an existing authored class is explicitly deferred.

## daScript membership and dispatch

- ue_implements_interface(object,path): false for null/nonimplementers; an invalid
  or noninterface class path rejects.
- ue_interface_call_int(object,path,method,input): checked int -> int dispatch.
- ue_interface_call_string(object,path,method): checked zero-input string getter.

Objects remain context-owned weak UObjectHandle capabilities; native interface
pointers are not exposed. Dispatch verifies interface membership, declared method
existence and compatibility of the reflected implementation, then uses ProcessEvent.
Ordinary Blueprint implementations are supported by these two dispatch adapters.
Blueprint -> DSL and C++ Execute_* -> DSL work outside an active daScript callback.
Dispatch back into a daScript Actor from an already active VM callback rejects
explicitly; the current runtime has no reentrant callback backend.

This first stage does not author new UInterface types, interface properties or
TScriptInterface values. Arbitrary native C++ Execute_* fallback cannot be invoked
through generic reflection when a target has no reflected implementation.
Generic typed/interface binding generation is later work. Event-only interface
methods (void with no outputs), pure/static/latent/RPC methods and structural
interface membership changes remain outside this function-graph backend.

Validation: Scripts/TestDSLTypeRegistry.ps1 -Interfaces -Cooked.
Next: broader typed interface dispatch and authored interface definitions.
Migration development remains deferred.


## Interface-constrained typed frames

`ue_begin_interface_call(object, interfacePath, method)` returns the same
UCallHandle used by the existing typed marshalling API. It verifies membership,
the method's declaration on the requested interface and compatibility of the
reflected implementation before allocating a frame. The frame retains a weak
interface identity and the declared signature fingerprint. Invocation checks
membership and the declared/actual function signatures again.

```das
let call = ue_begin_interface_call(target,
    "/Script/DasRuntime.DasProbeInterface", "Stats")
ue_arg_int(call, "Counter", 10) // inout input
ue_invoke_call(call)
let changed = ue_result_int(call, "Counter")
let output = ue_result_int(call, "Total")
ue_release_call(call)
```

Reuse ue_arg_* / ue_result_* for scalar leaves, reflected structs, arrays,
map/set and input/out/inout directions within the existing marshaller's supported
type set. Several arguments are supported; every required input must be assigned.
Top-level array/set element paths use `Values.0`; map paths use
`Values.0.key` / `Values.0.value`. Results belong to the call frame; copy strings
or container elements into owned daScript values before releasing it.

The normal call-frame limits, one-shot invocation, context ownership, expired
targets, failed-output rejection and release behavior apply. Input containers
are copied into the native frame; native memory/interface pointers do not escape.
See Marshalling.md, InOut.md, Arrays.md and HashContainers.md for supported types
and detailed limits. These frame limits are distinct from owned DSL snapshot limits.

Validated interface examples cover int input/return, double arrays, several
out/ref parameters, Unicode map values and int sets, including packaged dispatch
to ordinary Blueprint receivers. The existing reentrant-VM and native Execute_*
fallback limitations remain unchanged. This extends checked dispatch rather than
generating new typed wrapper source automatically.

Next: author new interfaces from DSL definitions; expand generated interface
wrappers as a later integration with the binding generator.


Void/no-output native interface events are now supported through owned Event Graph
nodes. See DSLInterfaceEvents.md. Return/out/ref interface methods keep the existing
function graph backend; VM reentry and unsupported flags remain separate work.
