# Owner-managed legacy Action/Axis input

Added 2026-10-10. **Not built or tested**. No UHT/build, example import, real input,
binding ownership, reload, PIE or packaged execution has been verified.

```das
ue_input_bind_action("Jump","Pressed","OnJumpPressed",true,false)
ue_input_bind_axis("MoveForward","OnMove",true,false)

[ue_function(BlueprintCallable)]
def OnJumpPressed():void { self.JumpCount += 1 }
[ue_function(BlueprintCallable)]
def OnMove(value:float):void { self.MoveValue = value }
```

This adapter uses the **legacy** named Action/Axis mappings and UInputComponent.
Enhanced Input InputAction/MappingContext assets are not supported by this slice.
Configure mappings and Actor input/possessed Pawn setup through existing Unreal
project/Blueprint mechanisms. The adapter does not create mappings, controllers,
InputComponents, enable input, change focus or modify the project's input stack.

An Actor uses its current InputComponent. An authored ActorComponent uses its
owning Actor's InputComponent. Binding requires a live persistent script session
and an existing valid component. Use ue_input_is_available() or call BindControls
after EnableInput/possession has completed. InputComponent creation/replacement
later does not automatically rebind old records.

API (explicit boolean flags):
- ue_input_bind_action(name, phase, callback, consume, execute_when_paused)
- ue_input_bind_axis(name, callback, consume, execute_when_paused)
- ue_input_is_available(): bool
- ue_input_unbind_action(name, phase): bool
- ue_input_unbind_axis(name): bool
- ue_input_unbind_all(): int (number removed in current session)

Action phases: Pressed, Released, Repeat, DoubleClick. Actions require void
callbacks without arguments; Axis requires void callback(value:float). Class
callback names are literals lowered to annotated method exports, including forward
declarations. Ordinary exported DAS handlers can use exported names directly.
No additional input annotation is introduced in this stage.

Mapping name/phase define a binding key in the owner/script session. Rebinding
replaces that session's old handler after component/name/contract/count validation.
There is a 64-input-binding limit per session. Unreal FName mapping equality applies.
The adapter does not verify that a configured key mapping exists: a valid binding
to an unmapped name receives no physical input.

Input uses the existing proxy/event queue and void/float payload dispatch. Action
bindings retain Unreal's handle for exact removal; Axis removal matches this
binding's dedicated proxy. Native/Blueprint/other script bindings on the component
remain. consume and execute_when_paused forward to Unreal's FInputBinding flags.
The normal input stack determines routing/priority and paused behavior.

Source/owner expiry, EndPlay, release, world cleanup, shutdown, accepted reload
and editor reinstancing tear down these bindings with existing session binding
cleanup. Failed candidate reload retains old bindings. Accepted reload requires
explicit BindControls again; BeginPlay is not replayed. Required-entry contracts
remain until session release, even after manual unbinding. Events use the current
live context; source recompile checks remain driven by existing persistent call/
lifecycle paths rather than a separate input-file polling mechanism.

Example: dsl_legacy_input.das. It exposes state and explicit BindControls/InputReady/
UnbindControls for a Blueprint configured with existing Jump/MoveForward mappings.
The example has not been configured, imported or exercised in the editor.

Still separate: Enhanced Input and typed action values; keys/touch/vector-axis;
automatic possession/component-change hooks; remapping/input configuration DSL;
network authority/replication and async-thread dispatch.

Pending checks: build/UHT and InputCore dependency; real pressed/released/axis input;
missing component/mapping; consume/paused routing; owner/component and world
isolation; replace/unbind preserves unrelated native bindings; queued revoked IDs;
input component replacement/GC; nested authored calls; accepted/rejected reload,
EndPlay/reinstancing; real PIE and packaged Development.

Enhanced Input action bindings were added subsequently (validation pending);
see ManagedEnhancedInput.md. ue_input_unbind_all now removes both binding families
within its current session. The legacy mapping APIs above retain their original scope.

Desired-state restoration was added subsequently (validation pending). Accepted
reload/component replacement can restore last successful recipes on a world tick;
see ManagedInputRestore.md. Earlier explicit-rebind statements describe the initial
adapter slices. Initial setup and explicit configuration changes still require calls.
