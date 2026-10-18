# Синхронные обработчики UserWidget

`UDasScriptUserWidget` — native родитель для Widget Blueprint с синхронными
обработчиками daScript. В Widget Blueprint задаются `ScriptFile` (имя `.das`
в каталоге Scripts) и `InputHandlers` (имя UE-события → exported DAS-функция).
Виджет можно создать обычным `CreateWidget` или существующим `ue_widget_create`.
Для клавиатурного ввода ему, как и обычному UserWidget, нужны focusable и focus.

```dascript
options gen2
require ue_widget_events
require ue_api_umg_input

[export]
def key_down(event:UEKeyEvent):UEEventReply {
    if (ue_input_key_GetKey(event).f_keyname.value=="SpaceBar") {
        return ue_handled()
    }
    return ue_unhandled()
}

[export]
def mouse_down(event:UEPointerEvent):UEEventReply {
    return ue_reply_capture_mouse(ue_handled(),ue_self_object())
}

[export]
def mouse_up(event:UEPointerEvent):UEEventReply {
    return ue_reply_release_mouse(ue_handled())
}
```

Для этого примера в `InputHandlers`: `OnKeyDown → key_down`,
`OnMouseButtonDown → mouse_down`, `OnMouseButtonUp → mouse_up`.
Внутри обработчика `ue_self_object()` — сам виджет. Снимок события можно
сохранить в DAS-переменной или массиве и читать после возврата из callback.

Для виджета, созданного и принадлежащего текущему скриптовому контексту,
доступно подключение до показа:

```dascript
ue_widget_bind_input(widget,"my_widget.das","OnKeyDown","key_down")
```

Все обработчики одного виджета используют один `ScriptFile` и контекст;
смена файла через этот helper освобождает прежний контекст и bindings.
Пустое имя handler снимает конкретную привязку. `NativeDestruct` освобождает
контекст виджета, snapshots и принадлежащие ему managed ресурсы.

## События

| Событие | Единственный параметр обработчика |
| --- | --- |
| OnKeyChar | UECharacterEvent |
| OnPreviewKeyDown, OnKeyDown, OnKeyUp | UEKeyEvent |
| OnAnalogValueChanged | UEAnalogInputEvent |
| OnMouseButtonDown, OnPreviewMouseButtonDown, OnMouseButtonUp | UEPointerEvent |
| OnMouseMove, OnMouseWheel, OnMouseButtonDoubleClick | UEPointerEvent |
| OnTouchGesture, OnTouchStarted, OnTouchMoved, OnTouchEnded | UEPointerEvent |
| OnTouchForceChanged, OnTouchFirstMove | UEPointerEvent |

Нужна точная сигнатура `[export] def handler(event:TYPE):UEEventReply`.
Вызов происходит на game thread непосредственно из `NativeOn…` и возвращает
`FReply` обратно в Slate в том же стеке. Отложенные ticker callbacks не используются.
Явный `Unhandled` передаётся в Slate без повторного вызова родителя. Если привязки
нет, файл не задан или обработчик завершился ошибкой, вызывается `Super::NativeOn…`.
Причина последней ошибки доступна в `LastInputError`.

## Reply и его время жизни

`UEEventReply` — opaque token команд текущего callback. `ue_handled()` и
`ue_unhandled()` создают reply. Команды возвращают новое значение; исходное не
изменяется:

- `ue_reply_focus(reply,widget,all_users=false)`, `ue_reply_clear_focus`;
- `ue_reply_capture_mouse`, `ue_reply_release_mouse`;
- `ue_reply_lock_mouse`, `ue_reply_unlock_mouse`;
- `ue_reply_mouse_position(reply,x,y)` — целые экранные координаты;
- `ue_reply_detect_drag(reply,widget,"LeftMouseButton")`, `ue_reply_end_drag`.

Команды требуют handled reply и уже построенный widget в том же игровом мире.
Они формируют native FReply и не меняют состояние Slate во время выполнения DAS.
Slate получает команды только после успешного возврата. Неудачный обработчик
не публикует частично построенный reply. Лимит — 128 reply values на callback.
Default reply, reply из предыдущего вызова и создание reply вне синхронного
обработчика отклоняются. Сохранение токена в global не продлевает его жизнь.
Синхронный повторный вход в VM из Slate не разрешён.

Это отдельный scoped bridge: generic reflected transport для `/Script/UMG.EventReply`
не открыт. Передача Geometry в обработчик, focus/motion events, paint, drag/drop operation payloads,
авторские `ue_class`-виджеты и `[ue_event]`-аннотации требуют следующих этапов.
Число функций в generated UMG-пакете этим bridge не меняется.

## Проверка

`Scripts/TestWidgetEvents.ps1` выполняет Editor build и один короткий native/DAS
runtime-сценарий через SObjectWidget. В нём проверяются handled/unhandled,
типизированные key/character/analog/pointer callbacks, focus/capture/lock/drag
команды, снимок после callback и GC, отказы default/stale reply, исключение без
частичных команд, неверная сигнатура и очистка при NativeDestruct.
С готовыми DLL можно использовать `-SkipBuild`. Отчёт:
`logs/widget-events-validation.json`. Полные PIE/packaged и физический ввод
этот runner не запускает. Полный исполняемый пример: `Tests/WidgetEvents/probe.das`.

Проверено 2026-10-18 на UE 5.8.3: Editor build — PASS; один runtime-сценарий —
PASS, 17 проверок, exit code 0, без engine Error/ensure. Null application и
null renderer используются только в test harness. Исходники, DLL и Engine BuildId
стабильны во время принятой проверки. Изменения без коммита.
