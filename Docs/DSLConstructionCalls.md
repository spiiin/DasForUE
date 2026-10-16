# Reflected method calls during construction

ConstructionScript may call reflected `ue_function` methods on authored Actors
and ActorComponents, including components created by the Actor's component DSL.
Use the same typed reference syntax as during gameplay:

```das
def ConstructionScript():void {
    self.Result = self.Worker.Evaluate(7)
    if(ue_object_valid(self.CounterTarget.handle)) {
        self.Result += self.CounterTarget.ReadScore()
    }
}
```

These calls run in the temporary VM set belonging to the current construction
invocation. Repeated calls to the same object's script share that temporary VM;
the next construction starts fresh. Existing persistent gameplay VMs, globals,
subscriptions and initialization-delivery records are preserved. Methods do not
implicitly invoke Initialize or BeginPlay. Reflected UObject property writes
remain visible and are not rolled back when a method fails.

The existing method contracts, typed input/output marshalling, reference checks
and authored recursion limit (32 calls) still apply. A nested method error
propagates to ConstructionScript and LastDasError; the next construction can
retry. Actor Blueprint method graphs are explicitly enabled for the duration of
construction in editor builds, matching Unreal's editor construction policy.

Allowing method dispatch does not enable gameplay lifecycle transitions, recursive
construction, managed timers/input/async/latent callbacks, bridge event emission,
or dynamic Actor/component creation and destruction. Those restrictions apply
inside nested methods as well. bind_events is not installed in temporary VMs.
Native reflected functions retain the existing construction policy; this is not
a general native-code sandbox.

Fixtures: dsl_construction_calls.das and dsl_construction_call_worker.das.
Run Scripts/TestConstructionCalls.ps1 for Editor and Game world checks in both
Create and fresh-process Reopen. The probe checks Actor and component dispatch,
temporary globals, preservation of a persistent component VM and subscriptions,
absence of Initialize delivery, reconstruction, forbidden timer calls, propagated
panic and successful retry.

## Validation — 2026-10-16

Editor build and TestConstructionCalls.ps1 passed: Editor/Game worlds in Create
and fresh-process Reopen, including a nonempty persistent subscription and VM.
Foundation passed with zero failures. Real Gameplay.RuntimePIE passed with one
success and zero warnings/failures after the component PIE-duplication fix in the
shared working copy. Separate logs/report used for the integration PIE probe:
logs/construction-calls-runtime-pie.log and its -report directory.

These checks exercise game worlds in the editor executable. A separate packaged
or Shipping build was not run specifically for this method-dispatch extension.