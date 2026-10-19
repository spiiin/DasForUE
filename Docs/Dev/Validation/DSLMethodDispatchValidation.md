# Own methods and Blueprint override validation — 2026-10-11

Editor build and foundation tests passed. The related regression batch passed
Actor/Component create/reopen and PIE, and MethodValues create/reopen and PIE
(`-Cooked` was not used).

`Scripts/TestMethodDispatchPIE.ps1` exercises `DasForUE.MethodDispatch.PIE` in a
separate unattended editor. The fixture creates a base imported Blueprint and
two genuine child Blueprint override graphs using the UClass signature overload
of AddFunctionGraph. One override calls the inherited implementation and adds
100; the other returns 200 without calling the parent.

The tests validate:

- A lifecycle callback calling a script-only own helper.
- Helpers absent from UObject reflection.
- Direct self calls, recursion, ref mutation, array parameters, and own callable
  method calls in the same persistent DAS context.
- Ordinary callable methods carry Final; explicit events do not.
- A default event body runs when there is no child override.
- A DAS self event call dispatches to the child BP; parent implementation changes
  the same object's Score exactly once.
- A child override without parent-call skips the default DAS body.
- Reimport rejects changing an existing method's event role before mutating its
  authoring metadata or UFunction. The generated guard source is removed.
- Runtime calls have no errors, and EndPIE releases persistent contexts.

Logs: `logs/method-dispatch-pie.log`, automation report:
`logs/method-dispatch-pie-report/index.json`.

Compiler policy is applied on each Blueprint precompile using internal method
metadata. This keeps the Final policy through repeated/skeleton compilation.
The existing typed authored-reference backend handles event argument marshalling;
there is no second event-specific frame backend.

Scope limits: authored DAS class inheritance and script Super syntax, arbitrary
native BlueprintOverride, named self arguments, role/dependent-BP migrations,
and cooked execution of the new fixture remain separate work. Interface method
implementations keep their virtual contract. Static helpers and reference returns
are deliberately refused in this first own-method lowering.
