# Live input navigation

В `require ue_api_umg_input` доступны:

- `ue_input_key_Key_GetNavigationActionFromKey(UEKeyEvent)`;
- `ue_input_key_Key_GetNavigationDirectionFromKey(UEKeyEvent)`;
- `ue_input_key_Key_GetNavigationDirectionFromAnalog(UEAnalogInputEvent)`.

Это вызовы текущего Slate NavigationConfig, включая custom/per-user policy.
Параметром может быть только snapshot текущего native input callback на
DasScriptUserWidget. Clone/копия в том же callback допустима; сохранённое событие
из предыдущего входа, заново созданный snapshot, другой тип, VM или старый call
frame не дают права запросить live navigation. Проверяются epoch, живой widget,
game world, исходный зарегистрированный Slate user и gameplay game thread.

Результат каждого запроса кэшируется один раз на функцию в пределах одного
native callback. Следующее событие выполняет новый запрос, даже если его данные
полностью совпадают. Конфигурация Slate не заменяется и не копируется в DAS.

## Analog consumption

Native GetNavigationDirectionFromAnalog меняет repeat timing и счётчик нажатий
в NavigationConfig. Два запроса внутри одного callback возвращают один результат
и не продвигают счётчик дважды. Следующий callback видит штатное подавление
повтора; выдержка времени и neutral axis работают по правилам native config.

Вызов analog query означает принятие скриптом обработки этого input. После
него native reply всегда handled. Если скрипт вернул unhandled, он заменяется
на bare handled с диагностикой LastInputError. При panic/Destroy после запроса
также возвращается bare handled и исходная ошибка; capture/focus и другие
reply-команды ошибочного callback не публикуются. Repeat state не откатывается.
Это исключает второе потребление того же события штатной UE-навигацией.

Без analog query обычный unhandled/error сохраняет parent fallback. Ошибочная
сигнатура, stale event, отсутствие user или Destroy до запроса отклоняются до
изменения repeat state. Key queries не требуют принудительного handled.

## Пример

```dascript
options gen2
require ue_api_umg_input
require ue_widget_events

[export]
def analog(geometry:UEGeometry; event:UEAnalogInputEvent):UEEventReply {
    let direction=ue_input_key_Key_GetNavigationDirectionFromAnalog(event)
    // Используйте direction, чтобы выбрать следующий элемент своего UI.
    return ue_handled()
}
```

`InputHandlers[OnAnalogValueChanged] = analog`. Результаты — generated enums
EUINavigation/EUINavigationAction, без строковых или числовых замен типов.
Получение результата само по себе не перемещает focus: для этого используются
обычные focus/navigation API или scoped EventReply команды.

## Жизненный цикл и проверки

NativeDestruct отзывает текущую navigation capability, даже если UObject ещё
существует. Call frames освобождаются при выходе из callback. Hot reload
проверяет использованные сигнатуры, создаёт новую VM и новый callback scope;
native repeat state Slate сохраняется. Detached input data остаются читаемыми
после callback/GC, но не могут воспроизвести старый navigation query.
Queued callbacks не дренируются внутри synchronous input scope.

`Scripts/TestInputNavigation.ps1 -SkipBuild -Publish`: fresh export/generation и
whole-module preflight, SObjectWidget input callbacks с custom NavigationConfig,
native repeat timing без sleep, stale/user/frame guards, panic, Destroy и reload.
Контракт: `Bindings/InputNavigation.contract.json`; пример/fixture:
`Tests/InputNavigation/probe.das`; отчёт: `logs/navigation-validation.json`.
Физический ввод, видимый focus traversal, полные Foundation/PIE/packaged не
проверяются этим целевым сценарием.

Итоговая проверка: 24 native/runtime + 8 новых generator tests — PASS.
Связанные регрессии: EventReply 33, Drag 47, InputEvents 38 — PASS.
UMG/Input: 407 ready, 110 export blocked, 0 generation blocked.
