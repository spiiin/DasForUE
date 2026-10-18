# EventReply в сгенерированных обёртках

`require ue_api_umg_input` публично подключает `ue_widget_events`. Все 11
EventReply wrappers используют существующий синхронный bridge UserWidget.
Пример handler для `UDasScriptUserWidget`, `OnMouseButtonDown`:

```dascript
require ue_api_umg_input
[export]
def mouse_down(geometry:UEGeometry; event:UEPointerEvent):UEEventReply {
    let widget=C_Widget_f2421369_from(ue_self_object())
    let capture=ue_ui_CaptureMouse(ue_ui_Handled(),widget)
    return capture.out_returnvalue
}
```

## Открытые функции

Handled, Unhandled, CaptureMouse, ReleaseMouseCapture, LockMouse, UnlockMouse,
SetUserFocus, ClearUserFocus, SetMousePosition, DetectDrag, EndDragDrop.
Точный native allowlist: `Bindings/EventReply.contract.json`.

Функции с `FEventReply&` возвращают generated result struct с `out_reply`
и `out_returnvalue`. Оба поля ссылаются на новый reply текущего callback;
исходный входной токен не изменяется. Это обычное правило generated inout
обёрток. Команды реализованы общими managed adapters; второй реализации
focus/capture/drag и хранения native FReply в persistent frames нет.

## Время жизни и отказы

- Создание frame, запись аргумента, dispatch и чтение результата требуют
  активного синхронного widget callback, текущей VM и уникального callback epoch.
- Сохранённый reply не работает в следующем callback. Сохранённые call frames
  удаляются при выходе из callback, включая ошибку, вместе с object pins.
- Reload создаёт новый контекст; NativeDestruct освобождает старый.
  Нельзя продлить жизнь reply через global, clone, массив или call frame.
- Команды требуют handled reply. Target — живой построенный widget текущего
  игрового мира; weak targets повторно проверяются перед возвратом в Slate.
- SetMousePosition принимает FVector2D, проверяет finite/int32 range, затем
  отбрасывает дробную часть как native UMG. DetectDrag требует mouse/touch key.
- После ошибки обработчика Slate получает native parent fallback без частично
  построенных команд. Лимит — 128 reply values на callback.
- Reflected ue_property/defaults, JSON и native контейнеры EventReply закрыты.
  DetectDragIfPressed и три navigation queries остаются отдельными отказами.

## Целевые проверки

`Scripts/TestEventReply.ps1 -SkipBuild -Publish` проверяет весь generated модуль,
11 ready функций и вызовы через SObjectWidget с null application/renderer.
Проверяются команды и оба inout/result слота, stale/default tokens, pending и
executed frames после callback, result до invoke, missing input, координаты,
неверная drag key, чужой/null target, limit, panic без частичных команд,
возврат после reload и очистка при NativeDestruct. Отдельно — три compile-time
попытки подделать token и восемь generator tests. Source/DLL/BuildId сверяются.
Отчёт: `logs/event-reply-validation.json`. Полные Foundation/PIE/packaged,
физический ввод и видимое окно не запускаются. Коммит не создаётся.


Для [analog navigation](InputNavigation.md) действует consuming-контракт: после
нативного запроса repeat state уже изменён. Ошибка, Destroy или unhandled reply
возвращают bare handled, исключая повторную обработку того же analog input в UE.
Остальные reply-команды ошибочного callback отбрасываются.
