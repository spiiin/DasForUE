# Image soft brush

`UImage::SetBrushFromSoftTexture` and `SetBrushFromSoftMaterial` are generated in
the UMG / Input package through the exact `managed_soft_brush_v1` call adapter.
The native function is not called; instead the runtime starts a managed async
load and applies the brush with `SetBrushFromTexture` / `SetBrushFromMaterial`
after completion, preserving `bMatchSize`.

The request is owned by the calling VM context and bound to the Image, its
owning live game-world UserWidget and their Slate widgets. It is cancelled by a
newer brush request or any other generated `SetBrush*` / `GetDynamicMaterial`
call on the same Image, by owner `NativeDestruct`, EndPlay, release and committed
hot reload. A completion is ignored if the brush was changed in the meantime.
At most 256 pending requests exist per context.

`require ue_image_soft_brush`:

- `ue_image_soft_brush_state(image):string`: `none`, `loading`, `ready`,
  `failed` or `cancelled` for the request made in the same VM.
- `ue_image_soft_brush_error(image):string`.

## Status

The adapter is compiled; the UMG / Input package generates and passes the
whole-module preflight with these two functions (409 ready, 108 export blocked,
staged, not published). A runtime scenario with real async loading,
cancellation and widget destruction has not been run yet.
