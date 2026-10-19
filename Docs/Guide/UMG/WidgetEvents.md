# Widget Input and Focus Events

`UDasScriptUserWidget` lets a Widget Blueprint handle keyboard, mouse, touch, analog and focus
events in daScript. Handlers run synchronously inside the native `NativeOn…` call and return an
`FReply` to Slate in the same stack, so they can mark input as handled, capture the mouse, set focus
or start drag detection.

## Setup

1. Create a Widget Blueprint whose parent class is `DasScriptUserWidget`.
2. Set `ScriptFile` (the `.das` file name in the Scripts folder).
3. Fill the `InputHandlers` map: UE event name → name of an `[export]` function in that file.

The widget can be created with the usual `CreateWidget` or with `ue_widget_create`. As with any
UserWidget, keyboard input requires the widget to be focusable and focused.

```das
options gen2
require ue_widget_events
require ue_api_umg_input

[export]
def key_down(geometry:UEGeometry; event:UEKeyEvent):UEEventReply {
    if (ue_input_key_GetKey(event).f_keyname.value=="SpaceBar") {
        return ue_handled()
    }
    return ue_unhandled()
}

[export]
def mouse_down(geometry:UEGeometry; event:UEPointerEvent):UEEventReply {
    let screen=ue_input_key_PointerEvent_GetScreenSpacePosition(event)
    let local=ue_ui_geometry_AbsoluteToLocal(geometry,screen)
    if(local.x<0.0lf || local.y<0.0lf){return ue_unhandled()}
    return ue_reply_capture_mouse(ue_handled(),ue_self_object())
}

[export]
def mouse_up(geometry:UEGeometry; event:UEPointerEvent):UEEventReply {
    return ue_reply_release_mouse(ue_handled())
}
```

Map: `OnKeyDown → key_down`, `OnMouseButtonDown → mouse_down`, `OnMouseButtonUp → mouse_up`.
Inside a handler `ue_self_object()` is the widget itself.

### Binding from script

For a widget created and owned by the current script context, bind handlers before showing it:

```das
ue_widget_bind_input(widget,"my_widget.das","OnKeyDown","key_down")
```

The Blueprint/native equivalent is `BindInputHandler`. All handlers of one widget share one
`ScriptFile` and one context; binding with a different file releases the previous context and its
bindings. An empty handler name removes that binding. `NativeDestruct` releases the widget's context,
snapshots and managed resources; the handler-name configuration is kept for re-construction, but old
VMs, replies and tokens are not carried over.

## Events and signatures

All input events receive `geometry:UEGeometry` first. The exact signature is
`[export] def handler(geometry:UEGeometry; event:TYPE):UEEventReply`.

| Event | Second parameter |
| --- | --- |
| `OnKeyChar` | `UECharacterEvent` |
| `OnPreviewKeyDown`, `OnKeyDown`, `OnKeyUp` | `UEKeyEvent` |
| `OnAnalogValueChanged` | `UEAnalogInputEvent` |
| `OnMouseButtonDown`, `OnPreviewMouseButtonDown`, `OnMouseButtonUp` | `UEPointerEvent` |
| `OnMouseMove`, `OnMouseWheel`, `OnMouseButtonDoubleClick` | `UEPointerEvent` |
| `OnTouchGesture`, `OnTouchStarted`, `OnTouchMoved`, `OnTouchEnded` | `UEPointerEvent` |
| `OnTouchForceChanged`, `OnTouchFirstMove` | `UEPointerEvent` |

Focus events use the same `InputHandlers` map with different signatures:

| Event | Signature |
| --- | --- |
| `OnFocusReceived` | `(geometry:UEGeometry; event:UEFocusEvent):UEEventReply` |
| `OnFocusLost` | `(event:UEFocusEvent):void` |

Drag & drop events use a separate `DragHandlers` map, see [Drag & Drop](DragDrop.md); the paint
callback uses `PaintHandler`, see [Paint and Animation](PaintAndAnimation.md).

Signatures are checked when the handler is bound and again on hot reload. Single-argument
handlers (without `geometry`), wrong parameter order or wrong types are rejected.

Dispatch rules:

- An explicit `Unhandled` is returned to Slate without calling the parent.
- If no handler is bound, no file is set, or the handler fails (error, panic, wrong signature),
  `Super::NativeOn…` is called (native parent fallback). No partially built reply is published.
- The last failure reason is in `LastInputError`; a successful callback clears it.
- Re-entering the VM synchronously from Slate (e.g. a focus change triggered inside a handler) is
  rejected.

## Event snapshots

`UEInputEvent`, `UEKeyEvent`, `UEAnalogInputEvent`, `UECharacterEvent`, `UEPointerEvent`,
`UENavigationEvent` and `UEFocusEvent` are opaque, immutable handles to an owned native copy of the
event (module `ue_input_events`, re-exported by `ue_api_umg_input`).

- The copy keeps modifiers, repeat, user index, device ID, timestamp, key/character codes, pointer
  positions, `CursorDelta`, gesture/touch data and an owned set of pressed buttons (max 256).
- It does not expose the Slate event path, window, `FindGeometry` or the source widget, and does not
  keep the widget, window or world alive.
- You may store snapshots in globals or arrays and clone them; they stay readable after the callback
  and survive GC while referenced. Release or reload of the context frees them; they are not carried
  between contexts.
- A default (empty) handle is invalid. Numeric constructors/casts and access to the token are
  compile-time errors. `ue_property`, defaults, serialization and native containers of event types are
  not supported.

`ue_api_umg_input` exposes 33 getters and projections from `KismetInputLibrary` and
`WidgetBlueprintLibrary` (for example `ue_input_key_GetKey`, `ue_input_key_InputEvent_IsShiftDown`,
`ue_ui_GetInputEventFromKeyEvent`, `ue_input_key_PointerEvent_GetScreenSpacePosition`). Results are new
detached snapshots.

`UEFocusEvent` holds the user index and the cause: `Mouse`, `Navigation`, `SetDirectly`, `Cleared`,
`OtherWidgetLostFocus`, `WindowActivate`.

```das
options gen2
require ue_widget_events

[export]
def focused(geometry:UEGeometry; event:UEFocusEvent):UEEventReply {
    let user = ue_focus_user(event)   // uint
    let cause = ue_focus_cause(event) // e.g. "Navigation"
    return ue_handled()
}

[export]
def unfocused(event:UEFocusEvent):void {
    let cause = ue_focus_cause(event)
    // update UI state
}
```

Binding a focus handler does not set focus; the widget receives focus by normal Unreal rules.
`OnFocusLost` has no reply authority: it may change script state but cannot create reply commands.
`OnFocusChanging`, `OnAddedToFocusPath`, `OnRemovedFromFocusPath` are not supported.

## Geometry

`UEGeometry` is an owned snapshot of the exact `FGeometry` passed to the current `NativeOn…` call
(local size, layout and render transforms, pivot, rotation, mirroring). Later layout changes or the
next callback do not change a stored value; call a getter again for current layout. A native
`FGeometry()` with zero size before layout is a valid value; an empty DAS handle is not.

```das
require ue_api_umg_input
let widget = C_Widget_f2421369_from(ue_self_object())
let geometry = ue_widget_GetTickSpaceGeometry(widget)
let size = ue_ui_geometry_GetLocalSize(geometry)
let absolute = ue_ui_geometry_LocalToAbsolute(geometry, ue_vector2d(4.0lf, 6.0lf))
```

17 functions are available: 12 `SlateBlueprintLibrary` conversions/queries
(`ue_ui_geometry_AbsoluteToLocal`, `LocalToAbsolute`, `GetLocalSize`, `GetAbsoluteSize`,
`IsUnderLocation`, `Scalar_*`, `Vector_*`, `LocalToViewport`, `ScreenToWidgetLocal`, …), three widget
geometry getters and two `WidgetLayoutLibrary` getters.

- Non-finite values and float overflow are rejected; inverse transforms on degenerate geometry are
  rejected before the native `Inverse`.
- Calls are allowed on the gameplay game thread (not in frozen physics).
- Viewport conversions need a game world; without a viewport the native headless fallback applies.
  A null or expired player is rejected.
- `ue_property`, defaults, serialization and native containers of `UEGeometry` are not supported.

## Replies

`UEEventReply` is an opaque token valid only in the current synchronous callback. `ue_handled()` and
`ue_unhandled()` create a reply; commands return a new reply and do not modify the input:

| Helper (`ue_widget_events`) | Effect |
| --- | --- |
| `ue_reply_focus(reply, widget, all_users=false)`, `ue_reply_clear_focus(reply, all_users=false)` | Set / clear user focus |
| `ue_reply_capture_mouse(reply, widget)`, `ue_reply_release_mouse(reply)` | Mouse capture |
| `ue_reply_lock_mouse(reply, widget)`, `ue_reply_unlock_mouse(reply)` | Lock cursor to widget |
| `ue_reply_mouse_position(reply, x, y)` | Move cursor (integer screen coordinates) |
| `ue_reply_detect_drag(reply, widget, "LeftMouseButton")`, `ue_reply_end_drag(reply)` | Drag detection |

The generated wrappers in `ue_api_umg_input` provide the same commands with UE names: `Handled`,
`Unhandled`, `CaptureMouse`, `ReleaseMouseCapture`, `LockMouse`, `UnlockMouse`, `SetUserFocus`,
`ClearUserFocus`, `SetMousePosition`, `DetectDrag`, `EndDragDrop`, plus `DetectDragIfPressed`
(see [Drag & Drop](DragDrop.md)). Functions taking `FEventReply&` return a result struct with
`out_reply` and `out_returnvalue`, both referring to the new reply:

```das
require ue_api_umg_input

[export]
def mouse_down(geometry:UEGeometry; event:UEPointerEvent):UEEventReply {
    let widget=C_Widget_f2421369_from(ue_self_object())
    let capture=ue_ui_CaptureMouse(ue_ui_Handled(),widget)
    return capture.out_returnvalue
}
```

Rules:

- Commands require a handled reply and a live, constructed target widget in the same game world;
  weak targets are re-checked before the reply is returned to Slate.
- Commands only build a native `FReply`; Slate state changes after the handler returns successfully.
- At most 128 reply values per callback.
- A default reply, a reply from a previous callback, or creating a reply outside a synchronous handler
  is rejected. Storing a reply in a global, array, clone or call frame does not extend its life.
- `SetMousePosition` checks finite/int32 range and truncates fractions like native UMG. `DetectDrag`
  requires a mouse or touch key.

## Navigation queries

Three generated functions query the current Slate `NavigationConfig` (including custom per-user
policy):

- `ue_input_key_Key_GetNavigationActionFromKey(UEKeyEvent)`
- `ue_input_key_Key_GetNavigationDirectionFromKey(UEKeyEvent)`
- `ue_input_key_Key_GetNavigationDirectionFromAnalog(UEAnalogInputEvent)`

They return the generated enums `EUINavigation` / `EUINavigationAction` and do not move focus by
themselves (use focus APIs or reply commands for that).

```das
options gen2
require ue_api_umg_input
require ue_widget_events

[export]
def analog(geometry:UEGeometry; event:UEAnalogInputEvent):UEEventReply {
    let direction=ue_input_key_Key_GetNavigationDirectionFromAnalog(event)
    // use direction to select the next element of your UI
    return ue_handled()
}
```

Map: `InputHandlers[OnAnalogValueChanged] = analog`.

- The argument must be the snapshot of the current native input callback (or its clone in the same
  callback). Stored events from earlier callbacks remain readable but cannot be used for a live query.
  The widget, game world and original Slate user are checked.
- Each function's result is cached once per callback; the next event queries again.
- **Analog consumption:** the native analog query advances repeat timing. Two queries in one callback
  return the same result without advancing twice. Calling it means the script accepts the input: the
  reply is always handled. If the script returns unhandled, or fails/destroys the widget after the
  query, a bare handled reply is returned (with a `LastInputError` diagnostic) and other reply commands
  of a failed callback are dropped. Repeat state is not rolled back. Without an analog query the normal
  parent fallback applies. Key queries do not force handled.

## Hot reload and lifetime

Hot reload re-checks every bound signature and creates a new VM and callback scope; an unsuitable
candidate (compile error or a missing/changed handler) keeps the working version. Changing
`ScriptFile` releases the previous session. Native Slate repeat state survives reload.
`NativeDestruct` revokes current capabilities even if the UObject still exists; destroying the active
VM is deferred until execution finishes. No native closures or function pointers into the old VM are
kept in the widget.

## Limitations

- Handlers are for `UDasScriptUserWidget` only; authored `ue_class` widgets and `[ue_event]` methods
  are not input handlers.
- Motion events and focus-path notifications are not supported.
- Physical input devices and visible focus traversal are not covered by the automated scenarios.

Try it: `Scripts/TestWidgetEvents.ps1`.

See also: [Paint and Animation](PaintAndAnimation.md) · [Drag & Drop](DragDrop.md) ·
[Gameplay input](../Gameplay/Input.md) · [UMG / Input API](../../Api/UMGInput.md) ·
[Apply and hot reload](../Workflow/ApplyAndHotReload.md)
