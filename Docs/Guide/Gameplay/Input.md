# Input

Scripts bind Enhanced Input actions (and, if needed, legacy Action/Axis mappings) to ordinary class methods, and manage Mapping Contexts for a local player. Bindings are owned by the script session: they are removed automatically on EndPlay/release and restored after an accepted hot reload.

The plugin declares the `EnhancedInput` plugin as a dependency. It never creates InputAction/MappingContext assets, controllers or input components, never forces possession and never enables input or changes focus: configure those through normal Unreal project/Blueprint setup.

The complete working example is `Plugins/DasForUE/Content/Das/Scripts/dsl_playable_character.das` (WASD, mouse look, jump, interact, HUD toggle); create it with `.\Scripts\CreatePlayableCharacter.ps1`.

## Enhanced Input actions

```das
require ue_enhanced_input
require ue_character_control

[ue_class(Blueprintable)]
class Player : Character {
    settings=ue_properties(EditAnywhere,BlueprintReadWrite) {
        var Controls:InputMappingContextRef
        var MoveAction:InputActionRef
        var JumpAction:InputActionRef
    }
    [ue_function(BlueprintCallable)]
    def BindControls():void {
        ue_enhanced_add_context(ue_pawn_controller(ue_character_self()),self.Controls,0)
        ue_enhanced_bind(self.MoveAction,TriggerEvent.Triggered,self.OnMove)
        ue_enhanced_bind(self.JumpAction,TriggerEvent.Started,self.OnJump)
        ue_enhanced_bind(self.JumpAction,TriggerEvent.Completed,self.OnJump)
    }
    [ue_function(BlueprintCallable)]
    def OnMove(value:InputActionValue):void {
        let axis=ue_input_axis2d(value)
        ue_character_move(ue_character_self(),float(axis.y),float(axis.x))
    }
    [ue_function(BlueprintCallable)]
    def OnJump(value:bool):void { /* pressed or released */ }
}
```

| Function | Purpose |
| --- | --- |
| `ue_enhanced_bind(action:InputActionRef, trigger:TriggerEvent, handler)` | Bind or replace the handler for this action + trigger. `trigger` is `TriggerEvent.Triggered/Started/Ongoing/Canceled/Completed` (the string names are still accepted); `handler` is `self.Method` or the method name. |
| `ue_enhanced_unbind(action, trigger):bool` | Remove that binding. |
| `ue_enhanced_is_available():bool` | Does the owner have an EnhancedInputComponent? (Does not prove that assets/contexts/routing are configured.) |
| `ue_input_unbind_all():int` | Remove all legacy and Enhanced bindings of the session. |

Triggers: `Triggered`, `Started`, `Ongoing`, `Canceled`, `Completed`. Prefer `TriggerEvent.Member`; string literals now produce `DAS_DSL_STRING_TRIGGER` while the old overloads remain available.

### Handler value types

The handler signature selects the transport:

| Handler parameter | Accepts |
| --- | --- |
| `InputActionValue` | Any value type; keeps the kind and all axes, even if the action's ValueType changes. Recommended. |
| `bool` | Boolean actions |
| `float` | Axis1D actions |
| `UEVector2D` | Axis2D actions |
| `UEVector` | Axis3D actions |

Class handlers return `void`. Pass `self.Method` to generate a hidden entry point automatically; methods declared later in the class are resolved. String handler names require explicitly reflected methods. Add `[ue_function(BlueprintCallable)]` when a handler should also appear in the Blueprint palette. Plain `[export] def handler(value:InputActionValue):void` functions remain accepted. With a scalar/vector handler, a runtime value of the wrong kind is dropped. The native proxy checks SourceAction and finite values and delivers an owned copy; you may store it in an array.

### InputActionValue

`require ue_input_value` (re-exported by `ue_enhanced_input`). Example: `Plugins/DasForUE/Content/Das/Scripts/input_action_values.das`.

```das
let jump:InputActionValue=ue_input_value(true)
let throttle=ue_input_value(0.5)
let move=ue_input_value(ue_vector2d(1.0lf,0.0lf))
let look=ue_input_value(ue_vector(0.0lf,1.0lf,0.0lf))

let pressed=ue_input_bool(jump)
let amount=ue_input_axis1d(throttle)     // double
let direction=ue_input_axis2d(move)
let rotation=ue_input_axis3d(look)
let is2D=ue_input_type(move)==InputValueType.Axis2D
```

- `ue_input_value` is overloaded for `bool`, `float`, `double`, `UEVector2D` and `UEVector`. `InputValueType` is `Boolean`, `Axis1D`, `Axis2D` or `Axis3D`.
- `ue_input_bool` uses UE `IsNonZero` with native tolerance. Reading a different axis count is allowed, as with UE `Get<T>()`: missing components are zero. Axis1D goes through native float conversion, like EnhancedInputLibrary.
- The value is an opaque owned copy (no fields, no native pointer). It also works as an argument/result of generated functions such as `MakeInputActionValueOfType`, `BreakInputActionValue` and `ToString`.

### Binding semantics

- Action + trigger is the binding key within the session. Rebinding validates the new handler first, then replaces the old binding. Other sessions and unrelated native/Blueprint bindings are untouched; removal uses Enhanced Input's exact binding handle.
- Action assets are held strongly while bound; owner/component references are weak.
- An ActorComponent uses its owning Actor's input component.
- At most 64 input bindings per session, legacy and Enhanced combined.
- Action consume/paused behavior and trigger/modifier settings come from the assets and the Enhanced Input system.

## Mapping Contexts

```das
ue_enhanced_add_context(self.Player,self.Controls,0)
```

| Function | Purpose |
| --- | --- |
| `ue_enhanced_add_context(player:PlayerControllerRef, context:InputMappingContextRef, priority:int)` | Add (acquire) or update the priority of a context owned by this session. |
| `ue_enhanced_has_context(player, context):bool` | Is the context installed, regardless of owner? |
| `ue_enhanced_remove_context(player, context):bool` | Release this session's record; `false` if absent. |
| `ue_enhanced_clear_contexts():int` | Release all context records of this session. |

- The player is explicit; no "first player" guessing. The controller must be local, in the script owner's world, with a ready LocalPlayer Enhanced Input subsystem.
- Priorities are `int32` (negative allowed). Adding uses the normal deferred mapping rebuild and does not save user settings.
- Ownership is cooperative: acquiring a context already installed externally or by another session is refused, but you can still query it and bind actions to it without acquiring it. On removal, a context whose priority was changed externally stays installed; only the record is released. For contexts shared by several systems, keep them under one external manager.
- Up to 32 context records per session. Scope is LocalPlayer subsystem + asset, so different local players can use the same asset independently. Context assets are held strongly while recorded.
- Contexts and action bindings are independent: removing a context does not remove action handlers, and `ue_input_unbind_all` does not release contexts. Both are cleaned up at session teardown.

Example: `dsl_enhanced_contexts.das` queries first and uses an externally installed context without taking ownership.

## Lifetime, reload and restoration

- EndPlay, owner/source expiry, release, world cleanup, shutdown and editor reinstancing remove bindings and context records.
- Each successful bind or context acquisition also stores a **recipe** (action/mapping, trigger/phase, handler, flags, context/priority). After an accepted reload or an input component replacement, a world-tick pass restores the last successfully installed configuration against the new code. Missing components or not-yet-ready local-player input defer the restore; errors are retried after one second.
- A rejected reload keeps the old live bindings.
- Restoration does not re-run your setup code: if you edit bind calls in the script, call your setup method again to apply the new configuration. Removed or incompatible handlers are not silently rebound; `BeginPlay` is not replayed.
- Manual unbind/remove/clear also cancels the corresponding recipe. External contexts are never acquired during recovery; a priority conflict defers recovery instead of overwriting another system.
- Removing bindings natively behind the plugin's back is not tracked; use these APIs or rebind explicitly.

## Legacy Action/Axis mappings

For projects still using named legacy mappings and `UInputComponent`:

```das
ue_input_bind_action("Jump","Pressed","OnJumpPressed",true,false)
ue_input_bind_axis("MoveForward","OnMove",true,false)

[ue_function(BlueprintCallable)]
def OnJumpPressed():void { self.JumpCount+=1 }
[ue_function(BlueprintCallable)]
def OnMove(value:float):void { self.MoveValue=value }
```

| Function | Notes |
| --- | --- |
| `ue_input_bind_action(name, phase, callback, consume, execute_when_paused)` | Phases `Pressed`, `Released`, `Repeat`, `DoubleClick`; callback `():void`. |
| `ue_input_bind_axis(name, callback, consume, execute_when_paused)` | Callback `(value:float):void`. |
| `ue_input_unbind_action(name, phase):bool`, `ue_input_unbind_axis(name):bool` | Remove one binding. |
| `ue_input_is_available():bool` | Does the owner have a valid InputComponent? |

- Binding requires an existing InputComponent: call your `BindControls` after EnableInput/possession, or check `ue_input_is_available()`.
- `consume` and `execute_when_paused` map to `FInputBinding` flags; routing and priority follow the normal input stack.
- The adapter does not check that a key mapping exists: binding an unmapped name is valid but never fires.

Example: `dsl_legacy_input.das` (expects `Jump`/`MoveForward` mappings).

## Widget input and UI mode

To toggle between game and UI input with a cursor, use `ue_widget_input` from [Gameplay services](GameplayServices.md). Keyboard, mouse, touch, focus and navigation handlers on widgets are covered in [Widget events](../UMG/WidgetEvents.md).

Generated UMG functions that take a native single-cast delegate accept a checked UFunction endpoint:

```das
require ue_api_umg_input

let endpoint=ue_function_delegate(target,"ChooseNavigation")
ue_widget_SetNavigationRuleCustom(widget,direction,endpoint)
```

`target` is a checked object reference or `UObjectHandle`; the named UFunction must match the native delegate signature exactly (including return/out/ref). The stored UE delegate is weak. Use the native Unbind/Remove API, or `ue_function_delegate_null()` where the setter accepts an empty delegate; there is no automatic unbind for these endpoints.

## Limitations

- No input configuration DSL (creating actions, mappings or remapping), no automatic possession/world-travel transfer, no network/replicated input.
- Elapsed/triggered-time handler parameters are not exposed.

See also: [Gameplay services](GameplayServices.md), [Widget events](../UMG/WidgetEvents.md), [Apply and hot reload](../Workflow/ApplyAndHotReload.md), [UMG and Input API](../../Api/UMGInput.md), [Examples](../GettingStarted/Examples.md)
