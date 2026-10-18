# UMG: аудит 37 generation failures

UE `5.8.3-0+UE5`. Снимок: **366 ready, 114 export blocked, 37 generation blocked**.

Этот отчёт не меняет coverage; состояние реализации указано отдельно в контракте.

| Группа | Уникальных функций |
| --- | ---: |
| event_reply | 11 |
| geometry | 17 |
| paint_context | 5 |
| widget_animation_handle | 4 |

Input events: 0 отдельных функций; 0 кандидата на detached snapshot, 0 требуют отдельной navigation policy.
Cross-group: .

## Непрозрачные типы

| Native type | Затронутых функций |
| --- | ---: |
| `/Script/SlateCore.Geometry` | 17 |
| `/Script/UMG.EventReply` | 11 |
| `/Script/UMG.PaintContext` | 5 |
| `/Script/UMG.WidgetAnimationHandle` | 4 |

Числа типов пересекаются; таблица групп выше — непересекающееся разбиение функций.

## Поимённый backlog

| Функция | Группа | Поля и направления | Следующий барьер |
| --- | --- | --- | --- |
| SlateBlueprintLibrary.AbsoluteToLocal | geometry | `geometry`: Geometry (in) | separate_geometry_contract |
| SlateBlueprintLibrary.GetAbsoluteSize | geometry | `geometry`: Geometry (in) | separate_geometry_contract |
| SlateBlueprintLibrary.GetLocalSize | geometry | `geometry`: Geometry (in) | separate_geometry_contract |
| SlateBlueprintLibrary.GetLocalTopLeft | geometry | `geometry`: Geometry (in) | separate_geometry_contract |
| SlateBlueprintLibrary.IsUnderLocation | geometry | `geometry`: Geometry (in) | separate_geometry_contract |
| SlateBlueprintLibrary.LocalToAbsolute | geometry | `geometry`: Geometry (in) | separate_geometry_contract |
| SlateBlueprintLibrary.LocalToViewport | geometry | `geometry`: Geometry (in) | separate_geometry_contract |
| SlateBlueprintLibrary.Scalar_AbsoluteToLocal | geometry | `geometry`: Geometry (in) | separate_geometry_contract |
| SlateBlueprintLibrary.Scalar_LocalToAbsolute | geometry | `geometry`: Geometry (in) | separate_geometry_contract |
| SlateBlueprintLibrary.ScreenToWidgetLocal | geometry | `geometry`: Geometry (in) | separate_geometry_contract |
| SlateBlueprintLibrary.Vector_AbsoluteToLocal | geometry | `geometry`: Geometry (in) | separate_geometry_contract |
| SlateBlueprintLibrary.Vector_LocalToAbsolute | geometry | `geometry`: Geometry (in) | separate_geometry_contract |
| UserWidget.PlayAnimation | widget_animation_handle | `returnvalue`: WidgetAnimationHandle (out) | separate_widget_animation_handle_contract |
| UserWidget.PlayAnimationForward | widget_animation_handle | `returnvalue`: WidgetAnimationHandle (out) | separate_widget_animation_handle_contract |
| UserWidget.PlayAnimationReverse | widget_animation_handle | `returnvalue`: WidgetAnimationHandle (out) | separate_widget_animation_handle_contract |
| UserWidget.PlayAnimationTimeRange | widget_animation_handle | `returnvalue`: WidgetAnimationHandle (out) | separate_widget_animation_handle_contract |
| Widget.GetCachedGeometry | geometry | `returnvalue`: Geometry (out) | separate_geometry_contract |
| Widget.GetPaintSpaceGeometry | geometry | `returnvalue`: Geometry (out) | separate_geometry_contract |
| Widget.GetTickSpaceGeometry | geometry | `returnvalue`: Geometry (out) | separate_geometry_contract |
| WidgetBlueprintLibrary.CaptureMouse | event_reply | `reply`: EventReply (inout), `returnvalue`: EventReply (out) | separate_event_reply_contract |
| WidgetBlueprintLibrary.ClearUserFocus | event_reply | `reply`: EventReply (inout), `returnvalue`: EventReply (out) | separate_event_reply_contract |
| WidgetBlueprintLibrary.DetectDrag | event_reply | `reply`: EventReply (inout), `returnvalue`: EventReply (out) | separate_event_reply_contract |
| WidgetBlueprintLibrary.DrawBox | paint_context | `context`: PaintContext (inout) | separate_paint_context_contract |
| WidgetBlueprintLibrary.DrawLine | paint_context | `context`: PaintContext (inout) | separate_paint_context_contract |
| WidgetBlueprintLibrary.DrawLines | paint_context | `context`: PaintContext (inout) | separate_paint_context_contract |
| WidgetBlueprintLibrary.DrawSpline | paint_context | `context`: PaintContext (inout) | separate_paint_context_contract |
| WidgetBlueprintLibrary.DrawTextFormatted | paint_context | `context`: PaintContext (inout) | separate_paint_context_contract |
| WidgetBlueprintLibrary.EndDragDrop | event_reply | `reply`: EventReply (inout), `returnvalue`: EventReply (out) | separate_event_reply_contract |
| WidgetBlueprintLibrary.Handled | event_reply | `returnvalue`: EventReply (out) | separate_event_reply_contract |
| WidgetBlueprintLibrary.LockMouse | event_reply | `reply`: EventReply (inout), `returnvalue`: EventReply (out) | separate_event_reply_contract |
| WidgetBlueprintLibrary.ReleaseMouseCapture | event_reply | `reply`: EventReply (inout), `returnvalue`: EventReply (out) | separate_event_reply_contract |
| WidgetBlueprintLibrary.SetMousePosition | event_reply | `reply`: EventReply (inout), `returnvalue`: EventReply (out) | separate_event_reply_contract |
| WidgetBlueprintLibrary.SetUserFocus | event_reply | `reply`: EventReply (inout), `returnvalue`: EventReply (out) | separate_event_reply_contract |
| WidgetBlueprintLibrary.Unhandled | event_reply | `returnvalue`: EventReply (out) | separate_event_reply_contract |
| WidgetBlueprintLibrary.UnlockMouse | event_reply | `reply`: EventReply (inout), `returnvalue`: EventReply (out) | separate_event_reply_contract |
| WidgetLayoutLibrary.GetPlayerScreenWidgetGeometry | geometry | `returnvalue`: Geometry (out) | separate_geometry_contract |
| WidgetLayoutLibrary.GetViewportWidgetGeometry | geometry | `returnvalue`: Geometry (out) | separate_geometry_contract |

## Явные execution-contract отказы

33 input snapshot functions реализованы. Эти четыре функции остаются закрытыми:

| Функция | Причина |
| --- | --- |
| KismetInputLibrary.Key_GetNavigationActionFromKey | input events require exact scalar snapshot function allowlist; navigation/reply need live adapters |
| KismetInputLibrary.Key_GetNavigationDirectionFromAnalog | input events require exact scalar snapshot function allowlist; navigation/reply need live adapters |
| KismetInputLibrary.Key_GetNavigationDirectionFromKey | input events require exact scalar snapshot function allowlist; navigation/reply need live adapters |
| WidgetBlueprintLibrary.DetectDragIfPressed | input events require exact scalar snapshot function allowlist; navigation/reply need live adapters |

## Native source evidence

- input_storage: `Engine/Source/Runtime/SlateCore/Public/Input/Events.h` (SHA256 `7e534ed8373258f515d9f634741f8de9808b11bb7940ab662bcf6c41fe1147c1`); строки 154, 408, 413, 1135, 1114, 430, 526, 604, 693, 1254.
- path_dereference: `Engine/Source/Runtime/SlateCore/Private/Input/Events.cpp` (SHA256 `39955fa911629718f0d572d970be8f4d5b244dd55f18b35abef4150156aa6598`); строки 22, 27.
- event_projections: `Engine/Source/Runtime/UMG/Private/WidgetBlueprintLibrary.cpp` (SHA256 `6229bb54e725de53dcbec54c471c51af78a2951fac3dfae0c38b519f467dffe4`); строки 631, 636, 641, 646, 651.
- navigation_policy: `Engine/Source/Runtime/Slate/Private/Framework/Application/NavigationConfig.cpp` (SHA256 `2e5abc6be1b899c98ede483e5fca07a91959d4caa9d09dae0e4ea20dfa2a46c9`); строки 83, 90, 97, 98.
- navigation_dispatch: `Engine/Source/Runtime/Engine/Private/KismetInputLibrary.cpp` (SHA256 `ef39802ad963412e742a94e0f3af532b4f1ef1a3ab023d191d0148756f14b90c`); строки 114, 124, 134, 105.
- paint_and_callbacks: `Engine/Source/Runtime/UMG/Public/Blueprint/UserWidget.h` (SHA256 `4dc746ee00c41f01304b03a159cfd0693d3e30719c5697afb30f1a4ff819d636`); строки 177, 210, 212, 629, 693.
- reply: `Engine/Source/Runtime/UMG/Public/Components/SlateWrapperTypes.h` (SHA256 `60dc1f49b7444a113f2e4d3818cd335453fd0b98068542f67fb435a9982c7162`); строки 128, 141.
- geometry: `Engine/Source/Runtime/SlateCore/Public/Layout/Geometry.h` (SHA256 `b857272e3567da9fce115710ff9f2e09abcba1e6ef781c8647ab15df4e9fa347`); строки 39, 636.
- animation_handle: `Engine/Source/Runtime/UMG/Public/Animation/WidgetAnimationHandle.h` (SHA256 `fef4af705282d66d9ba8f3a46c997cc5d008128f8cf5e3073a780d256ac09c7f`); строки 21, 72, 46.

Контракт и порядок реализации: [InputEventTransportContract](InputEventTransportContract.md).
Input snapshots не реализуют UserWidget overrides, EventReply или paint callbacks автоматически.
Проверки: реальный повтор генерации на свежей reflection + unit tests аудита; без PIE/packaged/physical input.
