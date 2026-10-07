# Lifecycle and delegate preservation during class migration

DSL declaration reimport, stable-ID field rename, numeric migration and field
retirement/restoration now use an explicit editor runtime preservation scope.
Numeric/archive rollback uses the same scope as a successful migration. No new
console option is required.

`BeginPreserveContextsForReinstance(RootClass)` requires initialized, idle runtime
on the game thread: no active VM, nested preservation scope, event draining or
pending events. Invocation-local call frames must be empty. It suspends all live
sessions for this synchronous operation, including other owners/worlds: an
unmodified owner can subscribe to a replaced source. Managed delegate proxies are
detached before Unreal duplicates delegate properties.

`OnObjectsReplaced` remaps session owners/worlds, context self, typed weak object
handles and subscription declaration sources through every replacement, including
rollback replacements. Ending the scope reinstalls each valid declaration once
using a new proxy ID. Old IDs cannot dispatch into the preserved VM. Dead owners,
ended/released sessions and world cleanup are not revived.

Lifecycle delivery flags stay with the session. Native BeginPlay/EndPlay calls
during compilation do not execute gameplay script. The native Actor adapter's
active lifecycle path and previous Tick enabled state restore on replacement
objects. The next real Tick continues the script; BeginPlay is not replayed.
Actual Destroy and EndPIE still deliver EndPlay once and release bindings/context.

Existing VM context/state survives the replacement scope. A successful DSL host
change approves only the exact applied declaration for owners in the root scope,
and forces source validation against the new reflected owner. Normal hot reload
still creates a candidate VM and transfers explicit `ue_state_*` state, as before.
Rollback provides no declaration approval, so rejected source cannot replace the
last-good code. Unmodified external owners keep their existing VM and bindings
remapped to the replacement source.

UFunction call frames remain scoped to one VM invocation and are cleared on exit,
including exceptions; saved tokens cannot be used by a later invocation. This
stage does not introduce cross-call frame caches or migrate reflected frame
addresses. Typed object handles and declared subscriptions are remapped.

This mechanism is integrated with DasDSLService operations. Arbitrary external
Blueprint compilation is not automatically enclosed by this scope. Numeric/archive operations and existing-Blueprint declaration reimport/rename
have bounded rollback; see DSLStructuralRollback.md. World Partition/streaming/
other asset migration scopes remain separate work.

Tests: `Scripts/TestDSLLifecycleMigrationPIE.ps1`. Details and confirmed regression
results: DSLLifecycleMigrationValidation.md.


Existing-Blueprint structural reimport and stable-ID rename now have bounded
rollback too; see DSLStructuralRollback.md. Expanded asset scopes and arbitrary
Unreal transactions remain separate work.
