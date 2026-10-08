# Explicit DSL out/ref parameters

Use parameter-local metadata and native mutable references:

```das
[ue_function(BlueprintCallable)]
def Adjust(var counter:int&; @ue_out var total:int&; amount:int):int {
    counter += amount
    total += counter * 2
    return counter
}
```

An unmarked mutable `var ... &` parameter now means inout: copy the native value
in, then copy its updated value back. `@ue_ref var counter:int&` is an optional
explicit spelling of the same contract. `@ue_out var total:int&` means output-only:
initialize to zero/false/empty/NAME_None, ignore previous caller data, then publish
the assigned value. Ordinary value parameters remain input-only. Const references
and direction markers on non-reference values reject.

Class methods and ordinary empty interface declaration methods share these rules.
Legacy abstract interface declarations retain their earlier direction behavior.
Parameter metadata is read from Variable.annotation; no parser changes or reader
macros are required. Other compiler metadata is left to daScript; unknown ue_*
parameter metadata rejects to catch spelling mistakes.

The older ue_function(ref=(counter,weights), out=(total,message)) lists and CSV
strings remain supported. A list direction agrees with the same explicit
parameter marker or overrides the unmarked default; disagreement rejects.
Both @ue_ref and @ue_out on one parameter, repeated markers, false/non-boolean
markers, unknown list names, overlap and missing mutable reference types reject.
Defaults remain unsupported on reflected parameters.

The first milestone supports int/float/double/bool/string/UENameValue and all
20 standalone container shapes from DSLContainerCalls.md. Several directions,
containers and ordinary inputs can share the existing 16-slot method frame.
Void and supported normal return values may accompany the output parameters.
Objects, enums, authored/native structs and math structs as explicit out/ref
parameters remain future extensions; those types retain their previous input/
return support. Pure/static authoring methods remain outside this stage.

## Native and VM behavior

Blueprint has native output pins for out arguments and native reference input
pins for ref arguments. Native properties have CPF_OutParm for out and
CPF_OutParm|CPF_ReferenceParm for ref. The implementation wrapper takes owned
input values, creates mutable local values, invokes the source method and packs
its output slots. No VM reference aliases native property storage.

Outputs are committed together only after successful execution and packing.
Runtime failure, wrong direction contract or a late packing/limit failure returns
no OutputValues. Blueprint copy-back preserves original ref values and uses
defaults for out values on failure. This covers parameter outputs; arbitrary
earlier script side effects on Actor fields are not a transaction.

Reference aliasing between distinct slots is not preserved: slots use independent
locals, and native copy-back follows declaration order. Container results remain
owned copies, with the existing collision and 4096-element/fixed-byte limits.

The native typed API uses FDasScalarValue.Direction (0=in, 1=out, 2=inout).
Successful FDasExecutionResult.OutputValues contains the full frame in declaration
order, including unchanged input slots. Failed calls expose no output frame.
Direction metadata is checked against the DSL contract before body execution.
Ordinary in-only functions keep their existing adapter path and empty OutputValues.

Example: Plugins/DasForUE/Content/Das/Scripts/dsl_out_ref.das. Validation:
Scripts/TestDSLTypeRegistry.ps1 -OutRef -Cooked. The fixture covers six primitive
and 20 container ref/out pairs, mixed calls, ProcessEvent and failure copy-back.
Next: interfaces. Migration development remains deferred.
