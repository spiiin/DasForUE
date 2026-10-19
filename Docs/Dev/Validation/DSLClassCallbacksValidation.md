# Class callback validation — 2026-10-09

UE 5.8.3 source build, Win64 Development, pinned daScript 0.6.4.

- TestActorComponent.ps1 -Cooked: PASS. Actor/Component examples use class
  BeginPlay/Tick/EndPlay methods with no lifecycle annotations or global exports.
  Component OnOwnerSignal uses ue_event and an automatically generated binder.
  Field changes occur through ordinary self assignments. The native runtime ABI
  and managed subscription/revocation backend are retained.
- Create/Reopen/packaged each pass the 12 shared checks, including Blueprint
  methods/ref values, reflected defaults, direct lifecycle state writes,
  exact Tick delta, owner events, isolation, component/Actor destruction and cleanup.
- Callback-only Actor (no Blueprint callable methods): PASS. Import installs
  LifecycleScript; BeginPlay/Tick/Destroy route callbacks and update reflected
  fields, ending once and releasing the session. Callback transport roundtrip PASS.
- Negative callback checks PASS: wrong Tick parameter type, duplicate ue_lifecycle
  annotation and callback combined with ue_function are refused explicitly.
- Existing composition checks PASS: repeated components, collisions, foreign asset
  preservation and deferred declaration migration guard.
- TestActorComponentPIE.ps1: one actual PIE automation test PASS, no warnings,
  failures or skipped tests. EndPIE observes direct EndCount/Actor Ends writes.
- TestEditor.ps1: 524 PASS, failures=0.
- GenerateBindings.ps1 -Check: PASS; native IR/module unchanged.
- TestBindingsGenerator.py: 84 PASS.
- git diff --check: PASS; ThirdParty unchanged. No commit was made.

The manually created Game worlds in the shared probe now initialize Actors with
PostInitializeComponents before dispatching BeginPlay, allowing Unreal to route
Destroy through EndPlay. Real PIE performs that initialization itself.

Scope: direct self field syntax in class lifecycle/event methods. Typed access to
fields on another authored UObject remains separate. Event handlers currently use
the existing single-int multicast contract. Structural migrations remain deferred.

Evidence: logs/actor-component-Create.log, actor-component-Reopen.log,
actor-component-cooked.log, actor-component-pie.log and report, smoke-editor.log,
bindings-export.log. Saved GUID assets and packaged output belong to the test project.
