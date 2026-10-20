# GAS local targeting

`require ue_gas` exposes a local, synchronous selection session. It uses real world channel collision queries and returns the same `GASTargetData` consumed by `ue_gas_apply_targets`.

```das
// Inside Activate(context:GASAbilityContext):
var query=ue_gas_trace(ue_vector(0.0lf,0.0lf,50.0lf),
                       ue_vector(1000.0lf,0.0lf,50.0lf),3,12.0f)
push(query.RequiredTags,ue_name_value("Target.Enemy")) // registered GameplayTag
query.MaxTargets=4
self.Selection=ue_gas_targeting_begin(context,query)
// Store Selection in an ordinary int field. In an input handler:
let preview=ue_gas_targeting_preview(context,self.Selection)
// On confirmation (after successful ue_gas_commit(context)):
var result=ue_gas_targeting_confirm(context,self.Selection)
if(result.Confirmed) {
    var applied <- ue_gas_apply_targets(context,spec,result.Targets)
    ue_gas_release_targets(result.Targets)
}
// On cancellation instead:
ue_gas_targeting_cancel(context,self.Selection)
```

`context` and `spec` in the fragment are the current ability context and an authored effect spec with that context's ASC as Source. Begin/preview/update/confirm do not commit or apply any effects. The committed activation rule remains enforced by the context overload of apply. For use outside abilities, every targeting operation also accepts `AuthoredAbilitySystemComponentRef` instead of the context.

## Queries and filters

- `ue_gas_trace(start,finish,channel=3,radius=0)` creates a line query. Positive radius creates a sphere sweep. Channel 3 is Visibility; use the project's collision channel. Native multi-channel semantics stop at the first blocking collision, even if the blocking actor is removed by a class/tag filter. Overlapping hits before the blocker remain candidates. To make an actor transparent, put it in IgnoredActors or change collision responses.
- `ue_gas_overlap(center,radius,channel=3)` creates a sphere overlap. Results include blocking and overlapping components. Actors are deduplicated and ordered by actor-origin distance to center, with the native object ID as the tie-breaker within the process.
- Factories set IgnoreSelf=true, RequireAbilitySystem=true and MaxTargets=16. A raw GASTargetingQuery() is a transport value; use factories for usable DAS defaults. MaxTargets is 1..64, radius is finite and at most 1,000,000 world units, overlap radius must be positive. Start/End must be finite. Up to 64 ignored actors and 64 tag names in total are allowed.
- Use ue_gas_target_class(query,actorClass) with a loaded typed Actor class to restrict candidates. ActorClassPath is the internal transport field; it is resolved among loaded classes without loading assets. IgnoredActors is an array of actor refs. IgnoreSelf excludes the ASC component owner and its GAS owner/avatar. TraceComplex controls the native collision query.
- RequiredTags must all match, BlockedTags must not match. Names must be registered GameplayTags and matching is hierarchical. Tags come from the target's ASC, not AActor.Tags. A tagged filter always requires an ASC. RequireAbilitySystem accepts any native ASC; applying authored effects additionally requires the authored ASC and compatible AttributeSets.

## Session ownership

Begin returns a positive ASC-local ID (0 on invalid query or capacity). Each ASC has at most 64 live sessions; IDs are never reused. A valid empty selection is allowed. Preview returns Valid plus Actors and never allocates persistent hit IDs. Update reruns collision selection and replaces the previous snapshot only on success. Passing an invalid update leaves the previous snapshot available.

Confirm consumes the session, rechecks all selected actors' lifetime/world/filters, and returns Confirmed plus Targets. Failure also consumes the session. Cancel consumes without producing TargetData; repeated confirm/cancel fail. Empty selections may confirm successfully with empty Targets. Confirmation does not rerun collision or range checks: call update immediately before confirm if fresh positions are needed.

Trace/sweep confirmation returns owned native hit IDs in Targets.Hits; overlap returns Actors. TargetData applies to each unique actor once. Release confirmed hit snapshots with ue_gas_release_targets after applying/discarding. Cancellation and preview allocate no persistent hits. Retained hit references to actors/components remain weak. Hit allocation failure rolls back allocations made by that confirmation.

Context overloads guard the activation serial and ownership. Pending context-owned sessions are cancelled before End/Cancel hooks and on ability removal/destruction. An old context cannot control a later activation's session. ASC-owned sessions are independent and must be explicitly confirmed/cancelled; all sessions are discarded on ASC EndPlay. IDs are scoped to their ASC/context: do not transfer a bare ID to another ASC.

This block does not introduce target-actor UI/reticles, asynchronous query callbacks, network TargetData transport, prediction, cone/box queries or allegiance policy. Friendly/enemy selection can use ASC GameplayTags.

## Focused verification

`Scripts/TestGASTargeting.ps1` builds Runtime/Editor and runs one short NullRHI commandlet. It exercises actual physics queries, DAS helpers, filtering, confirm/cancel, effect application, stale targets and activation cleanup. Full PIE and packaged runs are outside this step.
