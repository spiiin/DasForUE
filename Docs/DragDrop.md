# Drag & Drop

Публичный модуль `require ue_drag_drop`. Native родитель Widget Blueprint —
`DasScriptUserWidget`: `ScriptFile`, карта `DragHandlers` и `LastDragError`.
`BindDragHandler(Event, Handler)` подключает/удаляет обработчик; из DAS доступен
`ue_widget_bind_drag` для принадлежащих VM widgets. InputHandlers сохраняются
отдельно. Недоступный, ошибочный или reentrant handler использует native parent.

## Запуск

```dascript
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

Источник: `InputHandlers[OnMouseButtonDown] = mouse_down`,
`DragHandlers[OnDragDetected] = detected`. Приёмник:
`DragHandlers[OnDrop] = drop`. Slate вызывает OnDragDetected после достижения
порога движения; DetectDragIfPressed сам операцию ещё не создаёт.

PointerEvent для DetectDragIfPressed должен быть снимком именно текущего native
input callback. Его clone/копия сохраняет token и допустима в том же callback;
старые snapshots остаются читаемыми, но не дают права запросить новый drag.
Кнопка должна быть mouse/touch key. Совпадение effecting button или touch
возвращает handled + DetectDrag, несовпадение — unhandled, как native UE.

## Сигнатуры

Все handlers — `[export]`, аргументы по значению:

| Событие | Аргументы | Результат |
| --- | --- | --- |
| OnDragDetected | UEGeometry, UEPointerEvent | UObjectHandle новой операции или null |
| OnDragEnter | UEGeometry, UEPointerEvent, UObjectHandle | void |
| OnDragOver | UEGeometry, UEPointerEvent, UObjectHandle | bool |
| OnDragLeave | UEPointerEvent, UObjectHandle | void |
| OnDrop | UEGeometry, UEPointerEvent, UObjectHandle | bool |
| OnDragCancelled | UEPointerEvent, UObjectHandle | void |

Вместо FDragDropEvent передаётся owned snapshot его pointer-части; native shared
operation и EventPath в VM не попадают. Geometry/pointer можно сохранить и читать
после callback. Операция передаётся отдельно как обычный weak UObjectHandle. Проверяются
world, Slate user и pointer исходного drag.
`true` в over/drop означает handled; ошибка не публикует `true`.

## Операция и время жизни

`ue_drag_create(payload, visual, tag="", pivot=0, offset=ue_vector2d(0,0))`
разрешён только в OnDragDetected. Не более одной новой операции на callback;
вернуть можно только её либо null, если операция не создавалась. Ошибка после
создания отзывает операцию и очищает её данные.

Native класс — UDasDragDropOperation, наследник UDragDropOperation. Payload —
живой UObject из текущего world либо asset без world; null допустим. Visual —
отдельный constructed widget того же world; null использует стандартный текстовый
decorator UE с Tag. Pivot соответствует EDragPivot (0..9), offset конечный -1..1,
Tag не более 1024 UTF-16 units. До передачи в Slate объект удерживается native
scope; во время drag его и payload/visual удерживает FUMGDragDropOp.

`ue_drag_payload`, `ue_drag_visual`, `ue_drag_tag` разрешены только в callback
той же операции. `ue_drag_is_valid` можно вызвать и с сохранённым handle;
оно не продлевает жизнь. Лимит — 64 отслеживаемые операции на VM.

Native drop/cancel очищают Payload, DefaultDragVisual, Tag и delegates и навсегда
отзывают операцию. NativeDestruct и committed hot reload источника или уже
участвовавшего приёмника также отзывают её. Отклонённый reload оставляет рабочую
VM и операцию. Нет native callback, удерживающего VM pointer или DAS closure.

Если отозванная операция сейчас активна в Slate, на следующем core ticker
отменяется только совпадающий FUMGDragDropOp. Устаревшая отмена не должна
затронуть новую операцию. Чужие native/Blueprint subclasses не принимаются
managed callbacks; их поддержка и авторские operation classes — отдельный этап.

## Проверка

`Scripts/TestDragDrop.ps1 -SkipBuild -Publish`: fresh reflection/generation,
whole-module preflight и SObjectWidget → FUMGDragDropOp runtime scenario.
Итог: 47 native/runtime checks и 8 generator tests — PASS.
Проверяются generated detection, mouse/touch, stale pointer provenance,
payload/visual, enter/over/leave/drop/cancel, factory failures, GC,
source/target NativeDestruct и hot reload. Контракт —
`Bindings/DragDrop.contract.json`; fixture — `Tests/DragDrop/probe.das`;
отчёт — `logs/drag-validation.json`. Связанные регрессии: EventReply 33, OnPaint 28 — PASS.
Физический mouse drag, GPU rendering,
полные Foundation/PIE/packaged suites на этом этапе не запускались.
