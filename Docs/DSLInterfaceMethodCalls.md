# Interface method call layer

Selected native and authored interface methods now receive statically typed free
functions that daScript resolves with its normal value.Method(args) syntax. This
uses the pinned language's existing dot-call resolution, not a reader or parser patch.

```das
require ue_foundation
require ue_contract_bindings

let api = CounterContract_from(target)
let answer = api.Query(5)

var counter = 10
var total = -1
api.Stats(counter, total)
// counter and total now contain the native inout/out results.

let state = api.EchoState(InterfaceState_from_symbol("Active"))
```

The reference factory and current exported enum/struct representation are unchanged.
ue_interface<CounterContract>(target) and natural authored enum/struct conversion are
not added by this stage. Their future integration is separate from callable methods.
For native examples use DasProbeInterface_from(target), then api.Query/Label/Weights/
Stats; collection/event references have EchoMap/EchoSet and Notify/ResetCounter.
These are fixture method names, not DSL commands.

The facade calls the existing selected wrapper alias, so checked weak membership,
context ownership, class identity and declared/actual signature verification remain
the native boundary. A method requires the corresponding selected export. The
ordinary nominal receiver type disambiguates equal method names across interfaces;
the wrong receiver still fails type checking. Multiple aliases selecting the same
receiver/method produce one deterministic facade while retaining the low-level aliases.

ReturnValue becomes the method return. Every other output parameter becomes a var
mutable reference, including input/output parameters. The facade invokes once,
extracts the complete low-level result and only then copies/moves results into
caller variables. Output-only values are not read or passed as native inputs.
Supported container results are owned script values and move back to caller storage.
Passing the same variable to different ref/out slots retains frame semantics: the
native slots are independent, and copyback follows declaration order. It does not
preserve native pointer aliasing or roll back arbitrary earlier receiver side effects.

This call-side support does not expand the ue_class/ue_interface authoring-side
capability limits for reflected out/ref types. It also does not introduce reflected
TScriptInterface storage, VM reentry, native Execute fallback or structural migration.
Reserved handle-field name and nonidentifier UFunction names retain low-level wrappers
without a dot-call facade. Defaults retain the generator's existing supported rules.

Examples: generated_interface_calls.das and the generated authored wrapper probe
from Scripts/GenerateAuthoredInterfaceProbe.py. Validation includes generator tests,
TestEditor.ps1, TestDSLTypeRegistry.ps1 -Interfaces -Cooked and -AuthoredInterface -Cooked.


Typed reference factories and an explicit first stage of natural authored enum/struct
facade mappings are now available. See DSLInterfaceSourceValues.md for scope and syntax.
