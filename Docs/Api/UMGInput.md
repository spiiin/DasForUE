# UMG / Input Package (ue_api_umg_input)

`ue_api_umg_input` wraps the reflected API of UMG widgets and layout libraries, `KismetInputLibrary` key utilities and Enhanced Input. It is installed in the plugin scripts and generated from `Bindings/Packages/UMGInput.profile.json` (UE modules Engine, InputCore, SlateCore, Slate, UMG, EnhancedInput). It is a call package: authoring widgets, overriding widget events and binding input actions are handled by the managed modules linked below.

```das
options gen2
require ue_api_umg_input

[export]
def main(input:int):int {
    let move=ue_input_value(ue_vector2d(1.0lf,0.0lf))
    let parts=ue_enhanced_api_BreakInputActionValue(move)
    return parts.out_x==1.0lf ? 42 : -1
}
```

## Contents

| Prefix | Class or library |
| --- | --- |
| `ue_ui_` | `WidgetBlueprintLibrary` |
| `ue_ui_layout_` | `WidgetLayoutLibrary` |
| `ue_ui_geometry_` | `SlateBlueprintLibrary` |
| `ue_input_key_` | `KismetInputLibrary` |
| `ue_enhanced_api_` | `EnhancedInputLibrary` |
| `ue_widget_`, `ue_user_widget_`, `ue_panel_`, `ue_content_widget_` | `Widget`, `UserWidget`, `PanelWidget`, `ContentWidget` |
| `ue_button_`, `ue_text_block_`, `ue_editable_text_`, `ue_image_`, `ue_progress_`, `ue_slider_`, `ue_scroll_`, `ue_key_selector_` | Common widgets |
| `ue_canvas_`, `ue_canvas_slot_` | `CanvasPanel`, `CanvasPanelSlot` |
| `ue_input_component_`, `ue_input_settings_` | `InputComponent`, `InputSettings` |
| `ue_enhanced_component_`, `ue_mapping_context_`, `ue_input_local_`, `ue_input_world_` | `EnhancedInputComponent`, `InputMappingContext`, `EnhancedInputLocalPlayerSubsystem`, `EnhancedInputWorldSubsystem` |

About 410 wrappers are generated: widget setters and getters, panels and canvas slots, text, buttons, images, progress bars, sliders, scroll boxes, layout and viewport helpers, FKey utilities and the reflected `InputSettings`/Enhanced Input methods.

```das
ue_text_block_SetText(label, ue_text("MyGame","Score","Score"))
ue_progress_SetPercent(bar, 0.75)
ue_widget_SetVisibility(panel, E_ESlateVisibility_d128c9c0.v_Collapsed)
ue_user_widget_SetPositionInViewport(hud, ue_vector2d(20.0lf,20.0lf), true)
let viewport : UEVector2D = ue_ui_layout_GetViewportSize()
```

All arguments are explicit; Vector2D uses `UEVector2D`; text parameters take `UETextValue`.

## Values with dedicated transports

Several UMG types are opaque engine values. They are available through checked adapters, each with its own lifetime rule:

| Value | Where it is valid | Page |
| --- | --- | --- |
| `InputActionValue` (`ue_input_value`, `ue_input_type`, `ue_input_axis1d/2d/3d`, `ue_input_bool`) | Owned value, usable anywhere | [Input](../Guide/Gameplay/Input.md) |
| Input events (key, pointer, focus, motion …) | Owned snapshots | [Widget events](../Guide/UMG/WidgetEvents.md) |
| Live navigation queries | Only with the current native input event and Slate user | [Widget events](../Guide/UMG/WidgetEvents.md) |
| `Geometry` | Owned snapshot of one layout revision | [Widget events](../Guide/UMG/WidgetEvents.md) |
| `EventReply` | Only inside the current synchronous callback | [Widget events](../Guide/UMG/WidgetEvents.md) |
| `DetectDragIfPressed`, drag & drop | Current synchronous pointer callback | [Drag and drop](../Guide/UMG/DragDrop.md) |
| PaintContext draw functions | Synchronous `OnPaint` only | [Paint and animation](../Guide/UMG/PaintAndAnimation.md) |
| `WidgetAnimation` handles | Weak handle | [Paint and animation](../Guide/UMG/PaintAndAnimation.md) |
| Image soft brush (`SetBrushFromSoftTexture`/`Material`) | Managed async load | [Paint and animation](../Guide/UMG/PaintAndAnimation.md) |

### Delegate parameters

Native single-cast delegate parameters take a `UEFunctionDelegate` that names an existing `UFunction` with exactly the delegate's signature (including return/out/ref):

```das
let endpoint = ue_function_delegate(target, "ChooseNavigation")
ue_widget_SetNavigationRuleCustom(widget, E_EUINavigation_e4554eeb.v_Right, endpoint)
```

`target` is a checked reference or `UObjectHandle`. The delegate kind, its native signature and the handler's layout are checked in the setter's call frame; an incompatible handler is rejected. The target is held until the call. The stored Unreal delegate is weak. These endpoints are not subscriptions to free DAS functions and are not unbound automatically: call the matching native Unbind/Remove API (or pass `ue_function_delegate_null()` where the setter accepts an empty delegate). Only scalar input-only single-cast delegate parameters are supported; delegate outputs and containers of delegates are not.

## Managed alternatives

- Widget creation, showing, text and click binding for HUDs: `ue_widget_create`, `ue_widget_show`, `ue_widget_set_text`, `ue_widget_bind_click` (module `ue_gameplay_services`).
- Enhanced Input action bindings, mapping contexts and typed callbacks: [Input](../Guide/Gameplay/Input.md).
- Native widget event handlers (`NativeOn*`) through the `DasScriptUserWidget` parent: [Widget events](../Guide/UMG/WidgetEvents.md).

## Not included

- No callable reflected API for `InputComponent`, `EnhancedInputComponent` and `EnhancedInputLocalPlayerSubsystem`: their functions are protected legacy input, Blueprint events or non-reflected. Use the managed input modules. `InputAction` has no functions of its own but appears as a typed reference.
- About 110 export refusals: deprecated functions, delegate signature declarations (type definitions, not subscriptions), protected legacy input, non-callable accessors, editor helpers, and technical blockers such as widget lifecycle overrides (`Construct`/`Destruct`), focus-path and pointer notifications, animation-finished callbacks and `GetBoundActionValue`.
- `UserWidget` override events do not become ordinary calls just because they are listed by the exporter.

Named lists: [UMGExportBlockers](../Dev/Api/Blockers/UMGExportBlockers.md), [UMGInputBlockers](../Dev/Api/Blockers/UMGInputBlockers.md), `Bindings/Packages/ue_api_umg_input.summary.json`.

Try it: `.\Scripts\TestUMGInputPackage.ps1 -SkipBuild` (InputActionValue probe); `.\Scripts\GenerateApiPackage.ps1 -Profile .\Bindings\Packages\UMGInput.profile.json -SkipBuild -Publish` reinstalls the module.

See also: [API packages](README.md) · [Input](../Guide/Gameplay/Input.md) · [Widget events](../Guide/UMG/WidgetEvents.md) · [Paint and animation](../Guide/UMG/PaintAndAnimation.md)
