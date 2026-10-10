# Session-managed Enhanced Input mapping contexts

Added 2026-10-10. **Not built or tested**. Module/UHT, source wrappers, subsystem
readiness, ownership, actual input, reload, PIE and packaged behavior are unverified.

```das
ue_enhanced_add_context(self.Player,self.Controls,0)
ue_enhanced_bind(self.JumpAction,"Triggered","OnJump")
```

PlayerControllerRef and InputMappingContextRef are checked native wrappers in
ue_enhanced_input. Player selection is explicit; no first-player/world guessing.
The controller must be local, in the script owner's world, and have a LocalPlayer
Enhanced Input subsystem with a ready EnhancedPlayerInput.

API:
- ue_enhanced_add_context(player, context, priority): add/acquire or update an owned context.
- ue_enhanced_has_context(player, context): query installation, regardless of owner.
- ue_enhanced_remove_context(player, context): release this session's record, false if absent.
- ue_enhanced_clear_contexts(): release all context records of this session, returning count.

Contexts are not created by these APIs. Assign existing assets/PlayerController in
Blueprint or construct typed refs through the existing object API. Priorities use
int32, including negative priorities. Add uses normal deferred control-mapping rebuild
semantics and does not notify/save user settings (bNotifyUserSettings=false).

Initial acquisition refuses a context already installed externally or by another
session. An existing context can still be queried and used by action bindings
without acquiring it. Owned context priority updates check that current priority
still matches the lease. On removal/cleanup, a context whose priority was changed
externally is left installed; only this session's record is released.

This is a cooperative ownership boundary, not a universal shared-context reference
counter. External remove/re-add at the same priority is not distinguishable from
the original installation. Keep concurrently shared contexts under one external
manager and bind actions to them without acquiring them through this API.

Context assets are held strongly while recorded; subsystem references are weak.
At most 32 context records per session. Scope is LocalPlayer subsystem plus asset,
so separate local players can use the same asset independently. A successful install
is confirmed through the subsystem's reported priority before recording ownership.

EndPlay/release/world cleanup/shutdown/accepted reload and editor reinstancing release
context records after removing managed event/input bindings. Rejected candidate reload
retains old records. Reapply Setup after accepted reload/reinstancing. General asset
migration and automatic possession/world-travel transfer remain separate.

Context records and action bindings have independent explicit APIs. Removing a context
does not remove the action's native handler; unbind actions separately if desired.
ue_input_unbind_all only removes handlers; ue_enhanced_clear_contexts only releases
contexts. Both are automatically handled at session teardown.

Example: dsl_enhanced_contexts.das. Setup queries before adding, so it uses an
externally installed context without taking ownership. Example controller/assets/
component and real keyboard input have not been configured or executed.

Pending checks: exact wrapper/subsystem API compatibility; local/remote/world/controller
refusal; installed external context preservation; duplicate/update/negative priorities;
priority drift; failed install and record cleanup; multiple sessions/local players;
asset GC and subsystem destruction; deferred rebuild/no user-settings persistence;
teardown/reload/EndPlay before BeginPlay; actual input, PIE and packaged Development.

Desired-state restoration was added subsequently (validation pending). Accepted
reload/component replacement can restore last successful recipes on a world tick;
see ManagedInputRestore.md. Earlier explicit-rebind statements describe the initial
adapter slices. Initial setup and explicit configuration changes still require calls.
