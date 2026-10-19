# Drag & Drop

`UDasScriptUserWidget` supports UMG drag & drop with daScript handlers. A drag source detects the
drag in its mouse handler, creates a managed drag operation in `OnDragDetected`, and target widgets
receive enter/over/leave/drop/cancel callbacks.

## Setup

Import `require ue_drag_drop`. The Widget Blueprint parent is `DasScriptUserWidget`, which has
`ScriptFile`, a `DragHandlers` map (event → exported function) and `LastDragError`.
`BindDragHandler(Event, Handler)` adds or removes a handler; from script use
`ue_widget_bind_drag(widget, script, event, handler)` for widgets owned by the VM. `InputHandlers`
(see [Widget events](WidgetEvents.md)) are stored separately. A missing, failing or re-entrant
handler falls back to the native parent.

## Example

```das
options gen2
require ue_api_umg_input
require ue_drag_drop

[export]
def mouse_down(geometry:UEGeometry; event:UEPointerEvent):UEEventReply {
    return ue_ui_DetectDragIfPressed(event,
        C_Widget_f2421369_from(ue_self_object()),
        ue_input_key_PointerEvent_GetEffectingButton(event))
}

[export]
def detected(geometry:UEGeometry; event:UEPointerEvent):UObjectHandle {
    var no_visual:UObjectHandle
    return ue_drag_create(ue_self_object(),no_visual,"Item")
}

[export]
def drop(geometry:UEGeometry; event:UEPointerEvent; operation:UObjectHandle):bool {
    let payload=ue_drag_payload(operation)
    print("Dropped: {ue_drag_tag(operation)}\n")
    return ue_object_valid(payload)
}
```

- Source widget: `InputHandlers[OnMouseButtonDown] = mouse_down`,
  `DragHandlers[OnDragDetected] = detected`.
- Target widget: `DragHandlers[OnDrop] = drop`.

Slate calls `OnDragDetected` once the pointer moves past the drag threshold;
`DetectDragIfPressed` only requests detection and does not create the operation itself.

### DetectDragIfPressed

The `UEPointerEvent` passed to `ue_ui_DetectDragIfPressed` must be the snapshot of the current native
input callback; a clone of it in the same callback is fine. Older snapshots stay readable but cannot
start a new drag. The key must be a mouse or touch key. If the effecting button or touch matches,
the result is handled + DetectDrag; otherwise unhandled, as in native UE. (`ue_reply_detect_drag`
from `ue_widget_events` is the lower-level equivalent.)

## Handler signatures

All handlers are `[export]` and take arguments by value:

| Event | Arguments | Result |
| --- | --- | --- |
| `OnDragDetected` | `UEGeometry`, `UEPointerEvent` | `UObjectHandle` of the new operation, or null |
| `OnDragEnter` | `UEGeometry`, `UEPointerEvent`, `UObjectHandle` | `void` |
| `OnDragOver` | `UEGeometry`, `UEPointerEvent`, `UObjectHandle` | `bool` |
| `OnDragLeave` | `UEPointerEvent`, `UObjectHandle` | `void` |
| `OnDrop` | `UEGeometry`, `UEPointerEvent`, `UObjectHandle` | `bool` |
| `OnDragCancelled` | `UEPointerEvent`, `UObjectHandle` | `void` |

Instead of `FDragDropEvent` the handler receives an owned snapshot of its pointer part; the native
shared operation and event path never enter the VM. Geometry and pointer snapshots may be stored and
read after the callback. The operation is passed separately as a weak `UObjectHandle`. World, Slate
user and pointer of the original drag are checked. Returning `true` from over/drop means handled; a
failing handler never publishes `true`.

## Creating an operation

```das
ue_drag_create(payload, visual, tag="", pivot=0, offset=ue_vector2d(0.0lf,0.0lf)):UObjectHandle
```

- Allowed only inside `OnDragDetected`. At most one new operation per callback; the handler must
  return that operation or null if none was created. An error after creation revokes the operation and
  clears its data.
- The native class is `UDasDragDropOperation` (a `UDragDropOperation` subclass).
- `payload`: a live UObject of the current world, or an asset without a world; null is allowed.
- `visual`: a separate constructed widget of the same world; null uses UE's standard text decorator
  showing `tag`.
- `pivot`: `EDragPivot` value 0..9. `offset`: finite, each component in -1..1. `tag`: up to 1024
  UTF-16 code units.
- Before it reaches Slate, the operation is held by a native scope; during the drag, the operation,
  payload and visual are held by `FUMGDragDropOp`.

## Reading an operation

| Function | Where allowed |
| --- | --- |
| `ue_drag_payload(op)`, `ue_drag_visual(op)`, `ue_drag_tag(op)` | Only in a callback of that same operation |
| `ue_drag_is_valid(op)` | Anywhere, including with a stored handle; does not extend lifetime |

Up to 64 tracked operations per VM.

## Lifetime and revocation

- Native drop or cancel clears `Payload`, `DefaultDragVisual`, `Tag` and delegates and revokes the
  operation permanently.
- `NativeDestruct` or a committed hot reload of the source, or of a target that already participated,
  also revokes it. A rejected reload keeps the working VM and the operation.
- If a revoked operation is currently active in Slate, the matching `FUMGDragDropOp` is cancelled on
  the next core tick; a stale cancel never affects a newer drag.
- No native callback holds a VM pointer or DAS closure.

## Limitations

- Foreign native or Blueprint `UDragDropOperation` subclasses are not accepted by managed callbacks,
  and authored operation classes are not supported; use `ue_drag_create`.
- The raw `CreateDragDropOperation` is not exposed.
- Physical mouse drag and rendering are not covered by the automated scenario.

Try it: `Scripts/TestDragDrop.ps1`.

See also: [Widget input and focus events](WidgetEvents.md) · [Paint and Animation](PaintAndAnimation.md) ·
[UMG / Input API](../../Api/UMGInput.md)
