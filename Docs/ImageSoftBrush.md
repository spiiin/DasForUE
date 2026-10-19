# Image soft brush

`UImage::SetBrushFromSoftTexture` and `SetBrushFromSoftMaterial` are published in
`ue_api_umg_input` through the exact `managed_soft_brush_v1` call adapter.
The native function is not called; instead the runtime starts a managed async
load and applies the brush with `SetBrushFromTexture` / `SetBrushFromMaterial`
after completion, preserving `bMatchSize`.

```das
require ue_api_umg_input

typedef SoftIcon = $UESoftObject<C_Texture2D_2db8ea5c>

def show_icon(image:C_Image_61e460fb; path:string) {
    ue_image_SetBrushFromSoftTexture(image, ue_soft(type<SoftIcon>, path), true)
}
// Later, in the same VM:
// ue_image_soft_brush_state(C_Image_61e460fb_raw(image)) == "ready"
```

The request is owned by the calling VM context and bound to the Image, its
owning live game-world UserWidget and their Slate widgets. It is cancelled by a
newer brush request or any other generated `SetBrush*` / `GetDynamicMaterial`
call on the same Image, by owner `NativeDestruct`, EndPlay, release and committed
hot reload. A completion is ignored if the brush was changed in the meantime,
including by native code. At most 256 pending requests exist per context.
Requests never pin the Image, widget or loaded asset.

An already loaded asset is applied synchronously; a null soft reference clears
the brush. Errors are raised before any load starts: an Image that is not
constructed or not in a live game-world UserWidget, an invalid path, an asset
missing from the Asset Registry, or an asset whose loaded or registered class
does not match.

`require ue_image_soft_brush` (re-exported by the package):

- `ue_image_soft_brush_state(image):string`: `none`, `loading`, `ready`,
  `failed` or `cancelled` for the request made in the same VM.
- `ue_image_soft_brush_error(image):string`.

## Validation

`Scripts/TestImageSoftBrush.ps1 [-SkipBuild] [-Publish]` regenerates the UMG / Input
package (409 ready, 108 export blocked), passes the whole-module preflight and
runs the `DasImageSoftBrush` commandlet: temporary on-disk textures and a material
on a private mount, a real UserWidget with an Image in a game world and NullSlate,
real async loading, supersession, cancellation by a generated setter, an external
brush change, NativeDestruct, hot reload and owner release, type/registry errors
and GC (22 checks). Report: `logs/image-soft-brush-validation.json`.

The NullRHI editor builds no texture platform data, so `bMatchSize` is verified
as "uses the size UE reports" rather than a real pixel size. Visible rendering,
PIE and packaged runs are not covered.
