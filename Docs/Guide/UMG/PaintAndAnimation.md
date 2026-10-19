# Paint, Widget Animation and Soft Brushes

This page covers three UMG features of `ue_api_umg_input`: custom drawing in a synchronous `OnPaint`
handler, playing widget animations through a weak handle, and setting an Image brush from a soft
texture or material with managed async loading.

## Custom painting (OnPaint)

In a Widget Blueprint with parent `DasScriptUserWidget`, set `ScriptFile` and `PaintHandler`. The
handler has an exact signature `(context:UEPaintContext):void`:

```das
options gen2
require ue_api_umg_input

[export]
def paint(context:UEPaintContext):void {
    let tint=S_LinearColor_f4abb14b(f_r=1.0,f_g=1.0,f_b=1.0,f_a=1.0)
    let updated=ue_ui_DrawLine(context,
        ue_vector2d(5.0lf,5.0lf),ue_vector2d(60.0lf,30.0lf),tint,true,1.0)
}
```

`ue_api_umg_input` re-exports `ue_paint`. Available draw functions:

| Function | Parameters after `context` |
| --- | --- |
| `ue_ui_DrawBox` | position, size, `C_SlateBrushAsset_…` brush, tint |
| `ue_ui_DrawLine` | point A, point B, tint, antialias, thickness |
| `ue_ui_DrawLines` | `array<UEVector2D>` points, tint, antialias, thickness |
| `ue_ui_DrawSpline` | start, start dir, end, end dir, tint, thickness |
| `ue_ui_DrawTextFormatted` | `UETextValue` text, position, `C_Font_…` font, font size, typeface `UEName`, tint |

Each returns a `UEPaintContext` for the same callback; you may use either the original or the
returned value for the next command.

### How drawing is committed

- `Super::NativePaint` runs first; script commands are drawn on top of its final layer. The real
  `MaxLayer` of the native draw functions is returned.
- During the handler only the owned command parameters are recorded (vectors, colors, points, text,
  resource references); brush and font are pinned until the end of the callback.
- Native draw functions run in order only after the handler returns successfully and the widget is
  re-checked. An error, panic, invalid token, exceeded limit or `NativeDestruct` cancels the whole
  script batch; the parent drawing stays. The parent draw list is never cleared or replaced.
- Native no-op behavior (empty/degenerate `DrawLines`, null brush or font) is preserved.
- The last error is in `LastPaintError`.

### Paint context lifetime

`UEPaintContext` is an opaque token of the current `NativePaint` call. Geometry, culling rect, widget
style and draw list stay on the native stack. Storing the token in a global, array or clone does not
extend it: a default token, a token from another callback or context, old call frames and calls
outside `OnPaint` are rejected. Paint is never queued on a ticker. Re-entering the VM from paint is
rejected and the native parent is used. Reload and `NativeDestruct` release the context.
Reply commands (focus, capture, …) are not allowed in paint.

### Limits

| Limit | Value |
| --- | --- |
| Commands per callback | 1024 |
| Points per call / per callback | 4096 / 65536 |
| Text per call / per callback | 32768 / 131072 UTF-16 code units |
| Coordinates, scalars, colors | finite, absolute value ≤ 1e7; box size non-negative |
| Thickness | 0..10000 |
| FontSize | > 0 and ≤ 4096 |

Slate float transform and layer overflow are checked. Snapshots, reflected storage/defaults, JSON and
native containers of `UEPaintContext` are not supported. The widget must be a live game-world widget
on the gameplay game thread with an idle VM.

Try it: `Scripts/TestPaint.ps1`.

## Widget animations

`ue_api_umg_input` re-exports `ue_widget_animation`. Four functions return a
`UEWidgetAnimationHandle`: `ue_user_widget_PlayAnimation`, `ue_user_widget_PlayAnimationForward`,
`ue_user_widget_PlayAnimationReverse`, `ue_user_widget_PlayAnimationTimeRange`.

```das
require ue_api_umg_input

let handle=ue_user_widget_PlayAnimationForward(widget,animation,1.0,false)
if(ue_widget_animation_is_valid(handle)) {
    let playing=ue_widget_animation_is_playing(handle)
}
```

`widget` and `animation` are the generated wrappers `C_UserWidget_…` and `C_WidgetAnimation_…`.
For the full `PlayAnimation` call pass start time, loop count, play mode, speed and restore-state
explicitly (the package uses an explicit-defaults policy).

### Handle semantics

- The handle is a capability owned by one VM, not a copy of the animation state. The native handle is
  weak: it does not keep the widget or the animation alive.
- `ue_widget_animation_is_valid` checks native state, widget/world and revocation;
  `ue_widget_animation_is_playing` additionally checks that it is playing. Pause does not revoke a
  handle. Default, finished, destroyed and expired handles return `false`. (Native
  `FWidgetAnimationHandle::IsValid` alone is not enough: it may report `true` while finished state is
  still held.)
- Finish and `NativeDestruct` revoke the handle permanently; replaying or re-constructing the same
  widget does not revive old handles.
- Globals, arrays and clones may hold handles (up to 4096 values in the shared snapshot table).
- Reload or release drops the old context's handles but does not stop the UE animation itself.

Calls require a constructed widget in the current context's game world, a live animation asset,
finite non-negative time, positive speed and a non-negative loop count (0 = infinite). Native input,
reflected `ue_property`/defaults, JSON and native containers of the handle are not supported.

Try it: `Scripts/TestWidgetAnimation.ps1`.

## Image soft brush

`UImage::SetBrushFromSoftTexture` and `SetBrushFromSoftMaterial` are available as
`ue_image_SetBrushFromSoftTexture` and `ue_image_SetBrushFromSoftMaterial`. Instead of the native
blocking call, the runtime starts a managed async load and applies the brush with
`SetBrushFromTexture` / `SetBrushFromMaterial` on completion (preserving `bMatchSize`).

```das
require ue_api_umg_input

typedef SoftIcon = $UESoftObject<C_Texture2D_2db8ea5c>

def show_icon(image:C_Image_61e460fb; path:string) {
    ue_image_SetBrushFromSoftTexture(image, ue_soft(type<SoftIcon>, path), true)
}
// Later, in the same VM:
// ue_image_soft_brush_state(C_Image_61e460fb_raw(image)) == "ready"
```

`ue_image_soft_brush` (re-exported by the package) provides:

- `ue_image_soft_brush_state(image):string` — `none`, `loading`, `ready`, `failed` or `cancelled` for
  the request made in the same VM;
- `ue_image_soft_brush_error(image):string`.

Semantics:

- An already loaded asset is applied synchronously; a null soft reference clears the brush.
- The request belongs to the calling VM context and is bound to the Image and its owning live
  game-world UserWidget. It is cancelled by a newer brush request, by any other generated `SetBrush*`
  or `GetDynamicMaterial` call on the same Image, by owner `NativeDestruct`, EndPlay, release, and by a
  committed hot reload.
- A completion is ignored if the brush was changed meanwhile (including by native code).
- At most 256 pending requests per context. Requests never pin the Image, widget or loaded asset.
- Errors are raised before any load starts: Image not constructed or not in a live game-world
  UserWidget, invalid path, asset missing from the Asset Registry, or class mismatch.

Try it: `Scripts/TestImageSoftBrush.ps1`.

See also: [Widget input and focus events](WidgetEvents.md) · [Drag & Drop](DragDrop.md) ·
[Soft references](../DSL/SoftReferences.md) · [Asset loading](../Data/AssetLoading.md) ·
[UMG / Input API](../../Api/UMGInput.md)
