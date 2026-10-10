# Managed Enhanced Input action bindings

Added 2026-10-10. **Not built or tested**. Plugin/module linkage, UHT, typed source
wrappers, actual trigger input, lifecycle, GC, PIE and packaged execution are pending.

```das
require ue_enhanced_input

ue_enhanced_bind(self.JumpAction,"Triggered","OnJump")
ue_enhanced_bind(self.MoveAction,"Triggered","OnMove")

[ue_function(BlueprintCallable)]
def OnJump(value:bool):void { self.JumpHeld = value }
[ue_function(BlueprintCallable)]
def OnMove(value:UEVector):void { self.MoveValue = value }
```

The public source module provides a checked native InputActionRef. Blueprint fields
of that type use the actual UInputAction class; wrappers do not require user-level
asset paths in callback declarations. `ue_enhanced_bind(action, trigger, handler)`
uses the asset's ValueType to select a bool, float or Vector callback contract.
Class short literal handler names lower to annotated void method exports. Plain
exported DAS handlers use exported names through the same source helper.

Supported value shapes: Boolean -> bool, Axis1D -> float32, Axis3D -> UEVector.
Axis2D is explicitly refused, rather than silently padding/converting to Vector.
No-argument handlers and additional elapsed/triggered-time callback parameters are
not exposed by this slice. Internal reflected proxy matches Enhanced Input's full
dynamic delegate signature and extracts only the typed action value.

Trigger strings: Triggered, Started, Ongoing, Canceled, Completed. An action and
trigger are the session-local binding key. Rebinding replaces the old owned handle
after validation. Other sessions and unrelated native/Blueprint bindings remain.
The runtime stores Enhanced Input's uint32 handle for precise RemoveBindingByHandle.
Action assets are strongly held while bound; owner/component references are weak.
Changing ValueType while bound is not adapted: mismatched runtime values are dropped.

API:
- ue_enhanced_bind(action:InputActionRef, trigger:string, handler:string)
- ue_enhanced_unbind(action:InputActionRef, trigger:string):bool
- ue_enhanced_is_available():bool (current owner's EnhancedInputComponent exists)
- ue_input_unbind_all():int now removes both legacy and Enhanced bindings of the session.

ActorComponents use their Actor's input component. The shared 64-input-binding limit
counts legacy and Enhanced records together. Callback dispatch uses the existing
bounded game-thread event queue, native bool/float/Vector payloads and error reporting.

Project setup remains external to this adapter: create/assign InputAction assets,
configure an EnhancedInputComponent/PlayerInput, and add the Mapping Context through
the LocalPlayer subsystem. The runtime does not create mappings/contexts/controllers,
force possession, enable input or change focus. Ready only checks component type;
it does not prove assets/context/routing are configured. Action consume/paused and
trigger/modifier settings remain asset/system configuration, not legacy binding flags.

DasForUE.uplugin now declares EnhancedInput enabled as a dependency; runtime build
rules include EnhancedInput, and runtime startup explicitly loads that module for
native class availability. No unrelated project configuration was modified.

EndPlay, owner/source expiry, release/world shutdown/reinstancing and accepted reload
tear down bindings. Rejected reload retains old bindings. BindControls must be called
again after accepted reload or component replacement; no automatic possession hook.
Registered callback contracts remain until session release. Existing persistent calls
and lifecycle drive source reload checks; input does not add a separate watcher.

Examples: ue_enhanced_input.das and dsl_enhanced_input.das. Set the three fields to
Boolean/Axis1D/Axis3D assets respectively and install a matching Mapping Context.
Example assets and scene configuration have not been created/executed.

Pending checks: build/UHT/dependency loading; complete dynamic proxy signature;
wrapper class/path checks; all triggers and value types; incorrect assets/Axis2D;
callback mismatch before replacement; precise unbind and unrelated bindings; asset
GC, owner/world isolation, component replacement, reentry/reload/teardown; real PIE
and packaged Development. Mapping Context APIs, Axis2D and automatic lifecycle hooks
remain separate work.

Explicit session-managed Mapping Context add/remove/query APIs were added later
(validation pending); see ManagedMappingContexts.md. The adapter still does not
create assets, controllers or input components.

Desired-state restoration was added subsequently (validation pending). Accepted
reload/component replacement can restore last successful recipes on a world tick;
see ManagedInputRestore.md. Earlier explicit-rebind statements describe the initial
adapter slices. Initial setup and explicit configuration changes still require calls.
