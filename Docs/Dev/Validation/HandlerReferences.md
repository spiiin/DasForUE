# Unified handler references

Worktree: `C:/src/DasForUE2-ui`, branch `widget-layout`.

The class pass recognizes `self.Method` in `ue_bind` and BindUFunction/AddUFunction/RemoveUFunction method forms, promotes a hidden reflected entry point and lowers through a common checked macro. Delegate handlers can return values; queued/managed callback targets retain their void constraint. The checked macro validates the complete native delegate signature against the authored method model before Apply, and guards the explicit target to current self. No string lookup alone stands in for a typed reference.

Free `@@function` references generate stable UTF-8 signature/module hash identities. Delegate wrappers reuse the extracted class-method ABI adapter, including typed reference/container conversion, return values and ref/out publication. An infer pass accepts free references in managed callback slots, retaining their existing ABI rules and current-module restriction. Arbitrary function variables/closures remain outside this work. Inherited method discovery is covered by the later [inherited-handler extension](InheritedHandlers.md).

Free delegates use a session-owned transient native proxy with a weak owner and VM-generation lease, plus stable script/export identity. No VM function pointer is retained. A shared native receiver factory clones/links/restores computed container flags and roots receiver signatures; existing queued receivers use the same factory. Native event registrations are owned separately from temporary delegate tokens. VM replacement, release, EndPlay (including no BeginPlay) and Destroy revoke capability leases and detach owned native events. Rejected candidates preserve old capabilities. Free delegate outputs/results stage through the common property codec before touching caller memory; game-thread guards precede lease access.

## Targeted validation

Build agent job `dsl-handler-final-bdd47e61`: exit 0, `DAS_SUITES=PASS`.

- Handler references: 34 Apply/compile/runtime checks, including negative names/signatures/directions, hidden methods, current-self guard, plain and method Bind/Add/Remove, free results/ref-out/object/array ABI, native event registration, timer delivery, native synchronous result, GC, rejected/accepted reload, generation-checked dispatch, EndPlay, release and Destroy.
- Extern manifest/names: 15 checks.
- Event contracts: 51 checks.
- Inline callbacks: 26 checks.
- Existing authored delegates: Create and Reopen phases passed in separate processes.

Total: 126 numbered checks plus the legacy delegate phases. Only targeted suites were run for this task; no full suites, PIE or packaging.

Generated ABI helpers carry internal exact-contract evidence, checked in addition to their lowered VM types. Native receiver functions derive `FUNC_HasOutParms` from parameter flags and publish staged outputs through `Stack.OutParms`, preserving caller ref/out storage.

The handler runner freezes plugin source/modules, both fixtures, runner, extern baseline, binaries and module manifests; its receipt is `logs/handler-references-validation.json`. Focused receipt job `dsl-handler-receipt-481f197d` also passed (exit 0, 34 checks); local receipt hashes were rechecked against the committed source and binaries.
