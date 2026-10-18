# События фокуса UMG

`UDasScriptUserWidget` поддерживает две дополнительные записи `InputHandlers`:

| Событие | Экспортируемая функция daScript |
| --- | --- |
| `OnFocusReceived` | `(geometry:UEGeometry; event:UEFocusEvent):UEEventReply` |
| `OnFocusLost` | `(event:UEFocusEvent):void` |

Подключение — `require ue_widget_events`, регистрация через существующий
`ue_widget_bind_input(widget, script, event, handler)` или Blueprint/native
`BindInputHandler`. Пустое имя обработчика снимает привязку. Виджет должен быть
сконструирован, находиться в игровом мире и получать фокус по правилам Unreal
(включая focusable и отображение). Регистрация сама по себе фокус не устанавливает.

```dascript
options gen2
require ue_widget_events

[export]
def focused(geometry:UEGeometry; event:UEFocusEvent):UEEventReply {
    let user = ue_focus_user(event) // uint
    let cause = ue_focus_cause(event) // например, "Navigation"
    return ue_handled()
}

[export]
def unfocused(event:UEFocusEvent):void {
    let cause = ue_focus_cause(event)
    // обновить состояние интерфейса
}

// Для уже созданного принадлежащего VM widget:
// ue_widget_bind_input(widget, "menu.das", "OnFocusReceived", "focused")
// ue_widget_bind_input(widget, "menu.das", "OnFocusLost", "unfocused")
```

`UEFocusEvent` — непрозрачный неизменяемый снимок `FFocusEvent`. Он содержит
номер пользователя и причину: `Mouse`, `Navigation`, `SetDirectly`, `Cleared`,
`OtherWidgetLostFocus`, `WindowActivate`. В нём нет заимствованного `WidgetPath`,
ссылки на Slate user или разрешения менять фокус. Снимок можно сохранять и
клонировать; GC учитывает его в общей ограниченной таблице значений VM.
Нулевой handle и неизвестная native причина отвергаются. Создание из числового
токена, `ue_property`, defaults и generic отражённые вызовы с FocusEvent не открыты.

Получение фокуса выполняется синхронно в native стеке и использует существующий
EventReply bridge: команды имеют слабые ссылки на виджеты и эпоху callback.
При ошибке или Destroy до возврата результат не публикуется; действует native
parent fallback. Использовать reply предыдущего callback нельзя. `OnFocusLost`
не предоставляет EventReply authority: можно изменять состояние, но нельзя
создавать reply-команды из уведомления без возвращаемого результата.
`LastInputError` содержит причину отказа, успешный callback очищает её.

Повторный вход в VM из синхронной смены фокуса отклоняется. NativeDestruct
освобождает сессию через общий ReleaseObject; удаление активной VM откладывается
до завершения исполнения. Конфигурация имён обработчиков сохраняется для
повторного построения виджета, но старые VM, reply и snapshot tokens не переносятся.
Hot reload повторно проверяет обе сигнатуры. Неподходящий кандидат сохраняет
рабочую версию; смена ScriptFile освобождает предыдущую сессию. Native closures
и указатели на функции старой VM в виджете не хранятся.

`OnFocusChanging`, `OnAddedToFocusPath`, `OnRemovedFromFocusPath`, физическое
перемещение фокуса между окнами и авторские `[ue_event]`-методы сюда не входят.
Число generated API-функций не увеличивается: это native callback bridge.

Целевая проверка: `Scripts/TestFocusEvents.ps1` (или `-SkipBuild` с актуальными
DLL), сценарий `Tests/FocusEvents/probe.das`, отчёт `logs/focus-validation.json`.
Runner использует NullSlate и настоящие `SObjectWidget` callbacks, проверяет
неизменность исходников, DLL и Engine BuildId; полные PIE/packaged не запускает.


## Проверка 2026-10-18

UE 5.8.3 Editor Development build — PASS. Focus commandlet — PASS, 35 проверок;
соседний generated EventReply сценарий — PASS, 33 проверки. Проверены получение
и потеря фокуса через SObjectWidget, user/cause, сохранение снимка после GC,
reply-команды и отзыв прежнего reply, запрет reply из void notification,
ошибки и сигнатуры, reentry, Destroy в обоих callbacks, реконструкция и teardown.
Reload проверен с рабочим кандидатом, ошибкой компиляции и компилируемым
кандидатом, удаляющим обязательный `lost`: оба отказа сохраняют рабочую VM.
Contract generator EventReply — PASS, 8 тестов; execution-role guards — PASS.
Во время принятого runtime-прогона исходники, DLL и Engine BuildId стабильны,
engine Error/ensure нет. Физический ввод, полные Foundation/PIE/packaged
не запускались. Коммит не создавался.
